#!/usr/bin/env python3
# Copyright 2020-2021 by Vindor Music, Inc.
#
# Convert a soundfont file to Vindor format
#
# usage: convert.py [-h] [--speed SPEED] [--out OUT] [--test] SF2-FILE [INSTRUMENT [INSTRUMENT ...]]
#
# SF2 extraction tool for Vindor.
#
# positional arguments:
#   SF2-FILE       SF2 file to load
#   INSTRUMENT     instrument to extract; blank to list all
#
# optional arguments:
#   -h, --help     show this help message and exit
#   --speed SPEED  speed factor (>1 is faster, <1 slower)
#   --out OUT      Output directory [default 'vindor']
#   --test         Test and play sound files
#                  SF2-FILE      = directory with output
#                  INSTRUMENT... = list of MIDI note numbers
#
# Requires sf2utils:
#   Install with "pip install sf2utils"
#

import sys
import os
import re
import math
import glob
import copy
import argparse
from pprint import pprint

from sf2utils.sf2parse import Sf2File

# output format required by Vindor
soxformat = "-b 16 -c 1 -e signed-integer -r 44100"

# global state variables
maxmidi = 0
maxmap = {}
minmidi = 9999
minmap = {}
seen = {}

def midi2freq(note):
  """For a given MIDI note, return frequency."""
  return math.pow(2.0, (note-69.0)/12.0)*440.0

def convert(infile, outfile):
  """Use SOX to convert a file into a format suitable for Vindor."""
  cmd = "sox %s '%s' '%s'" % (soxformat, infile, outfile)
  os.system(cmd)

def split2(line):
  """Split a key=value line and standardize."""
  k = line.split(" ")
  first = k[0].split('=')[1]
  second = k[1].split('=')[1]
  return first, second

def loadline(map, line):
  """Load a line of key/value pairs into a dict."""
  k = line.strip().split(" ")
  for i in k:
    #print i
    try:
      k,v = i.split('=')
      try:
        map[k] = int(v)
      except:
        map[k] = v
    except: pass

def getsilence(file):
  """Return offset into file by skipping any initial silence."""
  out = "silence.raw"
  cmd = "sox %s %s silence 1 0.3 2%% 1 0.1 5%%" % (file, out)
  sz1 = os.path.getsize(file)
  sz2 = os.path.getsize(tmp)
  return (sz1 - sz2) / 2

def stripcomments(text):
  """Strip comments from a line."""
  return re.sub('//.*$', '', text, re.S).strip()

def output(out, filename, map, max):
  """Used by text_parse to output sounds."""
  global maxmidi, maxmap, minmidi, minmap
  print("MAXX",max)
  print(map)
  if "key" in map:
    map["lokey"] = map["key"]
    map["hikey"] = map["key"]

  if map["lokey"] < 24: map["lokey"] = 24
  if map["hikey"] > 109: map["hikey"] = 109

  if max == 1:
    if maxmidi >= 109: return
    map = maxmap
    map["lokey"] = maxmidi+1
    map["hikey"] = 109

  if max == 2:
    if minmidi <= 24: return
    map = minmap
    map["lokey"] = 24 
    map["hikey"] = minmidi-1

  for midi in range(map["lokey"], map["hikey"]+1):
    if midi in seen: continue

    rstart = map["offset"]
    rend = map["end"] - map["offset"]
    if rend < 0: continue

    if map["loop_start"] > map["loop_end"]: continue

    seen[midi] = 1

    infreq = midi2freq(int(map["pitch_keycenter"]))
    outfreq = midi2freq(midi)
    speed = outfreq / infreq

    outfile = "out/play%d.raw" % midi
    cmd = "sox %s %s trim %ds %ds speed %f" % (filename, outfile, rstart, rend, speed)
    #cmd = "sox %s %s trim %ds %ds silence 1 0.3 2%% 1 0.1 5%% speed %f" % (filename, outfile, rstart, rend, speed)
    print(cmd)
    os.system(cmd)

    #sz_expect = rend / speed
    #sz_actual = os.path.getsize(outfile) / 2
    #off = rstart / speed + (sz_expect - sz_actual)
    #print sz_expect, sz_actual
    off = rstart / speed
    lstart = map["loop_start"] / speed - off
    lend   = map["loop_end"] / speed - off
    out.write("%d,%d,%d,%d\n" % (midi, lstart, lend, lend-lstart))

    if midi > maxmidi:
      maxmidi = midi
      maxmap = copy.deepcopy(map)

    if midi < minmidi:
      minmidi = midi
      minmap = copy.deepcopy(map)

def text_parse(sf2, out, inst):
  """Parse SFZ in text format."""
  out = open("%s/play.inf" % out, "w")

  filename = ""
  mapx = {}

  stage = 0

  f = open(sf2, "r")
  for line in f:
    line = line.strip()
    line = stripcomments(line)
    if line.startswith("<"): 
      if stage == 2:
        output(out, filename, mapx, 0)
      stage = 0
      if line == "<global>": stage = 1
      if line == "<region>":
        mapx = {}
        stage = 2
    else:
      k = line.split("=")
      if stage == 1:
        if k[0] == 'sample': filename = k[1]
      if stage == 2:
        loadline(mapx, line)
  output(out, filename, mapx, 1)
  output(out, filename, mapx, 2)

def text_list_instruments(filename):
  """List instruments in an SFZ file."""
  f = open(sf2, "r")
  for line in f:
    line = line.strip()
    line = stripcomments(line)
    if 'region_label=' in line:
      k, v = line.split('=')
      print(v)

def binary_parse(filename, outdir, inst):
  """Parse SF2 binary file and extract instrument by name."""
  with open(filename, 'rb') as file:
    sf2_file = Sf2File(file)
    for i in sf2_file.instruments:
      if inst != None and i.name != inst: continue
      print(i.pretty_print())
      out = "%s/%s" % (outdir, i.name.replace(" ","_"))
      try:
        os.makedirs(out)
      except:
        pass

      tmp = "tmp/%s" % i.name.replace(" ","_")
      try:
        os.makedirs(tmp)
      except:
        pass
        
      pf = open("%s/%s.txt" % (out, inst), "w")
      pf.close()
      inf = open("%s/play.inf" % out, "w")
      print(i.name)
      inf.write("disp=%s\n" % inst)
      inf.flush()

      for bag in i.bags:
        print(bag.pretty_print())

        try:
          print("Pitch: ", bag.sample.original_pitch)
        except:
          print("ERR: no original pitch")
          continue

        # try:
        if bag.key_range is None: 
          print("No key range, assuming full range")
          key_range = [24, 90]
        else:
          key_range = bag.key_range
          # continue

        rmidi = bag.sample.original_pitch
        if bag.base_note is not None:
          rmidi = bag.base_note
        if rmidi is None:
          print("Rmidi empty")
          continue

        print("pitch original %d" % rmidi)
        print(bag.base_note)
        print(bag.sample.pitch_correction)
        pprint(bag)
        print("Range: ", key_range)
        print("Sample: ", bag.sample)

        rawfile = "%s/play%d.orig.raw" % (tmp,rmidi)
        outf = open(rawfile, "wb")
        outf.write(bag.sample.raw_sample_data)
        outf.close()
        print("write",len(bag.sample.raw_sample_data))

        for midi in range(key_range[0], key_range[1]+1):
          if midi < 24 or midi > 109: continue

          infreq = midi2freq(rmidi) / float(bag.sample.sample_rate) * 44100.0
          outfreq = midi2freq(midi)
          speed = outfreq / infreq * args.speed
          outfile = "%s/play%d.wav" % (out,midi)
          cmd = "sox %s '%s' '%s' speed %f" % (soxformat, rawfile, outfile, speed)
          print(cmd)
          os.system(cmd)

          lstart = bag.sample.start_loop / speed
          lend   = bag.sample.end_loop / speed
          inf.write("%d,%d,%d\n" % (midi, lstart+0.5, lend+0.5))
          inf.flush()
      inf.close()
    print("Scan for %s complete" % inst)

def test(dir, notes):
  """Test a generated directory by playing all its notes."""
  if notes is None or len(notes)==0:
    notes = [69]
  notes = set(map(int, notes))
  
  with open("%s/play.inf" % dir) as inf:
    for line in inf:
      if ',' not in line: continue
      a = list(map(int, line.split(",")))
      midi = a[0]
      if midi not in notes: continue
      print("Play %d Start %d ; End %d" % (midi, a[1], a[2]))
      cmd = "sox %s '%s/play%d.wav' p1.raw trim 0 %ds" % (soxformat, dir, midi, a[1])
      os.system(cmd)
      cmd = "sox %s '%s/play%d.wav' p2.raw trim %ds =%ds" % (soxformat, dir, midi, a[1]+1, a[2])
      os.system(cmd)
      cmd = "sox %s '%s/play%d.wav' p3.raw trim %ds" % (soxformat, dir, midi, a[2]+1)
      os.system(cmd)
      
      os.system("cat p1.raw > pout.raw")
      for i in range(1, 10):
        os.system("cat p2.raw >> pout.raw")
      os.system("cat p3.raw >> pout.raw")
      os.system("rm p1.raw p2.raw p3.raw")
      os.system("play %s pout.raw" % soxformat)
      os.system("rm pout.raw")

def binary_list_instruments(filename):
  """List instruments in an SF2 file."""
  with open(filename, 'rb') as file:
    sf2_file = Sf2File(file)
    for i in sf2_file.instruments:
      print(i.name)

def list_instruments(filename):
  if filename.lower().endswith('.sfz'):
    return text_list_instruments(filename)
  else:
    return binary_list_instruments(filename)

def parse(filename, out, inst):
  if filename.lower().endswith('.sfz'):
    return text_parse(filename, out, inst)
  else:
    return binary_parse(filename, out, inst)

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description='SF2/Z extraction tool for Vindor.')
  parser.add_argument('sf2', metavar='SF2/Z-FILE',
                    help='SF2/Z file to load')
  parser.add_argument('inst', metavar='INSTRUMENT', nargs='*',
                    help='Instrument name to extract; blank to list all')
  parser.add_argument('--speed', default=1.0, type=float,
                    help='Speed factor (>1 is faster, <1 slower)')
  parser.add_argument('--out', default="vindor", type=str,
                    help='Output directory [default "vindor"]')
  parser.add_argument('--test', action='store_true',
                    help='Test instrument output')
  args = parser.parse_args()

  if args.test:
    test(args.sf2, args.inst)
  elif len(args.inst) == 0:
    list_instruments(args.sf2)
  else:
    for inst in args.inst:
      parse(args.sf2, args.out, inst)

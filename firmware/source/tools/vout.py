#!/usr/bin/python
import os
import sys
import glob
import time

def process(fxn, n, str):
  # print("process",fxn,n,str)
  global fp
  global ser
  if fxn == 1:  # open
    (fmode, fname) = str.split(":", 2)
    fp = open(fname, fmode)
    # os.write(ser, '0x00')
    print("open %s" % fname)
  elif fxn == 2:  # close
    fp.close()
    # os.write(ser, '0x00')
    print("close")
  elif fxn == 3:  # read
    d = fp.read(n)
    os.write(ser, chr(l), 1)
    os.write(ser, d, l)
  elif fxn == 4:  # write
    fp.write(str)
    # os.write(ser, '0x00')


def filehost():
  global ser
  fxn = os.read(ser, 1)  # function
  # print("@",fxn,"%")
  fxn = ord(fxn)
  n = os.read(ser, 1)  # length
  n = ord(n)
  s = os.read(ser, n)  # data
  process(fxn, n, s)


def trydev(devlist):
  for file in devlist:
    # if file in seen: continue
    #print("*****open %s" % file)
    try:
      ser = os.open(file, os.O_RDWR)
    except:
      #print("fail")
      continue
    print("-------")
    while True:
      try:
        c = os.read(ser, 1)
      except OSError:
        break
      if len(c) > 0:
        if ord(c) == 1:  # control
          # print('!',c,'#')
          filehost()
        else:
          sys.stdout.write(c)
    print("*****close %s" % file)
    # seen[file] = 1

while True:
  devlist = glob.glob("/dev/cu.usbmodem*")
  time.sleep(1)
  trydev(devlist)
  devlist = glob.glob("/dev/cu.w*")
  time.sleep(1)
  trydev(devlist)
  devlist = glob.glob("/dev/cu.usbser*")
  time.sleep(1)
  trydev(devlist)


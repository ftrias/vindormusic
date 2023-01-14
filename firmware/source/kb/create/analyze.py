#!/usr/bin/env python3
# 

import sys
from parse import *
import argparse

import models
import convert
import pretty

# import future
import textwrap

def convertPretty(b):
  inx = sys.stdin.read()
  outx, m, b = convert.convertPrettyToInternal(inx, b)
  outp = ",\n".join(['"{}"'.format(x) for x in outx])
  o = convert.getOctForBits(b)
  print("""fingerings = FingerModel(
name="instrument",
desc="Instrument",
mask={},
bits={},
oct={},
model=[
{}
])""".format(m, b, o, outp))

def model2cpp(inst, outfile):
    print("MODEL", inst.name)

    BinTree.expand = inst.expand
    t, ver, err = processModel(inst)
    BinTree.expand = False

    if inst.mask is None:
        inst.mask = hex((1<<inst.bits)-1)

    result = """// {name}
const int fingerlist_{name}_octave = {oct};
const int fingerlist_{name}_mask = {mask};
const int fingerlist_{name}_bits = {bits};
const int16_t fingerlist_{name}_table[] = {{
""".format(name=inst.name, bits=inst.bits, mask=inst.mask, oct=inst.oct)

    txt = ", ".join(map(str, t))
    txt += ", END_FINGERING"
    wrapper = textwrap.TextWrapper()
    wrapper.initial_indent = "    "
    wrapper.subsequent_indent = "    "
    result += wrapper.fill(txt)

    result += "};\n"

    print(result, file=outfile)

def model2vnd(inst):
    print("MODEL", inst.name)
    str = model2vndstr(inst)
    with open("%s.vnd" % inst.name.lower(), "w") as fp:
        fp.write(str)
    return str

def model2vndstr(inst):
    table, ver, err = processModel(inst)
    str = makeVnd(table, inst.bits, inst.mask, inst.name, inst.desc, inst.oct)
    return str

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--cpp", help="Produce C code", action="store_true")
parser.add_argument("-v", "--vnd", help="Produce VND files and HTML", action="store_true")
parser.add_argument("-g", "--html", help="Produce HTML charts", action="store_true")
parser.add_argument("-t", "--test", help="Test one model", action="store_true")
parser.add_argument("-o", "--out", help="Output filen name")
parser.add_argument("-p", "--pretty", help="Convert pretty using bits")
args = parser.parse_args()

if args.out:
    outfile = open(args.out, "w")
else:
    outfile = sys.stdout

if args.pretty:
    convertPretty(int(args.pretty))

if args.test:
    t = processModel(models.clarinet2)
    r = testTable(t, "10000000000")
    print(r, allNotes[r])

cppmodels = (models.saxFingerings, models.clarinet, models.recorder, models.trumpet, models.flute)
if args.cpp:
    for inst in cppmodels:
        model2cpp(inst, outfile)

publicmodels = (models.clarinet2, models.oboe, models.tuba, models.basic, models.basic2, models.tinwhistle)
vndmodels = (models.handicapFingerings,) + publicmodels
if args.vnd:
    print('<pre>\n{"kb":[', file=outfile)
    for inst in vndmodels:
        print(model2vnd(inst), file=outfile)
    print(']}\n</pre>', file=outfile)

if args.html:
    with open("charts.html", "w") as cfile:
        s = []
        for inst in cppmodels + publicmodels:
            reformatFingering(inst)
            print(pretty.renderSVG(inst), file=cfile)

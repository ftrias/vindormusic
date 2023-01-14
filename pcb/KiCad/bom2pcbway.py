#!/Users/ftrias/anaconda/bin/python -I

import sys
sys.path.append('/Users/ftrias/Documents/Source/Vindor/vindor/hardware/pcb/KiCad')

# Import the KiCad python helper module
import kicad_netlist_reader as k
import sys

# Generate an instance of a generic netlist, and load the netlist tree from
# the command line option. If the file doesn't exist, execution will stop
net = k.netlist(sys.argv[1])

# Open a file to write to, if the file cannot be opened output to stdout
# instead
fname = sys.argv[2]+"-BOM.csv"
try:
    f = open(fname, 'wb')
except IOError:
    e = "Can't open output file for writing: " + sys.argv[2]
    print( __file__, ":", e, sys.stderr )
    f = sys.stdout

print("Output file:", fname)

out = ("Reference", "Value", "Footprint", "Manufacturer", "MPN")
f.write(",".join(out))
f.write("\n")

keep = {}
count = {}

# Output all of the component information (One component per row)
for c in net.components:
    dnp = c.getField("DNP")
    mpn = c.getField("MPN")
    if dnp == "1": continue
    line = (c.getRef(), c.getValue(), c.getFootprint(), "", mpn)
    if mpn in count:
        count[mpn] += 1
    else:
        count[mpn] = 1
    keep[c.getRef()] = line

for ref in sorted(keep.keys()):
    f.write(",".join(keep[ref]))
    f.write("\n")

print("Unique parts", len(count.keys()))

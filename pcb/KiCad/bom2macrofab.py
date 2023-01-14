#!/usr/local/bin/python
#!/Users/ftrias/anaconda/bin/python -I

import sys
sys.path.append('/Users/ftrias/Documents/Source/Vindor/vindor/hardware/pcb/KiCad')

print(sys.executable)

# Import the KiCad python helper module
import kicad_netlist_reader as k
# import csv
import xlsxwriter
import sys

# Generate an instance of a generic netlist, and load the netlist tree from
# the command line option. If the file doesn't exist, execution will stop
net = k.netlist(sys.argv[1])

# Open a file to write to, if the file cannot be opened output to stdout
# instead
try:
    f = open(sys.argv[2]+".xlsx", 'wb')
except IOError:
    e = "Can't open output file for writing: " + sys.argv[2]
    print( __file__, ":", e, sys.stderr )
    f = sys.stdout

# Create a new csv writer object to use as the output formatter
xls = xlsxwriter.Workbook(f)
sheet = xls.add_worksheet()

out = ("Reference", "Value", "Footprint", "MPN")
for i, v in enumerate(out):
    sheet.write(0, i, v)

num = 1

# Output all of the component information (One component per row)
for c in net.components:
    dnp = c.getField("DNP")
    if dnp == "1": continue
    line = (c.getRef(), c.getValue(), c.getFootprint(),
        c.getField("MPN"))
    for i, v in enumerate(line):
        sheet.write(num, i, v)
    num += 1

xls.close()

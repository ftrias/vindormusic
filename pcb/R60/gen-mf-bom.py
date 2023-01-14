#!/usr/bin/env python

import csv
import xlsxwriter

def makecsv():
  out = ("Reference", "Value", "Footprint", "MPN")
  with open("VindorR56.xlsx", "w") as ofp:
    ocsv = csv.writer(ofp, delimiter=',', quotechar='"')
    ocsv.writerow(out)
    with open("VindorR56", "r") as fp:
      icsv = csv.DictReader(fp, skipinitialspace=True) 
      for line in icsv:
        if "DNP" in line:
          if line["DNP"]=="1": continue
        p = []
        for i, v in enumerate(out):
          p.append(line[v])
        ocsv.writerow(p)
  
def makexlsx():
  out = ("Reference", "Value", "Footprint", "MPN")
  with xlsxwriter.Workbook("VindorR56.xlsx") as bom:
    sheet = bom.add_worksheet()
    for i, v in enumerate(out):
      sheet.write(0, i, v)
    num = 1
    with open("VindorR56", "r") as fp:
      icsv = csv.DictReader(fp, skipinitialspace=True) 
      for line in icsv:
        if "DNP" in line:
          if line["DNP"]=="1": continue
        for i, v in enumerate(out):
          sheet.write(num, i, line[v])
        num += 1


makexlsx()

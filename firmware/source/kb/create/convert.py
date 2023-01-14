import parse
import models
import re
import pretty

def downloadme():
  text = document.getElementById('out').value
  download("fingering.vnd", text)
  
def report(txt):
    document.getElementById('out').value = txt

def swap(items, i0, i1):
  t = items[i0]
  items[i0] = items[i1]
  items[i1] = t

# sofege = ("DO","RE","MI","FA","SOL","LA","SI","DO")

# def standardizeNote(note):
#   oct = re.findall(r'\d+', note)
#   note = re.sub(r'\d+','', note)
#   acc = re.findall(r'#b$', note)
#   note = re.sub(r'#b$+','', note)
#   a = re.findall(r'^[A-Za-z]+', note)

# def convertPrettyToInternal(text, b):
#   m = (1<<b)-1

#   f = []
#   for line in text.strip().split("\n"):
#     if line.startswith("#"): continue
#     if line.startswith(";"): continue
#     k, n = line.strip().split(":")
#     k = re.sub(r'[^0-9*]','',k)
#     # f.append(k)
#     items = list(k)
#     if len(items) > b+1:
#       report("Line too long: {}".format(line))
#       return
#     if b == 10:
#       items.insert(3,'0')
#     # elif b == 11:
#     #   swap(items, 3, 5)
#     #   swap(items, 4, 5)
#     # elif b == 12:
#     #   swap(items, 0, 1)
#     #   swap(items, 4, 6)
#     #   swap(items, 5, 6)
#     # elif b == 14:
#     #   swap(items, 2, 3)
#     #   swap(items, 6, 8)
#     #   swap(items, 7, 8)
#     f.append("b/" + "/".join(items) + "/" + n)
#   if b == 10:
#     m = 0b11101111111
#     b = 11

#   return f, m, b

def getOctForBits(b):
  if b == 10 or b == 11 or b == 12 or b == 14:
    if b == 14:
      return 0
  else:
    return -1
  return 1

def runme():
  try:
    b = int(document.getElementById('bits').value)
  except:
    report("Bits must be a number")
    return

  o = getOctForBits(b)
  if o == -1:
    report("Only bits 10, 11, 12 or 14 permitted")
    return

  inx = document.getElementById('in').value

  inst = models.FingerModel(
    name=document.getElementById('name').value,
    desc=document.getElementById('desc').value,
    bits=b,
    model=inx)

  # try:
  parse.reformatFingering(inst)
  table, diag, errors = parse.processModel(inst)
  strx = parse.makeVnd(table, inst.bits, inst.mask, inst.name, inst.desc, inst.oct)
  # except Exception as ex:
  #   strx = ["Error"]
  #   diag = [repr(ex), ex.stack]

  document.getElementById('diag').value = "\n".join(diag)

  if len(errors) > 0:
    document.getElementById('out').value = "\n".join(errors)
  else:
    document.getElementById('out').value = strx
    document.getElementById('rendersvg').innerHTML = pretty.renderSVG(inst)

#!/usr/bin/env python
# 

import models
import re

class BinTree:
    expand = False
    idcount = 0

    def __init__(self, value):
        self.left = None
        self.right = None
        self.value = value
        self.id = self.idcount
        self.idcount += 1

    def addFingering(self, fingering, note):

        if self.expand and "*" in fingering:
            self.addFingering(fingering.replace("*","0", 1), note)
            self.addFingering(fingering.replace("*","1", 1), note)
            return

        curSubTree = self

        # create empty nodes in the correct path
        # if the node already exists, follow the existing tree
        for finger in fingering:
            if curSubTree.value is not None:
                raise Exception("tried path {} but leaf in path already set with {}!".format(curSubTree.value, note))

            if finger == '1':                
                if curSubTree.right == None:
                    curSubTree.right = BinTree(None)
                curSubTree = curSubTree.right
            elif finger =='0':
                if curSubTree.left == None:
                    curSubTree.left = BinTree(None)
                curSubTree = curSubTree.left
            else:
                if (curSubTree.left == None) and (curSubTree.right == None):
                    curSubTree.left = BinTree(None)
                    curSubTree.right = curSubTree.left
                    curSubTree = curSubTree.left
                else:
                    self.addFingering(fingering.replace("*","0", 1), note)
                    self.addFingering(fingering.replace("*","1", 1), note)
                    return

        # fill final node with note value
        if curSubTree.value != None:
            if curSubTree.value != note:
                raise Exception("tried adding {}/{}, but leaf already occupied with {}!".format(fingering, curSubTree.value, note))
        curSubTree.value = note

    def treeFromTupList(self, tupList):
        for fingering, note in tupList:
            self.addFingering(fingering, note)


    def checkFingering(self, fingering):
        curSubTree = self
        for finger in fingering:

            # if this tree has no more children, return value, even if
            # there are more buttons to check

            if (curSubTree.left == None) and (curSubTree.right == None):
                break
            elif finger == '1':
                curSubTree = curSubTree.right

            # * character will default to left branch, but doesn't matter
            # because left and right branches should give same result
            else:
                curSubTree = curSubTree.left
            
            if curSubTree is None:
                return None

        return curSubTree.value

    def addLeaf(self, side, value):
        if side == 0:
            self.left = BinTree(None)
            self.left.value = value
        else:
            self.right = BinTree(None)
            self.right.value = value

    def trimLeaf(self):
        if self is None: return
        if self.left is not None and self.right is not None:
            if self.left.value is not None and self.right.value is not None:
                if self.left.value == self.right.value:
                    self.value = self.left.value
                    self.left = None
                    self.right = None
            else:
                self.left.trimLeaf()
                self.right.trimLeaf()

    # def myid(self, side=None):
    #     if side is None:
    #         return self.id
    #     elif side == 0:
    #         if 
    #         return self.left.id

    def display(self, indent=''):
        if self is None: return
        print(indent, end='')
        if self.value is not None:
            print(self, self.value, type(self.value))
            return
        print(self, self.left, self.right)
        if self.left is not None: 
            self.left.display(indent+'0')
        else:
            print(indent+'0', 'MISSING')
        if self.right is not None: 
            self.right.display(indent+'1')
        else:
            print(indent+'1', 'MISSING')
        
def swap(items, i0, i1):
    t = items[i0]
    items[i0] = items[i1]
    items[i1] = t

def reformatFingeringLine(line, b):
    if line.startswith("b"):
        return line
    else:
        line = line.strip()
        if len(line) == 0: return None
        if line.startswith("#"): return None
        if line.startswith(";"): return None
        k, n = line.strip().split(":")
        k = re.sub(r'[^0-9*]','',k)
        items = list(k)
        if len(items) > b+1:
            return None
        if b == 10:
            items.insert(3, '0')
        elif b == 11:
            swap(items, 3, 5)
            swap(items, 4, 5)
        elif b == 12:
            swap(items, 0, 1)
            swap(items, 4, 6)
            swap(items, 5, 6)
        elif b == 14:
            swap(items, 2, 3)
            swap(items, 6, 8)
            swap(items, 7, 8)
        result = "b/" + "/".join(items) + "/" + n
        return result

def reformatFingering(inst):
    if isinstance(inst.model, str):
        inst.model = inst.model.split("\n")

    results = []
    for entry in inst.model:
        entry = reformatFingeringLine(entry, inst.bits)
        if entry is None: continue
        results.append(entry)
    inst.model = results

    if inst.bits == 10: 
        inst.bits = 11
        
def makeTupList(fingerings, bits):
    tupList = []

    if isinstance(fingerings, str):
        fingerings = fingerings.split("\n")

    if bits == 10: 
        rbits = 11
    else:
        rbits = bits

    # split the fingering and the note
    for entry in fingerings:
        entry = reformatFingeringLine(entry, bits)

        if entry is None:
            continue

        # partition = entry.rpartition('/')
        # fingering = partition[0]
        # note = partition[2]
        i = entry.rfind("/")
        fingering = entry[:i]
        note = entry[i+1:]

        # removes spaces, slashes, and b
        for delchar in ' b/':
            fingering = fingering.replace(delchar,'')

        # fills shorter entries with *'s so that all entries
        # are of length "bits"
        while (len(fingering) < bits):
            fingering += "*"

        tupList.append((fingering, note))
    
    return tupList, rbits

def fingerSort(fingTup):
    fingering = fingTup[0]
    subFingering = ""
    for char in fingering:
        if char == '*':
            char = '0'
        subFingering = subFingering + char

    return int(subFingering, 2)

def shortFingerings(tupList):
    # sort the list of tuples bitwise by their fingering
    tupList.sort(key=fingerSort)

    # length of binary string representing each fingering
    fingLength = len(tupList[0][0])
    short = []

    # since we check two indices at a time, only loop until second to last item
    for listIndex in range(len(tupList) - 1):

        # if all chars are different, the last common char is at index len
        # (technically one char beyond the end)
        lastCommon = fingLength

        # get fingering binary string by looking at the first item in two
        # adjacent tuples from the list
        fing1 = tupList[listIndex][0]
        fing2 = tupList[listIndex + 1][0]

        # since using negative notation to look at chars form the end of the
        # string, loop from 1 to length (inclusive, so we want to actually check
        # the last index, hence the + 1)
        for fingIndex in range(1, fingLength + 1):

            # if the current two characters are different, then find the last index
            # where the characters are all the same. then slice first fingering,
            #  leaving the first char that differentiates it from its neighbor
            if fing1[-fingIndex] != fing2[-fingIndex]:
                lastCommon = fingLength - fingIndex + 1
                break

        # slice this fingering at the index where following characters would be redundant
        # since we can distinguish between fingerings by the last char
        # and also strip *'s from the end, which are redundant
        shortened = fing1[:lastCommon].rstrip("*")
        short.append((shortened, tupList[listIndex][1]))

    # since we only loop until index - 1, we have to add in the last entry
    # (we can't go around the loop again since there is nothing left to compare
    # this fingering to)
    # strip *'s from end
    lastFingering = tupList[len(tupList) - 1][0].rstrip("*")
    lastNote = tupList[len(tupList) - 1][1]
    short.append ((lastFingering, lastNote))
    return short

def calcMask(tupList, bits):
    # first = tupList[0][0]
    # print("TEST", first, tupList[0])
    # if bits > len(first):
    #     first += "0" * (bits - len(first))
    # val = list(first)
    # mask = [0] * len(val)
    mask = [0 for i in range(bits)]
    first = ''.join(map(str, mask))
    # print("MASK FIRST", bits, first, mask)
    for k, n in tupList:
        # print(k)
        for i, v in enumerate(k):
            if mask[i] == 0 and v!='*' and v != first[i]:
                # print("diff", i)
                mask[i] = 1
    result = 0
    # print(mask)
    for i in mask:
        result = (result << 1) + i
    return result

test_array = [-1] + [-2]
# print("!".join(map(str, test_array)), type(test_array))
if test_array[1] == -2:
    js_merge = False
else:
    js_merge = True

def mergeArrays(a1, a2, a3=None):
    global js_merge
    if js_merge:
        if a3 is None:
            return a1.concat(a2)
        else:
            return a1.concat(a2, a3)
    if a3 is None:
        return a1+a2
    else:
        return a1+a2+a3

def makeTable(tree):
    if tree:
        if tree.left == None and tree.right == None:
            note = int(noteNameToNumber(tree.value))
            return [note]
        elif tree.left == tree.right:
            leftTable = makeTable(tree.left)
            # return [0] + leftTable ##return [1] + leftTable
            return mergeArrays([0], leftTable)
        else:
            leftTable = makeTable(tree.left)
            rightTable = makeTable(tree.right)
            c = len(leftTable) + 1
            # return [c] + leftTable + rightTable
            return mergeArrays([c], leftTable, rightTable)
    else:
        return []

def adjustTable(table):
    newtable = []
    for i, v in enumerate(table):
        if v > 0:
            newtable.append(v + i)
        else:
            newtable.append(v)
    return newtable

def tableChecker(table, fingering):
    index = 0
    for char in fingering:
        if table[index] < 0:
            return table[index]
        elif char == '0':
            index += 1
        else:
            index += table[index]
    return table[index]

def printTable(table):
    index = 0
    for entry in table:
        print(str(index) + "   " + str(table[index]))
        index += 1

noteStep = {"C":0,"D":2,"E":4,"F":5,"G":7,"A":9,"B":11}
def noteNameToOrder(note):
    for c in note:
        if c in noteStep:
            num = noteStep[c]
        elif c == "#":
            num += 1
        elif c == "b":
            num -= 1
        else:
            oct = int(c) - 1
            num += oct * 12
    return num

def noteNameToNumber(note):
    return -(noteNameToOrder(note) + 3)

allNotes = ["B0",
    "C","C#","D","D#","E","F","F#","G","G#","A","A#","B",
    "C2","C2#","D2","D2#","E2","F2","F2#","G2","G2#","A2","A2#","B2",
    "C3","C3#","D3","D3#","E3","F3","F3#","G3","G3#","A3","A3#","B3",
    "C4","C4#","D4","D4#","E4","F4","F4#","G4","G4#","A4","A4#","B4",
    "C5"
    ]

def noteNameToNumber2(note):
    if note == "Bb":
        note = "A#"
    elif note == "Eb":
        note = "F#"
    index = allNotes.index(note)
    return -(index + 2)

def checkGapsInTable(table, bits):
    counter = 0
    for fingAsInt in range(2**bits):
        fingAsString = bin(fingAsInt)[2:]
        while len(fingAsString) < bits:
            fingAsString = "0" + fingAsString
        try:
            tableChecker(table, fingAsString)
        except:
            print("The fingering {} is not accounted for".format(fingAsString))
            counter += 1
    return "there are {} fingerings unaccounted for".format(counter)

def makeVnd(table, bits, mask, name, desc, useOctave):
    # for index in range(len(table)):
    #     if table[index] > 0:
    #         table[index] += index
    try:
        if mask.startswith("0x"):
            mask = int(mask, 16)
        if mask.startswith("0b"):
            mask = int(mask, 2)
    except:
        pass

    # tree = "[" + ','.join([int(x) for x in table]) + "]"
    strx = '''
"type":"fingering",
"name": "{}",
"desc": "{}",
"bits": {}, 
"mask": {},
"usectave": {},
"tree": {}
'''.format(name, desc, bits, mask, useOctave, table)
    return "{"+strx+"}"

def i2b(n, bits):
    result = ""
    for b in range(bits-1, -1, -1):
        if n & (1<<b):
            result += "1"
        else:
            result += "0"
    return result

def testTable(t, fingering):
    current = 0
    for bit in fingering:
        if t[current] == 0:
            current += 1
            continue
        if bit == "1":
            current = t[current]
            if current <= 0:
                return None
        else:
            current += 1
        
        nxt = t[current]

        if nxt < 0:
            return - nxt - 2

def verifyTable(t, bits):
    results = []
    for i in range(2**bits):
        s = i2b(i, bits)
        r = testTable(t, s)
        if r is None:
            results.append("TABLE MISSING {}".format(s))

    print("\n".join(results))
    return results

def followPathDown(root, lr):
    if root is None:
        return None
    if root.value is not None:
        return root.value
    if lr:
        if root.left is not None:
            return followPathDown(root.left, lr)
        else:
            return followPathDown(root.right, lr)
    else:
        if root.right is not None:
            return followPathDown(root.right, lr)
        else:
            return followPathDown(root.left, lr)

def verifyTreeRecurse(results, root, prefix=""):
    if root is None: return
    if root.value is not None:
        # print("PATH end", root.value, prefix)
        return
    if root.left is None:
        r = followPathDown(root, 0)
        if r is not None:
            results.append("Tree missing {} leaf 0. Assume {}".format(prefix, r))
            root.addLeaf(0, r)
        else:
            raise Exception("Tree missing {} leaf 0. Tree is broken".format(prefix))
    else:
        verifyTreeRecurse(results, root.left, prefix + "0")

    if root.right is None:
        r = followPathDown(root, 0)
        if r is not None:
            results.append("Tree missing {} leaf 1. Assume {}".format(prefix, r))
            root.addLeaf(1, r)
        else:
            raise Exception("Tree missing {} leaf 1. Tree is broken".format(prefix))
    else:
        verifyTreeRecurse(results, root.right, prefix + "1")
    return

def verifyTree(root, prefix=""):
    results = []
    verifyTreeRecurse(results, root, prefix)
    return results

def processModel(inst):
    saxTupList, inst.bits = makeTupList(inst.model, inst.bits)
    saxShort = shortFingerings(saxTupList)
    mask = calcMask(saxShort, inst.bits)
    if inst.mask == 0:
        inst.mask = mask
    if mask != inst.mask:
        print("Mismatch mask", inst.bits, inst.mask, mask)
    saxTree = BinTree(None)
    saxTree.treeFromTupList(saxShort)
    saxTree.trimLeaf()
    # if args.test:
    #     saxTree.display()
    ver = verifyTree(saxTree)
    saxTable = makeTable(saxTree)
    # checkGapsInTable(saxTable, bits)
    adj = adjustTable(saxTable)
    err = verifyTable(adj, inst.bits)
    return adj, ver, err

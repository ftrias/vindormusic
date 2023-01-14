#!/usr/bin/env python
# 

from models import recorder as fingerings

# converts a list of strings of 1's, 0's, and *'s into a list of binary strings, expanding all the
# possibilities from the wildcard character *
def expandEntry(entry: str, branchArray):
    # recursive case, only 1 character left
    if len(entry) <= 1:
        # if the last character is a 1 or 0
        if entry != '*':
            # make new array since we shouldn't mess with the contents of an
            # array we are still reading through
            updatedArray = []

            # add the current character to the beginning of each string in the branchArray
            for branch in branchArray:
                updatedArray.append(entry + branch)
            return updatedArray
        else:

            # we have to double the existing array of branches to add both
            # possible values represented by the wildcard * character
            # since this button 'doesn't matter', there are now two possible fingerings
            # for each fingering/button combination we have already listed
            splitArray = []
            for branch in branchArray:
                 splitArray.append('0' + branch)
                 splitArray.append('1' + branch)
            return splitArray
    else:
        if entry[-1] != '*':
            #adds the last character of entry to beginning strings in branchArray
            #calls itself again with the last character removed
            return (expandEntry(entry[:-1], expandEntry(entry[-1], branchArray)))
        else:
            # if the last char is a wildcard, create all the possible fingerings
            # where either this button is pressed, or isn't pressed
            return (expandEntry(entry[:-1], expandEntry('0', branchArray)) + expandEntry(entry[:-1], expandEntry('1', branchArray)))

# dict of completely expanded fingering and note pairs
finalDict = {}

for entry in fingerings:
    # save note for later by splitting string after the final /
    note = entry.split("/")[-1]

    # removes all the note names, slashes, and starting char
    # only leaving the fingering
    for delchar in ' b/ABCDEFG#b0123':
        entry = entry.replace(delchar,'')

    # expand *'s and loop through all the possible fingerings for a SINGLE entry from original list
    for expandedEntry in expandEntry(entry, ['']):

        # convert base 2 bitstring to base 10 integer index
        entryBase10 = int(expandedEntry, 2)

        # checks to make sure fingering isn't already assigned
        if entryBase10 in finalDict:
            raise Exception('multiple notes assigned to same fingering')

        else:
            # add the corresponding note to every expanded possibility
            finalDict[entryBase10] = note


# # sort by note's value (string)
# for index in sorted(finalDict, key=finalDict.__getitem__):
#     print(str(index) + "   " + finalDict[index])

# sorts by index value
for index in sorted(finalDict):
        print(bin(index) + "   " + finalDict[index])

#!/usr/bin/env python
# 

f = 1.6
bits = 10
silence = 0
maxb = 1 << bits
top = 65535

scale = maxb ** f

print("const int expPressureBits = %d;" % bits)
print("const int expPressureMap[%d] = {" % (maxb+1))
for i in range(silence):
    print("0,", end='')
    if ((i+1) % 20 == 0):
        print("")
for i in range(maxb - silence):
    v = (i ** f) / scale * top
    print("%d," % int(v), end='')
    if ((i+1) % 20 == 0):
        print("")
print("%d};" % top)
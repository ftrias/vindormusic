#!/Users/ftrias/anaconda/bin/python

blocksperfile = 128
blocklines=1024 * 3
blockitems=16
item = 0
count = 0
size = 0
blockcount = -1
filecount = 0

prefix = "stereo"

with open('stereo.sf2', 'rb') as f:
  while True:
    dat = f.read(blockitems)
    if len(dat) == 0:
      outf.write('\n0};\n')
      break

    size += len(dat)

    if count==0:
      if blockcount==-1 or blockcount > blocksperfile:
        outf = open("%s%d.c" % (prefix, filecount), "w")
        blockcount = 0
        filecount += 1

      outf.write("const unsigned char stereo_sf2_data%d[] = {\n" % item)
      blockcount += 1

    line = ",".join(["0x%02x" % byte for byte in dat])

    count += 1

    if count >= blocklines:
      item += 1
      outf.write(line + "\n")
      outf.write("};\n")
      count = 0
    else:
      outf.write(line + ',\n')

outf = open("%s.c" % prefix, "w")

for i in range(0,item+1):
  outf.write("extern const unsigned char stereo_sf2_data%d[];\n" % i)

outf.write("const unsigned char *stereo_sf2[] = {\n")
for i in range(0,item+1):
  outf.write("stereo_sf2_data%d,\n" % i)
outf.write("0};\n")

outf.write("const int stereo_sf2_block = %d;\n" % (blocklines * blockitems))
outf.write("const int stereo_sf2_len = %d;\n" % size)

outf = open("%s.h" % prefix, "w")
outf.writelines([
'extern "C" const unsigned char **stereo_sf2;\n',
'extern "C" const int stereo_sf2_block;\n',
'extern "C" const int stereo_sf2_len;\n'
])

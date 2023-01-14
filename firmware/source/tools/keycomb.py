#!/usr/bin/python

nocodes=[['SD','SU']]

numcodes=['SD','SU','CD','CU','MD','MU'];
numcode={}
v=1
for x in numcodes:
  numcode[x]=v
  v=v+1

def moptions(choices, xisvalid):
  skey = 0
  ckey = 0
  akey = 0
  mkey = 0
  xuse = 0
  xmeta = 0
  px = ''
  for x in choices:
    if x=='SD': skey=1
    elif x=='SU': skey=0
    elif x=='CD': ckey=1
    elif x=='CU': ckey=0
    elif x=='AD': akey=1
    elif x=='AU': akey=0
    elif x=='MD': mkey=1
    elif x=='MU':
      if px=='MD' and (skey+ckey)==0: xmeta = 1
      mkey=0
    for noc in nocodes:
      if noc[0]==px and noc[1]==x: xmeta = 1
    if (skey+ckey)>0: xuse = 1
    px = x
  if xisvalid:
    if xmeta==1: return 0
    if skey==0 and ckey==0 and mkey==0:
      return 1
    return 0
  res = []
  if skey==0 and px!='SU': res.append('SD')
  if skey==1: res.append('SU')
  if ckey==0: res.append('CD')
  if ckey==1: res.append('CU')
  if akey==0: res.append('AD')
  if akey==1: res.append('AU')
  if mkey==0: res.append('MD')
  if mkey==1: 
    #if skey!=0 or ckey!=0: res.append('MU')
    if xuse==1: res.append('MU')
  #print "RS",res
  return res

def comb(choices, numkeys):
  if len(choices)>=numkeys:
    if moptions(choices, 1):
      results.append(choices)
      #print "DEBUG", choices
    return choices

  for i in moptions(choices, 0):
    comb(choices+[i], numkeys)

def recurse(numkeys):
  for i0 in ['SD', 'CD', 'AD', 'MD']:
    comb([i0], numkeys)

def forall():
  for i0 in ['SD', 'CD', 'MD']:
    for i1 in moptions([i0], 0):
      for i2 in moptions([i0, i1], 0):
        for i3 in moptions([i0, i1, i2], 0):
          for i4 in moptions([i0, i1, i2, i3], 0):
            for i5 in moptions([i0, i1, i2, i3, i4], 0):
              for i6 in moptions([i0, i1, i2, i3, i4, i5], 0):
                for i7 in moptions([i0, i1, i2, i3, i4, i5, i6], 0):
                  if moptions([i0,i1,i2,i3,i4,i5,i6,i7], 1):
                    results.append([i0,i1,i2,i3,i4,i5,i6,i7])
                  #print "DEBUG",[i0,i1,i2,i3,i4,i5,i6,i7]

  #for x in results: print x
  print len(results)

results = []
recurse(6)
for x in results: 
  print x

v=0
for x in results: 
  print "'"+"".join(x)+"':",v,","
  v=v+1

v=0
for x in results: 
  print "{KX_"+", KX_".join(x)+"},   // "+("%s"%v)
  v=v+1

print len(results)


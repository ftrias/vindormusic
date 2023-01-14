#!/usr/bin/env python3

import sys
sys.path.append("../../external/SoundfontDecoder")

import decoder

SFDIR = "../../sounds/sf2"

params = [
  [SFDIR+"/32MbGMStereo.sf2", 60, [3], 0, "sound", "omega_Clarinet"],
  [SFDIR+"/32MbGMStereo.sf2", 65, [5], 0, "sound", "omega_Sax"],
  [SFDIR+"/068_Florestan_Woodwinds.sf2", 0, [2], 0, "sound", "omega_Flute"],
]

for p in params:
  decoder.decode_selected(*p)

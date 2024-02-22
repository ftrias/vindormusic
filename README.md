Vindor ES Source Code
=====================

This repository holds the source code, modules and PCB layouts of the Vindor ES1/2 electronic saxophone and wind instrument (clarinet, flute, trumpet, etc.). More information is available at [Vindor Music](http://vindormusic.com)

Directories
-----------

| Directory           | Contents
| ------------------- | ------------------------
| /apps               | Mobile and web apps
| /apps/juce-gui      | Main control app for Mac/Win/iOS/Android
| /apps/splayer       | Free generic MIDI player
| /firmware           | Source for firmware and related modules
| /firmware/source    | ES1/2 main source
| /firmware/external  | Non-Vindor open source libraries used
| /pcb                | KiCad source for PCB
| /shell              | Solidworks models for the shell
| /documents          | Documentation and promotional material
| /sounds             | Sound files for use on SD cards


License
-------

Unless otherwise noted, the software is Copyright (c) 2023 by Fernando Trias and Vindor Music, Inc. The software is licensed for use under the Creative Commons license available in `LICENSE.txt` and summarized on the [CC page](https://creativecommons.org/licenses/by-nc/3.0/).

This software uses external modules that are distributed according to their own licenses, which are all open source.

To summarize:
1. You may freely use this software for non-commercial purposes.
2. You must give appropriate credit to respective authors.

History
---------
This project began in 2016 to produce an affordable electronic wind instrument. At that time, the market was dominated by Yamaha and Akai. Both produced superb instruments that were prohibitively expensive for casual musicians. In addition, there were a number of small niche manufacturers targeting advanced players. Although Akai produced a low-cost USB instrument, it was large, heavy and had limited market penetration. In addition, we wanted to create an easy-to-use low-cost alternative for beginning players that would be both stand-alone and interface easily with Garageband and similar apps popular with younger musicians.

The Vindor began with a Kickstarter campaign and started being sold commercially in 2018 both online and in stores. It was covered by numerous blogs and journals, including Fast Company and the Boston Globe. In 2019, Roland release the Aerophone Mini AE-01, which although larger and slightly more expensive than the Vindor, targeted the same entry-level market. This, combined with the COVID shutdown and subsequent rise in component costs, made producing more Vindors impractical and all existing inventory was sold out by late 2020. 

By 2023, the chip market had not recovered and it still not possible to make more Vindors (the main CPU is still unavailable). Since Vindor Music closed its doors, we have decided to open source the firmware and apps, as well as the PCB, BOM and other components.

Open Source Announcement
------------------------

Vindor Music has open sourced the ES1 electronic wind instrument software and hardware. The Vindor ES1 was a low-cost entry-level electronic wind instrument that launched in 2018 and sold until 2020, a casualty of the COVID pandemic. The release includes all the source for the firmware, Android, iOS, Mac and Windows apps, as well as the BOM, schematics and PCB layout of the circuit boards. It includes everything you need to make your own ES1 and program it to do whatever you like. We hope this will inspire hobbyists to port the ES1 to a new CPU and continue the mission of providing an affordable and functional electronic wind instrument. The source can be found at https://github.com/ftrias/vindormusic.

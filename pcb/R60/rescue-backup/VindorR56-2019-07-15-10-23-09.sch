EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:NorBot
LIBS:nxp_armmcu
LIBS:switches
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MK20DX256VLH7 U?
U 1 1 5AF9D3F1
P 5500 4000
F 0 "U?" H 6100 2200 60  0000 C CNN
F 1 "MK20DX256VLH7" H 5150 6700 60  0000 C CNN
F 2 "" H 5500 4000 60  0000 C CNN
F 3 "" H 5500 4000 60  0000 C CNN
	1    5500 4000
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW?
U 1 1 5AF9D78D
P 7700 2350
F 0 "SW?" H 7750 2450 50  0000 L CNN
F 1 "SW_Push" H 7700 2290 50  0000 C CNN
F 2 "" H 7700 2550 50  0001 C CNN
F 3 "" H 7700 2550 50  0001 C CNN
	1    7700 2350
	1    0    0    -1  
$EndComp
Text GLabel 6700 5000 2    60   Input ~ 0
26
Wire Wire Line
	6450 5000 6700 5000
Text GLabel 7350 2350 0    60   Input ~ 0
26
Wire Wire Line
	7350 2350 7500 2350
$Comp
L GND #PWR?
U 1 1 5AF9D8A5
P 8100 2400
F 0 "#PWR?" H 8100 2150 50  0001 C CNN
F 1 "GND" H 8100 2250 50  0000 C CNN
F 2 "" H 8100 2400 50  0001 C CNN
F 3 "" H 8100 2400 50  0001 C CNN
	1    8100 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 2350 8100 2350
Wire Wire Line
	8100 2350 8100 2400
$EndSCHEMATC

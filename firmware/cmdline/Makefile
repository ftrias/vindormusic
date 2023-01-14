DX=../../gui

# CC=clang++
# LIB=-framework CoreMIDI -framework CoreAudio -framework CoreFoundation

LIB=-lstdc++ -lasound -pthread
CC=gcc
CFLAGS=-D__LINUX_ALSA__

all: midicmd

midicmd: midicmd.cpp $(DX)/RtMidi.cpp $(DX)/RtMidi.h
	$(CC) $(CFLAGS) -o midicmd midicmd.cpp $(DX)/RtMidi.cpp $(LIB) -I$(DX)

midicmd.exe: midicmd.cpp $(DX)/RtMidi.cpp $(DX)/RtMidi.h
	x86_64-w64-mingw32-g++-win32 $(CFLAGS) -o midicmd.exe midicmd.cpp $(DX)/RtMidi.cpp -I$(DX) -lwinmm -static

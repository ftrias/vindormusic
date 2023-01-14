
:q

# Download dependencies
#

DIR=.

WCONF=CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32

help:
	@echo Use mac, win or linux

mac: dir usb dfu

linux: dir install.apt

win:
	make "CONF=$(WCONF)" winx

winx: dir usb dfu

wincp:
	cp libusb/libusb/libusb.h libs
	cp libusb/Win32/Release/lib/libusb-1.0.lib libs

dfu: $(DIR)/dfu-util/src/dfu-suffix
usb: $(DIR)/libusb

dir: $(DIR)/fork
	/bin/sh - ./download.sh

$(DIR)/dfu-util/src/config.h: usb
	#cd "$@" && git apply ../dfu-util.patch
	cp $(DIR)/libusb/config.h $@

$(DIR)/dfu-util/src/dfu-suffix: $(DIR)/dfu-util/src/config.h
	cd "$(dir $@)/.." && sh autogen.sh && ./configure && make

$(DIR)/libusb: $(DIR)/libusb/configure.ac.orig

$(DIR)/libusb/configure.ac.orig:
	cd "$(dir $@)" && cp configure.ac configure.ac.orig && sed -e 's/2.69/2.68/' configure.ac.orig > configure.ac
	cd "$(dir $@)" && glibtoolize && ./autogen.sh && ./configure $(CONF) --disable-shared && make
	rm "$@/libusb/.libs/libusb-1.0.dll.a"

SRC=../dfu-util
OBJ = build

SRC_FILES=dfu dfu_file dfu_load dfu_util dfuse dfuse_mem quirks dfu-xtra

OBJS := $(foreach obj, $(SRC_FILES), $(OBJ)/$(obj).o)

OUT = linux
CFLAGS = -I. -I$(SRC) -I$(SRC)/src -I../libusb/libusb -DHAVE_CONFIG_H -DHAVE_CONFIG_H -DHAVE_ERR -g -O3
LIB2 = /usr/lib/x86_64-linux-gnu/libusb.a

# OUT = mac
# CFLAGS = -I. -I$(SRC)/src -I../libusb/libusb -DHAVE_CONFIG_H -mmacosx-version-min=10.11 -DHAVE_CONFIG_H -DHAVE_ERR -g -O3 -arch x86_64 -arch arm64
# LIB2 = ../libusb/libusb/.libs/libusb-1.0.a

libdfu.a: $(OBJ) $(OBJS)
	ar rcs libdfu.a $(OBJS) $(LIB2)
	# libtool -o libdfu.a $(OBJS) $(LIB2)

$(OBJ):
	mkdir $(OBJ)

$(OBJ)/%.o: $(SRC)/src/%.c
	cc $(CFLAGS) -c -o "$@" "$<"

clean:
	rm -rf libdfu.a $(OBJ)

install:
	cp libdfu.a ../libs/$(OUT)

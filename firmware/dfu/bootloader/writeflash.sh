#!/bin/bash
# see http://openocd.org/doc/html/Flash-Commands.html
openocd &
pid=$!
sleep 1
gdb <<EOF
target extended-remote localhost:3333
monitor kinetis mdm halt
set remotetimeout 10
monitor kinetis mdm mass_erase
monitor flash write_bank 0 boot-mk20dx256-64.bin
EOF
kill $pid

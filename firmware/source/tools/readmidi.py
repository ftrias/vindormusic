#!/usr/bin/env python3

import pygame.midi
import array

def print_devices():
    for n in range(pygame.midi.get_count()):
        print (n,pygame.midi.get_device_info(n))

def process_message(message):
    global of
    code = message.pop(0)
    if code == 1:
        filename = ''.join(map(chr, message))
        of = open(filename, "wb")
        print("open:", filename)
    elif code == 2:
        of.close()
        print("close")
    elif code == 3:
        result = []
        for b0, b1 in zip(*[iter(message)]*2):
            b = (b0 << 4) + b1
            result.append(b)
        bytes = array.array('B', result).tostring()
        of.write(bytes)
        print("w", end='', flush=True)
    else:
        print("invalid:", message)

def readInput(input_device):
    message = []
    while True:
        if input_device.poll():
            event = input_device.read(1)[0][0]
            #print("D:",event)
            for code in event:
                if code == 0xF7: # end of message!
                    process_message(message)
                elif code == 0xF0: # end of message!
                    message = []
                else:
                    message.append(code)

if __name__ == '__main__':
    pygame.midi.init()
    print_devices()
    my_input = pygame.midi.Input(0)
    readInput(my_input)

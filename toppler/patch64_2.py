#!/bin/env python3
from pwn import *

e = ELF('./toppler64')

# make the function pts_lifesleft returns always true
e.write(0x0041401d, b'\x40\xb0\x01') # MOV EAX,0x1

e.save('toppler64_patch2')

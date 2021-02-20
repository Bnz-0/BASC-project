#!/bin/env python3
from pwn import *

e = ELF('./toppler32')

# make the function pts_lifesleft returns always true
e.write(0x08056426, b'\xc6\xc0\x01') # MOV AL,0x1

e.save('toppler32_patch2')

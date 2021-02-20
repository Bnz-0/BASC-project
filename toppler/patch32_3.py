#!/bin/env python3
from pwn import *

e = ELF('./toppler32')

# get extra life for each point instead every 5000
e.write(0x080563b2 + 6, p32(0)) # MOV dword prt [nextlife],0
e.write(0x080563f6 + 1, p32(1)) # ADD EAX,0x1

# increments the number of lifes you can get
e.write(0x080563ca + 2, p8(127)) # CMP EAX,0x7f
e.write(0x080563cf + 6, p32(127)) # MOV dword prt [lifes],0x7f

e.save('toppler32_patch3')

#!/bin/env python3
from pwn import *

e = ELF('./toppler64')

# get extra life for each point instead every 5000
e.write(0x00413f5c + 7, p32(0)) # MOV qword ptr [nextlife],0x0
e.write(0x00413fa1 + 2, p32(1)) # ADD RAX,0x1

# increments the number of lifes you can get
e.write(0x00413fd1 + 2, p8(127)) # CMP EAX,0x7f
e.write(0x00413fd6 + 6, p32(127)) # MOV dword ptr [lives],0x7f

e.save('toppler64_patch3')

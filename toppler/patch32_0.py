#!/bin/env python3
from pwn import *

e = ELF('./toppler32')

# writes a 6 bytes NOP to avoid a JMP and continue the execution
# this cause the starting of the "submarine section" of the game when die
e.write(0x0804d2dc, b'\x66\x0f\x1f\x44\x00\x00') # 6 bytes nop

e.save('toppler32_patch0')

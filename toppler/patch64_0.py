#!/bin/env python3
from pwn import *

e = ELF('./toppler64')

# nop the `object[iVar4].kind = OBJ_KIND_APPEAR` and the tower's robots will never appear
e.write(0x004152af, b'\x66\x0F\x1F\x44\x00\x00') # 6 bytes nop

e.save('toppler64_patch0')

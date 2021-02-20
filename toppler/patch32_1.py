#!/bin/env python3
from pwn import *

e = ELF('./toppler32')

# sub 0 to the time, so infinite time
e.write(0x0804c4f1 + 2, b'\x00') # SUB time,0

e.save('toppler32_patch1')

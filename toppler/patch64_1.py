#!/bin/env python3
from pwn import *

e = ELF('./toppler64')

# sub 0 to the time, so infinite time
e.write(0x004063fe + 2, p8(0)) # lea edx,[rax + 0x0]

e.save('toppler64_patch1')

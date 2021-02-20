#!/bin/env python3
from pwn import *

p = process('./matteo_benzi')

# phase 1
p.recvuntil('!')
p.sendline('Public speaking is very easy.')

# phase 2
p.recvuntil('?')
p.sendline('1 2 4 8 16 32')

# phase 3
p.recvuntil('!')
p.sendline('0 o 212')

# phase 4 + activate secret phase
p.recvuntil('!')
p.sendline('10 37 b4s1LiC0')

# phase 5
p.recvuntil('Try this one.')
p.sendline('bel1n.')

# phase 6
p.recvuntil('...')
p.sendline('4 6 5 3 1 2')

# secret phase
p.recvuntil('...')
p.sendline('40')

print(p.recvuntil("You've defused the bomb!").decode('utf-8'))

#p.interactive()
p.close()

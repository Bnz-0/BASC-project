#!/bin/env python3
from pwn import *

p = process('chroot ./fakeroot /guess_the_number_5c1549905c60d35ca812b2e666b7644b bnz', shell=True)

for x in [39,10,42,10,26,2,26,35,41,27]:
	print(p.recvuntil('Your guess?').decode('utf-8'))
	p.sendline(str(x))
	print(x)

print(p.recvuntil('You have won, congratulations!').decode('utf-8'))
p.close()

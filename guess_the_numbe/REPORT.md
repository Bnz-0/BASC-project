> **Used tools:**
> - gdb
> - pwntools

## Fake win

Looking at the source code I notice that to choose the number to guess it uses the `rand()` function, which his seed is selected reading 4 bytes from `/dev/urandom` file.

So a way to _easy win_ is to known which seed is using the binary, so that we can generate the sequence of pseudo-random number to "guess" what number it is thinking and beat it.

To do that I decide to run it in a fake root with a constant inside `/dev/urandom` (in this case 0) to generate always the same sequence (which, with seed 0, is: 39, 10, 42, 10, 26, 2, 26, 35, 41, 27)

---

To try it the steps are:
- `$ make`: generates a minimal fake root
- `$ sudo ./fake_win.py`: runs the binary inside the fake root using `chroot` and "guess" all the 10 number

## Spawn the shell

To spawn a shell I found a buffer overflow in the function `game_loop`: a `gets` is used to write the input in a 128 bytes long buffer.

Since that the stack isn't executable I decided to try to find a ROP chain using ropper. Luckily it finds one, so with the help of gdb and cyclic I found where is the return address of the function (16 bytes after the end of the buffer).

So, to sum up the script play a game losing 10 times, then send 144 'n' (only the first one have a meaning) and inject the rop-chain addresses.

## Other vulnerabilities/cheat

- **my_hash**: seems that this function can be inverted, so to find the inverse of 770283071 and add it to the environment to get always 42.
- **print_name**: also in this function is present a buffer overflow. It have his own canary, but the value is calculated using the current POSIX time so is quite easy to bypass. \
	The "real problem" here is that is passing troughs the process arguments, that didn't accept some bytes like 0, which seems to be necessary for ropper to find an execve rop-chain. That's why I decided to use the other buffer overflow to spawn a shell.
- **play_the_game**: this function, when it reads somethings that is not a number, printf directly the buffer, allowing the user to use the printf string formatting powers.

---

### and special thanks goes to...
**Andrea Straforini**, who saved me to wasting more time than I already lost.

The fact is that I believed that the executable has a stack canary protection. Both gdb and pwntools tell me that the binary has a canary, so before doing the real stuff I tried to read the value of the canary... And I did it!
But after that I being stuck at using it. Luckily talking with Andrea the "wait... canary?!" phrase saved me.

Only after we realize that the canary comes from some library because the binary is statically linked.

Btw I decided to push also the `get_useless_canary.py` script because yes it's useless but at least it works.


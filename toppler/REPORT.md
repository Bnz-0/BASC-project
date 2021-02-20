To do this assignment I mainly used ghidra and sometimes also gdb.

Most of discoveries I made are done exploring the function `main_game_loop`. To find it I used gdb with some breakpoints taken from ghidra trying to have some "known point" to starting the static analysis not only form the main.

---

# toppler32

## patch 0 (die and win)
Inside the function `gam_towergame` I found a piece of code where the state is set to `STATE_DIE`:

```c
lifes = top_died();
if (lifes != false) {
	state = STATE_DIED;
}
```

My intent was to avoid that the variable `state` will never being set to `STATE_DIED` but I feared of what would happen if I jump in the water.
So I decided to find another way and the first stupid thing that came to my mind was to nop the jump instruction corresponding to the if statement:

```x86asm
0804d2d5 e8 a8 f9        CALL       top_died
         00 00
0804d2da 84 c0           TEST       AL,AL
0804d2dc 0f 85 fb        JNZ        LAB_0804d5dd ; jump corresponding to the if
         02 00 00
0804d2e2 e8 71 f9        CALL       top_anglepos
         00 00
```

So I replaced it with a 6 byte nop instruction and, sure to have broke everything, I tried to run the game and die.
The funny (and very lucky) thing is that apparently reaching the following code with a `state != STATE_DIED` means that you have passed the level and the submarine appear to bring you to the next tower!

So now basically each time I die I didn't lose a life (probably because it would have been decremented only if `state == STATE_DIED`) and pass the level.

## patch 1 (infinite time)
I found that inside the function `akt_time` a variable, probably used for the timing, is subtracted by 1.

```x86asm
0804c4ef 8b 03           MOV        time,dword ptr [EBX]
0804c4f1 83 e8 01        SUB        time,0x1
0804c4f4 89 03           MOV        dword ptr [EBX],time
```

Modifying the instruction to make it subtracting by 0 cause the freeze of the timer.

## patch 2 (infinite life)
After some research i found a function (called `pts_lifesleft`) which I guess is used to check if you have still some life:

```x86asm
; basically do `return lifes != 0;`, where lifes is a global variable

0805641f 83 3d 9c        CMP        dword ptr [lifes],0x0
         91 06 08 00
08056426 0f 95 c0        SETNZ      AL
08056429 c3              RET

```

So I changed the setnz instruction to `MOV AL,0x1` to make this function returns always true.
What I achieved is to have infinite lifes, because even if I reach 0 life this function returns true anyway, so the game thinks that I have still some lifes.

## patch 3 (get a life for each point)

Casually I discovered that every X points you get an extra life, so I decided to change this behavior and to get a life every time I get a point.

This is the disassembled function where extra life are managed. A particular thing is that the variable `nextlife` is incremented by 5000 until it reach the actual point, which means that an eventual 10000+ points gain cause multiple extra lifes.

```c
void pts_add(int add) {
  points = points + add;
  while (nextlife <= points) {
    nextlife = nextlife + 5000;
    lives_add();
  }
  return;
}
```

So I saw that if I modify the instruction to add 1 instead 5000 to `nextlife` I'll gain a lot of lifes every time I gain some point, so I basically changed the constant value of the ADD from 0x1388 to 0x1

```x86asm
080563f6 05 88 13        ADD EAX,0x1388 ; increments "nextlife" by 5000
         00 00
```

Another thing to do is to set the initial value of `nextlife`, because when it starts it's 5000.
So searching by its references I found the function `pts_reset`, where the variable is set to 5000, so like before I changed the constant value of the MOV to 0x1388 to 0

```x86asm
080563b2 c7 05 a0        MOV dword ptr [nextlife],0x1388 ; set the variable "nextlife" to 5000
         91 06 08 
         88 13 00 00
```

Last thing, optional to be fair, is that the game gives you no more than 8 lifes, even if you make a lot of points.
So inside the function `lives_add` I changed 8, in the CPM and MOV instructions, with 127, which increase the maximum amount of lifes to 127.

```x86asm
080563ca 83 f8 08        CMP        EAX,0x8
080563cd 7e 0a           JLE        LAB_080563d9
080563cf c7 05 9c        MOV        dword ptr [lifes],0x8
         91 06 08 
         08 00 00 00
```

> 127 and not with a higher number because of the signed jump.
> I tried also to change the jump to an unsigned one to get 254 max lifes, but for some unknown reason it didn't work...

---

# toppler 64

## patch 0 (no tower robots)

Watching for the helpful hints and using the 32 bit binary, I discover a function correspond to one called "`rob_new`", not a name very clear to be fair but reading the variables names I see that "rob" means "robots" and immediately it caught my attention because probably this is the function that creates new robots!

Reading the 64 bit binary unfortunately I didn't discovery much, just that in the end this function set some variables of an obscure array/struct, but watching at the 32 bit binary disassemble I see that it's setting an array of struct with some constant and one of these is called `OBJ_KIND_APPEAR`.

Hoping that my guess from the constant name is correct I nop the instruction where `0x6` (which is the value of `OBJ_KIND_APPEAR`) is wrote and each robot **inside** the tower disappear, or better: they not appear.

```x86asm
004152af c7 00 06        MOV dword ptr [RAX],offset DAT_0042b910 ; 6 == OBJ_KIND_APPEAR
         00 00 00
```

## patch 1 (infinite time)
Helped by the 32 bit binary I easily found the function `akt_time`, where time is decremented.
I substitute this with `0` instead `-1` (`0xff`) and the time stops running.

```x86asm
004063fe 8d 50 ff        LEA EDX,[RAX + -0x1]
```

## patch 2 (infinite life)
Again helped by the 32 bit binary I found the location of the `pts_lifesleft` function.
Using the same tecnique I substitute the `SETNZ AL` instruction with `MOV EAX,0x1` to make this function return always true.

```x86asm
0041401b 85 c0           TEST EAX,EAX
0041401d 0f 95 c0        SETNZ AL
00414020 5d              POP RBP
00414021 c3              RET
```

## patch 3 (get a life for each point)
This time I searched directly by scalar value to find the place where `0x1388` is used.
After I found it, rebuilding the already known meaning and flow of the program was really easy.
So, like with the 32 bit version, I replace `0x1388` in the function `pts_reset` with `0`

```x86asm
00413f5c 48 c7 05        MOV qword ptr [nextlife],0x1388
         81 79 01
         00 88 13
         00 00
```

Then replace again `0x1388` with `1` in the `pts_add` function

```
00413fa1 48 05 88        ADD RAX,0x1388
         13 00 00
```

And finally substitute `0x8` to `0x7f` inside `lives_add` to increase the maximum amount of lifes.

```
00413fd1 83 f8 08        CMP EAX,0x8
00413fd4 7e 0a           JLE LAB_00413fe0
00413fd6 c7 05 10        MOV dword ptr [lives],0x8
         79 01 00 
         08 00 00 00
```

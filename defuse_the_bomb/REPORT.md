To solve these phases basically I used only **ghidra** for the static analysis and **gdb** for the dynamic one. I never felt so confused as to use some "dynamic hacks" or change the binary to try to understand what is happening, even if with the phase 5 I got really close to do it.

The only preliminary things that I've done is find the main, but I simply search where `__libc_start_main` is called and read the first argument.

## Phase 1
The disassemble of the first phase is quite simple: calls a function that take 2 strings and checks the result.
Because it's the first phase I just tried to pass the same string: **"Public speaking is very easy."**, luckily my guess was correct and I passed the first stage.

So I marked this function as `custom_strcmp` and I looked at it to see if does something else. The "important" thing that I've discovered is the meaning of another unknown function which simply is a `custom_strlen`.

## Phase 2
The ghidra's disassemble of the second phase is super clear:
- extract 6 integers from the input into an array
- check if the first number is 1
- for each integer checks if `values[i] == values[i+1]*2`

so the correct input is **"1 2 4 8 16 32"**

## Phase 3
The third phase expects in this order, an integer, a char and another integer.
Then with a big switch, based on the first input, checks the other 2 values.

So I just choose one combination, in my case: **"0 o 212"**

## Phase 4
The fourth phase takes 2 integer, do some checks, call a recursive function that calculate something using the first input and then checks if the second input and the calculated value are equals to `0x25`.

So the second input is fixed to 37, but I should have understand what the recursive function do to choose a correct first input.
Looking better at the first part of the checks I discovered that the first input must be `0 < input1 < 0xe`, so I decided to try all the possible values and in my case 10 is the correct one.

Then the input that defuse the stage 4 is **"10 37"**

## Phase 5
The fifth phase was the hardest for me, mainly because I did not understand that here is an array involved.

This stage expects a string long 6 char (immediately discovered because I've already marked the `custom_strlen` function).
Then for each character does a simple calculations and the sum of all of that must be `0x49`.

Now, probably that night I was on acid or something, because I didn't see that the "strange pointer" involved in the calculations is actually a simple array and the calculations is just to access into a value inside the array.
I spent a lot of time with gdb trying to understand what is happening and finally I discovered that it's a 16 length array and the index is calculated using the lower 4 bit f the i-th char.

To easily trying it I reproduce the behaviour in python:
```python
>>> sum(([0x2,0xa,0x6,0x1,0xc,0x10,0x9,0x3,0x4,0x7,0xe,0x5,0xb,0x8,0xf,0xd][ord(c) & 0xf] for c in "input!"))
62
```

So I play a bit to find a correct input to use and... **"bel1n."** is the way!

> To be fair, a things that I still didn't understand is that in the calculus to get the index there is a `*4` but this broke everything. I think is a consequence of  the conversion from doubleword to quadword (EAX -> RAX) made by the `CDQE` instruction, but i'm not sure.

## Phase 6
The last phase takes again 6 integers and apparently do a lot of stuff. Looking better this phase is separated in 2 part: the first checks if the input is applicable to the second one, which do the real calculates involving also another array/matrix to check if the input is also correct.

I didn't explore the second part so I don't know what it does because the first part specifies some constraint for the input:
- `0 < value[i] < 7`
- `value[i] != value[j], i != j`

so a valid input is for example "1 2 3 4 5 6".

So I decided to brute force all the 720 possible valid input instead understanding the second part of this phase. The fist correct input I found was **"4 6 5 3 1 2"**.

## Secret phase

To unlock the secret phase, in a string located in a certain address there must be 2 integer and the sequence of characters "b4s1LiC0".

A brief static analysis didn't show where this string is written, so I decided to look with gdb for changes in the various phase, hoping in a hint.
I saw that this string is written with the same input of the 4th phase, then I changed the input for that phase with **"10 37 b4s1LiC0"** which still pass the phase.

Then I'm starting at understand what the secret phase do: \
It takes only a single integer from the input, then checks if it's between 1 and 1001 and... my laziness wins again. So, like the 6th phase, I brute forced all the possible values.

**40** was the first correct value that I found.

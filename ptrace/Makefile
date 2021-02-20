All: instruction_counter dynamic_disasm

instruction_counter: instruction_counter.c
	gcc instruction_counter.c -m64 -Wall -o instruction_counter

dynamic_disasm: dynamic_disasm.c
	gcc dynamic_disasm.c -m64 -Wall -lcapstone -o dynamic_disasm

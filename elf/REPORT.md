### elf1 `BASC{s3cTi0N5}`

Simply looking at the elf's segment section appears some strange sections, which if put together form the flag

```bash
$ readelf --segment elf1
...
   02     .interp .note.gnu.property .note.gnu.build-id .note.ABI-tag .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt
   03     .init .plt .plt.got .plt.sec .text B A S C { s 3 c T i 0 N 5 } .fini
   04     .rodata .eh_frame_hdr .eh_frame
...
```

### elf2 `BASC{ARMed_&_d4ng3r0uS}`

The elf2 binary is for an arm system, using qemu to run it reveals the flag

```bash
$ qemu-arm elf2
BASC{ARMed_&_d4ng3r0uS}
```

### elf3 `BASC{cAs3_maTT3rS}`

Looking at the ELF header I noticed that the magic number is not of an ELF file:

```bash
$ readelf -h elf3
readelf: Error: Not an ELF file - it has the wrong magic bytes at the start
$ xxd elf3 | head -n 1
00000000: 7f65 6c66 0201 0100 0000 0000 0000 0000  .elf............
```

So i replaced it with the correct one (`7f45 4c46`) and executing it to get the flag

### elf4 `BASC{no_eXec_no_party}`

Trying to run the elf4 generates a _Segmentation fault (core dumped)_ and everything in the ELF header seems ok.

Investigating a bit more on what happen I discovered that it tries to do something with the address `0x401c60` which generates the seg fault, but more strange and interesting is that it happen at the very beginning of the program:

```bash
strace ./elf4
execve("./elf4", ["./elf4"], 0x7ffd0494b640 /* 57 vars */) = 0
--- SIGSEGV {si_signo=SIGSEGV, si_code=SEGV_ACCERR, si_addr=0x401c60} ---
+++ killed by SIGSEGV (core dumped) +++
Segmentation fault (core dumped)
```

So I looked at the entry point of the executable and I discovered that is exactly at `0x401c60`, then I watched the program headers to see if it is a valid address and I saw that the entry point points to a non executable header:

```bash
$ readelf --segment elf4
...
Program Headers:
  Type           Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align
  LOAD           0x000000 0x0000000000400000 0x0000000000400000 0x000518 0x000518 R   0x1000
  LOAD           0x001000 0x0000000000401000 0x0000000000401000 0x09366d 0x09366d R   0x1000
  LOAD           0x095000 0x0000000000495000 0x0000000000495000 0x02662a 0x02662a R   0x1000
...
```

So I modified his flags to make it executable and launching the binary the flag appears

### ELF_files.zip `BASC{can_U_run_a_ZIP?}`

Ok I admit, you screwed me.


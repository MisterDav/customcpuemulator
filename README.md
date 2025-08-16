# customcpuemulator
This is a CPU emulator for a different 8-bit CPU I made, which was made to be better than the original. This one is still notably slow when compiled, especially with memory operations.

This CPU includes an 8-bit data bus, 16-bit address bus.
16 bit program counter
16 bit temporary register

2 8-bit general purpose registers (A and B)
8 bit status register

8 total ALU operations (ADD, SUB, OR, AND, XOR, LSH, RSH, NOT)
Can have 64 total instructions, constructed of 16 micro-instructions which run in sequence. 11 of which are unused.

Right now the emulator has the ability to dump the ram contents to a file and load rom, as well as displaying the data stored within VRAM from 0x4000 to 0x8000.
This gives the emulator a display size of 512x512.

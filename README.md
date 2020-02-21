# CPU6502
A MOS 6502 emulator with a simple API for easy integration into 6502 machine emulators

## Compiling and running
Requires cmake

To compile:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

To run:
```
./cpu6502 <binary file>
```

## Resources and references
- Instruction reference: https://www.masswerk.at/6502/6502_instruction_set.html
- And: http://www.6502.org/tutorials/6502opcodes.html
- Lots of structure and implementation references: https://github.com/gianlucag/mos6502
- And https://github.com/DavidBuchanan314/6502-emu
- Addressing modes descriptions: http://emulator101.com
- Various information from: https://en.wikipedia.org/wiki/MOS_Technology_6502

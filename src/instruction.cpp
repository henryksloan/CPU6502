#include "instruction.h"

void Instructions::add_instr(instr_map_t &map, std::string op_str, std::vector<InstrMode> modes) {
    for (auto mode : modes) {
        map[mode.opcode] = { op_str, mode.mode_str, mode.cycles };
    }
}

Instructions::instr_map_t Instructions::generate_instr_map() {
    instr_map_t map;

    // TODO: Maybe move to data file
    add_instr(map, "ADC", {{0x69, "IMM", 2}, {0x65, "ZER", 3}, {0x75, "ZEX", 4}, {0x6D, "ABS", 4},
                           {0x7D, "ABX", 4}, {0x79, "ABY", 4}, {0x61, "INX", 6}, {0x71, "INY", 5}});
    add_instr(map, "AND", {{0x29, "IMM", 2}, {0x25, "ZER", 3}, {0x35, "ZEX", 4}, {0x2D, "ABS", 4},
                           {0x3D, "ABX", 4}, {0x39, "ABY", 4}, {0x21, "INX", 6}, {0x31, "INY", 5}});
    add_instr(map, "ASL", {{0x0A, "ACC", 2}, {0x06, "ZER", 5}, {0x16, "ZEX", 6},
                           {0x0E, "ABS", 6}, {0x1E, "ABX", 7}});
    add_instr(map, "BCC", {{0x90, "REL", 2}});
    add_instr(map, "BCS", {{0xB0, "REL", 2}});
    add_instr(map, "BEQ", {{0xF0, "REL", 2}});
    add_instr(map, "BIT", {{0x24, "ZER", 3}, {0x2C, "ABS", 4}});
    add_instr(map, "BMI", {{0x30, "REL", 2}});
    add_instr(map, "BNE", {{0xD0, "REL", 2}});
    add_instr(map, "BPL", {{0x10, "REL", 2}});
    add_instr(map, "BRK", {{0x00, "IMP", 7}});
    add_instr(map, "BVC", {{0x50, "REL", 2}});
    add_instr(map, "BVS", {{0x70, "REL", 2}});
    add_instr(map, "CLC", {{0x18, "IMP", 2}});
    add_instr(map, "CLD", {{0xD8, "IMP", 2}});
    add_instr(map, "CLI", {{0x58, "IMP", 2}});
    add_instr(map, "CLV", {{0xB8, "IMP", 2}});
    add_instr(map, "CMP", {{0xC9, "IMM", 2}, {0xC5, "ZER", 3}, {0xD5, "ZEX", 4}, {0xCD, "ABS", 4},
                           {0xDD, "ABX", 4}, {0xD9, "ABY", 4}, {0xC1, "INX", 6}, {0xD1, "INY", 5}});
    add_instr(map, "CPX", {{0xE0, "IMM", 2}, {0xE4, "ZER", 3}, {0xEC, "ABS", 4}});
    add_instr(map, "CPY", {{0xC0, "IMM", 2}, {0xC4, "ZER", 3}, {0xCC, "ABS", 4}});
    add_instr(map, "DEC", {{0xC6, "ZER", 5}, {0xD6, "ZEX", 6}, {0xCE, "ABS", 6}, {0xDE, "ABX", 7}});
    add_instr(map, "DEX", {{0xCA, "IMP", 2}});
    add_instr(map, "DEY", {{0x88, "IMP", 2}});
    add_instr(map, "EOR", {{0x49, "IMM", 2}, {0x45, "ZER", 3}, {0x55, "ZEX", 4}, {0x4D, "ABS", 4},
                           {0x5D, "ABX", 4}, {0x59, "ABY", 4}, {0x41, "INX", 6}, {0x51, "INY", 5}});
    add_instr(map, "INC", {{0xE6, "ZER", 5}, {0xF6, "ZEX", 6}, {0xEE, "ABS", 6}, {0xFE, "ABX", 7}});
    add_instr(map, "INX", {{0xE8, "IMP", 2}});
    add_instr(map, "INY", {{0xC8, "IMP", 2}});
    add_instr(map, "JMP", {{0x4C, "ABS", 3}, {0x6C, "ABI", 5}});
    add_instr(map, "JSR", {{0x20, "ABS", 6}});
    add_instr(map, "LDA", {{0xA9, "IMM", 2}, {0xA5, "ZER", 3}, {0xB5, "ZEX", 4}, {0xAD, "ABS", 4},
                           {0xBD, "ABX", 4}, {0xB9, "ABY", 4}, {0xA1, "INX", 6}, {0xB1, "INY", 5}});
    add_instr(map, "LDX", {{0xA2, "IMM", 2}, {0xA6, "ZER", 3}, {0xB6, "ZEX", 4},
                           {0xAE, "ABS", 4}, {0xBE, "ABX", 4}});
    add_instr(map, "LDY", {{0xA2, "IMM", 2}, {0xA4, "ZER", 3}, {0xB4, "ZEX", 4},
                           {0xAC, "ABS", 4}, {0xBC, "ABX", 4}});
    add_instr(map, "LSR", {{0x4A, "ACC", 2}, {0x46, "ZER", 5}, {0x56, "ZEX", 6},
                           {0x4E, "ABS", 6}, {0x5E, "ABX", 7}});
    add_instr(map, "NOP", {{0xEA, "IMP", 2}});
    add_instr(map, "ORA", {{0x09, "IMM", 2}, {0x05, "ZER", 3}, {0x15, "ZEX", 4}, {0x0D, "ABS", 4},
                           {0x1D, "ABX", 4}, {0x19, "ABY", 4}, {0x01, "INX", 6}, {0x11, "INY", 5}});
    add_instr(map, "PHA", {{0x48, "IMP", 2}});
    add_instr(map, "PHP", {{0x08, "IMP", 2}});
    add_instr(map, "PLA", {{0x68, "IMP", 2}});
    add_instr(map, "PLP", {{0x28, "IMP", 2}});
    add_instr(map, "ROL", {{0x2A, "ACC", 2}, {0x26, "ZER", 5}, {0x36, "ZEX", 6},
                           {0x2E, "ABS", 6}, {0x3E, "ABX", 7}});
    add_instr(map, "ROR", {{0x6A, "ACC", 2}, {0x66, "ZER", 5}, {0x76, "ZEX", 6},
                           {0x6E, "ABS", 6}, {0x7E, "ABX", 7}});
    add_instr(map, "RTI", {{0x40, "IMP", 2}});
    add_instr(map, "RTS", {{0x60, "IMP", 2}});
    add_instr(map, "SBC", {{0xE9, "IMM", 2}, {0xE5, "ZER", 3}, {0xF5, "ZEX", 4}, {0xED, "ABS", 4},
                           {0xFD, "ABX", 4}, {0xF9, "ABY", 4}, {0xE1, "INX", 6}, {0xF1, "INY", 5}});
    add_instr(map, "SEC", {{0x38, "IMP", 2}});
    add_instr(map, "SED", {{0xF8, "IMP", 2}});
    add_instr(map, "SEI", {{0x78, "IMP", 2}});
    add_instr(map, "STA", {{0x85, "ZER", 3}, {0x95, "ZEX", 4}, {0x8D, "ABS", 4}, {0x9D, "ABX", 5},
                           {0x99, "ABY", 5}, {0x81, "INX", 6}, {0x91, "INY", 6}});
    add_instr(map, "STX", {{0x86, "ZER", 3}, {0x96, "ZEX", 4}, {0x8E, "ABS", 4}});
    add_instr(map, "STY", {{0x84, "ZER", 3}, {0x94, "ZEX", 4}, {0x8C, "ABS", 4}});
    add_instr(map, "TAX", {{0xAA, "IMP", 2}});
    add_instr(map, "TAY", {{0xA8, "IMP", 2}});
    add_instr(map, "TSX", {{0xBA, "IMP", 2}});
    add_instr(map, "TXA", {{0x8A, "IMP", 2}});
    add_instr(map, "TXS", {{0x9A, "IMP", 2}});
    add_instr(map, "TYA", {{0x98, "IMP", 2}});

    return map;
}

const std::unordered_map<uint8_t, InstrInfo> Instructions::instr_map = generate_instr_map();

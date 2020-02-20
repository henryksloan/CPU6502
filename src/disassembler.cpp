#include "disassembler.h"

const std::map<std::string, Disassembler::AddrMode> Disassembler::addr_modes = {
    {"ACC", {0, "A"}},
    {"IMM", {1, "#$b"}},
    {"ABS", {2, "$w"}},
    {"ZER", {1, "$b"}},
    {"ZEX", {1, "$b,X"}},
    {"ZEY", {1, "$b,Y"}},
    {"ABX", {2, "$w,X"}},
    {"ABY", {2, "$w,Y"}},
    {"IMP", {0, ""}},
    {"REL", {1, "$w"}},
    {"INX", {1, "($b,X)"}},
    {"INY", {1, "($b),Y"}},
    {"ABI", {2, "($w)"}}
};

Disassembler::Disassembler(uint16_t base)
    : base(base) {
    // TODO: Fill invalids
    register_instr(0x69, "ADC", "IMM");
    register_instr(0x65, "ADC", "ZER");
    register_instr(0x75, "ADC", "ZEX");
    register_instr(0x6D, "ADC", "ABS");
    register_instr(0x7D, "ADC", "ABX");
    register_instr(0x79, "ADC", "ABY");
    register_instr(0x61, "ADC", "INX");
    register_instr(0x71, "ADC", "INY");
    register_instr(0x29, "AND", "IMM");
    register_instr(0x25, "AND", "ZER");
    register_instr(0x35, "AND", "ZEX");
    register_instr(0x2D, "AND", "ABS");
    register_instr(0x3D, "AND", "ABX");
    register_instr(0x39, "AND", "ABY");
    register_instr(0x21, "AND", "INX");
    register_instr(0x31, "AND", "INY");
    register_instr(0x0A, "ASL", "ACC");
    register_instr(0x06, "ASL", "ZER");
    register_instr(0x16, "ASL", "ZEX");
    register_instr(0x0E, "ASL", "ABS");
    register_instr(0x1E, "ASL", "ABX");
    register_instr(0x90, "BCC", "REL");
    register_instr(0xB0, "BCS", "REL");
    register_instr(0xF0, "BEQ", "REL");
    register_instr(0x24, "BIT", "ZER");
    register_instr(0x2C, "BIT", "ABS");
    register_instr(0x30, "BMI", "REL");
    register_instr(0xD0, "BNE", "REL");
    register_instr(0x10, "BPL", "REL");
    register_instr(0x00, "BRK", "IMP");
    register_instr(0x50, "BVC", "REL");
    register_instr(0x70, "BVS", "REL");
    register_instr(0x18, "CLC", "IMP");
    register_instr(0xD8, "CLD", "IMP");
    register_instr(0x58, "CLI", "IMP");
    register_instr(0xB8, "CLV", "IMP");
    register_instr(0xC9, "CMP", "IMM");
    register_instr(0xC5, "CMP", "ZER");
    register_instr(0xD5, "CMP", "ZEX");
    register_instr(0xCD, "CMP", "ABS");
    register_instr(0xDD, "CMP", "ABX");
    register_instr(0xD9, "CMP", "ABY");
    register_instr(0xC1, "CMP", "INX");
    register_instr(0xD1, "CMP", "INY");
    register_instr(0xE0, "CPX", "IMM");
    register_instr(0xE4, "CPX", "ZER");
    register_instr(0xEC, "CPX", "ABS");
    register_instr(0xC0, "CPY", "IMM");
    register_instr(0xC4, "CPY", "ZER");
    register_instr(0xCC, "CPY", "ABS");
    register_instr(0xC6, "DEC", "ZER");
    register_instr(0xD6, "DEC", "ZEX");
    register_instr(0xCE, "DEC", "ABS");
    register_instr(0xDE, "DEC", "ABX");
    register_instr(0xCA, "DEX", "IMP");
    register_instr(0x88, "DEY", "IMP");
    register_instr(0x49, "EOR", "IMM");
    register_instr(0x45, "EOR", "ZER");
    register_instr(0x55, "EOR", "ZEX");
    register_instr(0x4D, "EOR", "ABS");
    register_instr(0x5D, "EOR", "ABX");
    register_instr(0x59, "EOR", "ABY");
    register_instr(0x41, "EOR", "INX");
    register_instr(0x51, "EOR", "INY");
    register_instr(0xE6, "INC", "ZER");
    register_instr(0xF6, "INC", "ZEX");
    register_instr(0xEE, "INC", "ABS");
    register_instr(0xFE, "INC", "ABX");
    register_instr(0xE8, "INX", "IMP");
    register_instr(0xC8, "INY", "IMP");
    register_instr(0x4C, "JMP", "ABS");
    register_instr(0x6C, "JMP", "ABI");
    register_instr(0x20, "JSR", "ABS");
    register_instr(0xA9, "LDA", "IMM");
    register_instr(0xA5, "LDA", "ZER");
    register_instr(0xB5, "LDA", "ZEX");
    register_instr(0xAD, "LDA", "ABS");
    register_instr(0xBD, "LDA", "ABX");
    register_instr(0xB9, "LDA", "ABY");
    register_instr(0xA1, "LDA", "INX");
    register_instr(0xB1, "LDA", "INY");
    register_instr(0xA2, "LDX", "IMM");
    register_instr(0xA6, "LDX", "ZER");
    register_instr(0xB6, "LDX", "ZEX");
    register_instr(0xAE, "LDX", "ABS");
    register_instr(0xBE, "LDX", "ABX");
    register_instr(0xA0, "LDY", "IMM");
    register_instr(0xA4, "LDY", "ZER");
    register_instr(0xB4, "LDY", "ZEX");
    register_instr(0xAC, "LDY", "ABS");
    register_instr(0xBC, "LDY", "ABX");
    register_instr(0x4A, "LSR", "ACC");
    register_instr(0x46, "LSR", "ZER");
    register_instr(0x56, "LSR", "ZEX");
    register_instr(0x4E, "LSR", "ABS");
    register_instr(0x5E, "LSR", "ABX");
    register_instr(0xEA, "NOP", "IMP");
    register_instr(0x09, "ORA", "IMM");
    register_instr(0x05, "ORA", "ZER");
    register_instr(0x15, "ORA", "ZEX");
    register_instr(0x0D, "ORA", "ABS");
    register_instr(0x1D, "ORA", "ABX");
    register_instr(0x19, "ORA", "ABY");
    register_instr(0x01, "ORA", "INX");
    register_instr(0x11, "ORA", "INY");
    register_instr(0x48, "PHA", "IMP");
    register_instr(0x08, "PHP", "IMP");
    register_instr(0x68, "PLA", "IMP");
    register_instr(0x28, "PLP", "IMP");
    register_instr(0x2A, "ROL", "ACC");
    register_instr(0x26, "ROL", "ZER");
    register_instr(0x36, "ROL", "ZEX");
    register_instr(0x2E, "ROL", "ABS");
    register_instr(0x3E, "ROL", "ABX");
    register_instr(0x6A, "ROR", "ACC");
    register_instr(0x66, "ROR", "ZER");
    register_instr(0x76, "ROR", "ZEX");
    register_instr(0x6E, "ROR", "ABS");
    register_instr(0x7E, "ROR", "ABX");
    register_instr(0x40, "RTI", "IMP");
    register_instr(0x60, "RTS", "IMP");
    register_instr(0xE9, "SBC", "IMM");
    register_instr(0xE5, "SBC", "ZER");
    register_instr(0xF5, "SBC", "ZEX");
    register_instr(0xED, "SBC", "ABS");
    register_instr(0xFD, "SBC", "ABX");
    register_instr(0xF9, "SBC", "ABY");
    register_instr(0xE1, "SBC", "INX");
    register_instr(0xF1, "SBC", "INY");
    register_instr(0x38, "SEC", "IMP");
    register_instr(0xF8, "SED", "IMP");
    register_instr(0x78, "SEI", "IMP");
    register_instr(0x85, "STA", "ZER");
    register_instr(0x95, "STA", "ZEX");
    register_instr(0x8D, "STA", "ABS");
    register_instr(0x9D, "STA", "ABX");
    register_instr(0x99, "STA", "ABY");
    register_instr(0x81, "STA", "INX");
    register_instr(0x91, "STA", "INY");
    register_instr(0x86, "STX", "ZER");
    register_instr(0x96, "STX", "ZEY");
    register_instr(0x8E, "STX", "ABS");
    register_instr(0x84, "STY", "ZER");
    register_instr(0x94, "STY", "ZEX");
    register_instr(0x8C, "STY", "ABS");
    register_instr(0xAA, "TAX", "IMP");
    register_instr(0xA8, "TAY", "IMP");
    register_instr(0xBA, "TSX", "IMP");
    register_instr(0x8A, "TXA", "IMP");
    register_instr(0x9A, "TXS", "IMP");
    register_instr(0x98, "TYA", "IMP");
}

#include <iostream>

std::string Disassembler::instr_to_string(Disassembler::Instr instr, uint16_t PC, uint16_t src) {
    std::stringstream instr_ss;

    // TODO: Relative addresses and such
    if (instr.mode == &addr_modes.at("REL")) {
        src = PC + 2 + (int16_t) src;
    }
    instr_ss << std::uppercase << instr.opcode << std::hex << std::setfill('0');
    size_t len = strlen(instr.mode->format);
    if (len != 0) instr_ss << " ";
    for (int i = 0; i < len; i++) {
        switch (instr.mode->format[i]) {
            case 'b':
                instr_ss << std::setw(2) << src;
                break;

            case 'w':
                instr_ss << std::setw(4) << src;
                break;

            default:
                instr_ss << instr.mode->format[i];
                break;
        }
    }

    return instr_ss.str();
}

void Disassembler::file_to_strings(std::ifstream &file) {
    uint8_t opcode;
    uint16_t src;

    char temp;
    uint16_t PC = base;
    while (file.read(&temp, 1)) {
        opcode = temp;
        // std::cout << std::hex << std::setw(2) << std::setfill('0') << (int) opcode << ' ';
        src = 0;
        for (int i = 0; i < instr_table[opcode].mode->length; i++) {
            file.read(&temp, 1);
            // std::cout << std::hex << std::setw(2) << std::setfill('0') << (int) temp << ' ';
            src |= temp << 8*i;
        }
        // std::cout << std::hex << std::setw(4) << std::setfill('0') << (int) src << '\n';
        std::cout << std::hex << std::setfill('0') << std::setw(4) << (int) PC << " " << std::setw(2) << (int) opcode << " " << std::setw(4) << (int) src << " " << instr_to_string(instr_table[opcode], PC, src) << std::endl;
        instructions.push_back(instr_to_string(instr_table[opcode], PC, src));
        PC += instr_table[opcode].mode->length + 1;
    }
}

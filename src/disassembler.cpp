#include <iostream>

#include "disassembler.h"

std::string Disassembler::instr_to_string(InstrInfo info, uint16_t PC, uint16_t src) {
    std::stringstream instr_ss;

    // TODO: Relative addresses and such
    if (info.mode_str.compare("REL") == 0) {
        src = PC + 2 + (int16_t) src;
    }
    instr_ss << std::uppercase << info.op_str << std::hex << std::setfill('0');
    std::string format = Instructions::mode_map.at(info.mode_str).format;
    size_t len = format.length();
    if (len != 0) instr_ss << " ";
    for (int i = 0; i < len; i++) {
        switch (format[i]) {
            case 'b':
                instr_ss << std::setw(2) << src;
                break;

            case 'w':
                instr_ss << std::setw(4) << src;
                break;

            default:
                instr_ss << format[i];
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
    InstrInfo info;
    short length;
    while (file.read(&temp, 1)) {
        opcode = temp;
        info = Instructions::instr_map.at(opcode);
        length = Instructions::mode_map.at(info.mode_str).length;

        // std::cout << std::hex << std::setw(2) << std::setfill('0') << (int) opcode << ' ';
        src = 0;
        for (int i = 0; i < length; i++) {
            file.read(&temp, 1);
            // std::cout << std::hex << std::setw(2) << std::setfill('0') << (int) temp << ' ';
            src |= temp << 8*i;
        }
        // std::cout << std::hex << std::setw(4) << std::setfill('0') << (int) src << '\n';
        std::cout << std::hex << std::setfill('0') << std::setw(4) << (int) PC << " " << std::setw(2) << (int) opcode << " " << std::setw(4) << (int) src << " " << instr_to_string(info, PC, src) << std::endl;
        instructions.push_back(instr_to_string(info, PC, src));
        PC += length + 1;
    }
}

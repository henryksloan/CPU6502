#include "cpu_6502.h"
#include "memory.h"

CPU6502::CPU6502(std::shared_ptr<Memory> mem)
        : mem{std::move(mem)},
          A{0}, X{0}, Y{0}, P{0}, S{0},
          PC{0x10} {
    
    instr_table.fill(nullptr); // TODO: Fill invalids

    instr_table[0x69] = new CPU6502::Instr(this, 2, &CPU6502::Op_ADC, &CPU6502::Addr_IMM);
    instr_table[0x65] = new CPU6502::Instr(this, 3, &CPU6502::Op_ADC, &CPU6502::Addr_ZER);
    instr_table[0x75] = new CPU6502::Instr(this, 4, &CPU6502::Op_ADC, &CPU6502::Addr_ZEX);
    instr_table[0x6D] = new CPU6502::Instr(this, 4, &CPU6502::Op_ADC, &CPU6502::Addr_ABS);
    instr_table[0x7D] = new CPU6502::Instr(this, 4, &CPU6502::Op_ADC, &CPU6502::Addr_ABX);
    instr_table[0x79] = new CPU6502::Instr(this, 4, &CPU6502::Op_ADC, &CPU6502::Addr_ABY);
    instr_table[0x61] = new CPU6502::Instr(this, 6, &CPU6502::Op_ADC, &CPU6502::Addr_INX);
    instr_table[0x71] = new CPU6502::Instr(this, 5, &CPU6502::Op_ADC, &CPU6502::Addr_INY);

    instr_table[0x29] = new CPU6502::Instr(this, 2, &CPU6502::Op_AND, &CPU6502::Addr_IMM);
    instr_table[0x25] = new CPU6502::Instr(this, 3, &CPU6502::Op_AND, &CPU6502::Addr_ZER);
    instr_table[0x35] = new CPU6502::Instr(this, 4, &CPU6502::Op_AND, &CPU6502::Addr_ZEX);
    instr_table[0x2D] = new CPU6502::Instr(this, 4, &CPU6502::Op_AND, &CPU6502::Addr_ABS);
    instr_table[0x3D] = new CPU6502::Instr(this, 4, &CPU6502::Op_AND, &CPU6502::Addr_ABX);
    instr_table[0x39] = new CPU6502::Instr(this, 4, &CPU6502::Op_AND, &CPU6502::Addr_ABY);
    instr_table[0x21] = new CPU6502::Instr(this, 6, &CPU6502::Op_AND, &CPU6502::Addr_INX);
    instr_table[0x31] = new CPU6502::Instr(this, 5, &CPU6502::Op_AND, &CPU6502::Addr_INY);

    instr_table[0x0A] = new CPU6502::Instr(this, 2, &CPU6502::Op_ASL_A, &CPU6502::Addr_ACC);
    instr_table[0x06] = new CPU6502::Instr(this, 5, &CPU6502::Op_ASL, &CPU6502::Addr_ZER);
    instr_table[0x16] = new CPU6502::Instr(this, 6, &CPU6502::Op_ASL, &CPU6502::Addr_ZEX);
    instr_table[0x0E] = new CPU6502::Instr(this, 6, &CPU6502::Op_ASL, &CPU6502::Addr_ABS);
    instr_table[0x1E] = new CPU6502::Instr(this, 7, &CPU6502::Op_ASL, &CPU6502::Addr_ABX);

    instr_table[0x90] = new CPU6502::Instr(this, 2, &CPU6502::Op_BCC, &CPU6502::Addr_REL);
    instr_table[0xB0] = new CPU6502::Instr(this, 2, &CPU6502::Op_BCS, &CPU6502::Addr_REL);
    instr_table[0xF0] = new CPU6502::Instr(this, 2, &CPU6502::Op_BEQ, &CPU6502::Addr_REL);

    instr_table[0x24] = new CPU6502::Instr(this, 3, &CPU6502::Op_BIT, &CPU6502::Addr_ZER);
    instr_table[0x2C] = new CPU6502::Instr(this, 4, &CPU6502::Op_BIT, &CPU6502::Addr_ABS);

    instr_table[0x30] = new CPU6502::Instr(this, 2, &CPU6502::Op_BMI, &CPU6502::Addr_REL);
    instr_table[0xD0] = new CPU6502::Instr(this, 2, &CPU6502::Op_BNE, &CPU6502::Addr_REL);
    instr_table[0x10] = new CPU6502::Instr(this, 2, &CPU6502::Op_BPL, &CPU6502::Addr_REL);

    instr_table[0x00] = new CPU6502::Instr(this, 7, &CPU6502::Op_BRK, &CPU6502::Addr_IMP);

    instr_table[0x50] = new CPU6502::Instr(this, 2, &CPU6502::Op_BVC, &CPU6502::Addr_REL);
    instr_table[0x70] = new CPU6502::Instr(this, 2, &CPU6502::Op_BVS, &CPU6502::Addr_REL);

    instr_table[0x18] = new CPU6502::Instr(this, 2, &CPU6502::Op_CLC, &CPU6502::Addr_IMP);
    instr_table[0xD8] = new CPU6502::Instr(this, 2, &CPU6502::Op_CLD, &CPU6502::Addr_IMP);
    instr_table[0x58] = new CPU6502::Instr(this, 2, &CPU6502::Op_CLI, &CPU6502::Addr_IMP);
    instr_table[0xB8] = new CPU6502::Instr(this, 2, &CPU6502::Op_CLV, &CPU6502::Addr_IMP);

    instr_table[0xC9] = new CPU6502::Instr(this, 2, &CPU6502::Op_CMP, &CPU6502::Addr_IMM);
    instr_table[0xC5] = new CPU6502::Instr(this, 3, &CPU6502::Op_CMP, &CPU6502::Addr_ZER);
    instr_table[0xD5] = new CPU6502::Instr(this, 4, &CPU6502::Op_CMP, &CPU6502::Addr_ZEX);
    instr_table[0xCD] = new CPU6502::Instr(this, 4, &CPU6502::Op_CMP, &CPU6502::Addr_ABS);
    instr_table[0xDD] = new CPU6502::Instr(this, 4, &CPU6502::Op_CMP, &CPU6502::Addr_ABX);
    instr_table[0xD9] = new CPU6502::Instr(this, 4, &CPU6502::Op_CMP, &CPU6502::Addr_ABY);
    instr_table[0xC1] = new CPU6502::Instr(this, 6, &CPU6502::Op_CMP, &CPU6502::Addr_INX);
    instr_table[0xD1] = new CPU6502::Instr(this, 5, &CPU6502::Op_CMP, &CPU6502::Addr_INY);

    instr_table[0xE0] = new CPU6502::Instr(this, 2, &CPU6502::Op_CPX, &CPU6502::Addr_IMM);
    instr_table[0xE4] = new CPU6502::Instr(this, 3, &CPU6502::Op_CPX, &CPU6502::Addr_ZER);
    instr_table[0xEC] = new CPU6502::Instr(this, 4, &CPU6502::Op_CPX, &CPU6502::Addr_ABS);

    instr_table[0xC0] = new CPU6502::Instr(this, 2, &CPU6502::Op_CPY, &CPU6502::Addr_IMM);
    instr_table[0xC4] = new CPU6502::Instr(this, 3, &CPU6502::Op_CPY, &CPU6502::Addr_ZER);
    instr_table[0xCC] = new CPU6502::Instr(this, 4, &CPU6502::Op_CPY, &CPU6502::Addr_ABS);

    instr_table[0xC6] = new CPU6502::Instr(this, 5, &CPU6502::Op_DEC, &CPU6502::Addr_ZER);
    instr_table[0xD6] = new CPU6502::Instr(this, 6, &CPU6502::Op_DEC, &CPU6502::Addr_ZEX);
    instr_table[0xCE] = new CPU6502::Instr(this, 6, &CPU6502::Op_DEC, &CPU6502::Addr_ABS);
    instr_table[0xDE] = new CPU6502::Instr(this, 7, &CPU6502::Op_DEC, &CPU6502::Addr_ABX);

    instr_table[0xCA] = new CPU6502::Instr(this, 2, &CPU6502::Op_DEX, &CPU6502::Addr_IMP);
    instr_table[0x88] = new CPU6502::Instr(this, 2, &CPU6502::Op_DEY, &CPU6502::Addr_IMP);

    instr_table[0x49] = new CPU6502::Instr(this, 2, &CPU6502::Op_EOR, &CPU6502::Addr_IMM);
    instr_table[0x45] = new CPU6502::Instr(this, 3, &CPU6502::Op_EOR, &CPU6502::Addr_ZER);
    instr_table[0x55] = new CPU6502::Instr(this, 4, &CPU6502::Op_EOR, &CPU6502::Addr_ZEX);
    instr_table[0x4D] = new CPU6502::Instr(this, 4, &CPU6502::Op_EOR, &CPU6502::Addr_ABS);
    instr_table[0x5D] = new CPU6502::Instr(this, 4, &CPU6502::Op_EOR, &CPU6502::Addr_ABX);
    instr_table[0x59] = new CPU6502::Instr(this, 4, &CPU6502::Op_EOR, &CPU6502::Addr_ABY);
    instr_table[0x41] = new CPU6502::Instr(this, 6, &CPU6502::Op_EOR, &CPU6502::Addr_INX);
    instr_table[0x51] = new CPU6502::Instr(this, 5, &CPU6502::Op_EOR, &CPU6502::Addr_INY);

    instr_table[0xE6] = new CPU6502::Instr(this, 5, &CPU6502::Op_INC, &CPU6502::Addr_ZER);
    instr_table[0xF6] = new CPU6502::Instr(this, 6, &CPU6502::Op_INC, &CPU6502::Addr_ZEX);
    instr_table[0xEE] = new CPU6502::Instr(this, 6, &CPU6502::Op_INC, &CPU6502::Addr_ABS);
    instr_table[0xFE] = new CPU6502::Instr(this, 7, &CPU6502::Op_INC, &CPU6502::Addr_ABX);

    instr_table[0xEA] = new CPU6502::Instr(this, 2, &CPU6502::Op_INX, &CPU6502::Addr_IMP);
    instr_table[0xC8] = new CPU6502::Instr(this, 2, &CPU6502::Op_INY, &CPU6502::Addr_IMP);

    instr_table[0x4C] = new CPU6502::Instr(this, 3, &CPU6502::Op_JMP, &CPU6502::Addr_ABS);
    instr_table[0x6C] = new CPU6502::Instr(this, 5, &CPU6502::Op_JMP, &CPU6502::Addr_ABI);
    instr_table[0x20] = new CPU6502::Instr(this, 6, &CPU6502::Op_JSR, &CPU6502::Addr_ABS);

    instr_table[0xA9] = new CPU6502::Instr(this, 2, &CPU6502::Op_LDA, &CPU6502::Addr_IMM);
    instr_table[0xA5] = new CPU6502::Instr(this, 3, &CPU6502::Op_LDA, &CPU6502::Addr_ZER);
    instr_table[0xB5] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDA, &CPU6502::Addr_ZEX);
    instr_table[0xAD] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDA, &CPU6502::Addr_ABS);
    instr_table[0xBD] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDA, &CPU6502::Addr_ABX);
    instr_table[0xB9] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDA, &CPU6502::Addr_ABY);
    instr_table[0xA1] = new CPU6502::Instr(this, 6, &CPU6502::Op_LDA, &CPU6502::Addr_INX);
    instr_table[0xB1] = new CPU6502::Instr(this, 5, &CPU6502::Op_LDA, &CPU6502::Addr_INY);

    instr_table[0xA2] = new CPU6502::Instr(this, 2, &CPU6502::Op_LDX, &CPU6502::Addr_IMM);
    instr_table[0xA6] = new CPU6502::Instr(this, 3, &CPU6502::Op_LDX, &CPU6502::Addr_ZER);
    instr_table[0xB6] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDX, &CPU6502::Addr_ZEX);
    instr_table[0xAE] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDX, &CPU6502::Addr_ABS);
    instr_table[0xBE] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDX, &CPU6502::Addr_ABX);

    instr_table[0xA0] = new CPU6502::Instr(this, 2, &CPU6502::Op_LDY, &CPU6502::Addr_IMM);
    instr_table[0xA4] = new CPU6502::Instr(this, 3, &CPU6502::Op_LDY, &CPU6502::Addr_ZER);
    instr_table[0xB4] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDY, &CPU6502::Addr_ZEX);
    instr_table[0xAC] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDY, &CPU6502::Addr_ABS);
    instr_table[0xBC] = new CPU6502::Instr(this, 4, &CPU6502::Op_LDY, &CPU6502::Addr_ABX);

    instr_table[0x4A] = new CPU6502::Instr(this, 2, &CPU6502::Op_LSR_A, &CPU6502::Addr_ACC);
    instr_table[0x46] = new CPU6502::Instr(this, 5, &CPU6502::Op_LSR, &CPU6502::Addr_ZER);
    instr_table[0x56] = new CPU6502::Instr(this, 6, &CPU6502::Op_LSR, &CPU6502::Addr_ZEX);
    instr_table[0x4E] = new CPU6502::Instr(this, 6, &CPU6502::Op_LSR, &CPU6502::Addr_ABS);
    instr_table[0x5E] = new CPU6502::Instr(this, 7, &CPU6502::Op_LSR, &CPU6502::Addr_ABX);

    instr_table[0xEA] = new CPU6502::Instr(this, 2, &CPU6502::Op_NOP, &CPU6502::Addr_IMP);

    instr_table[0x09] = new CPU6502::Instr(this, 2, &CPU6502::Op_ORA, &CPU6502::Addr_IMM);
    instr_table[0x05] = new CPU6502::Instr(this, 3, &CPU6502::Op_ORA, &CPU6502::Addr_ZER);
    instr_table[0x15] = new CPU6502::Instr(this, 4, &CPU6502::Op_ORA, &CPU6502::Addr_ZEX);
    instr_table[0x0D] = new CPU6502::Instr(this, 4, &CPU6502::Op_ORA, &CPU6502::Addr_ABS);
    instr_table[0x1D] = new CPU6502::Instr(this, 4, &CPU6502::Op_ORA, &CPU6502::Addr_ABX);
    instr_table[0x19] = new CPU6502::Instr(this, 4, &CPU6502::Op_ORA, &CPU6502::Addr_ABY);
    instr_table[0x01] = new CPU6502::Instr(this, 6, &CPU6502::Op_ORA, &CPU6502::Addr_INX);
    instr_table[0x11] = new CPU6502::Instr(this, 5, &CPU6502::Op_ORA, &CPU6502::Addr_INY);

    instr_table[0x48] = new CPU6502::Instr(this, 3, &CPU6502::Op_PHA, &CPU6502::Addr_IMP);
    instr_table[0x08] = new CPU6502::Instr(this, 3, &CPU6502::Op_PHP, &CPU6502::Addr_IMP);
    instr_table[0x68] = new CPU6502::Instr(this, 4, &CPU6502::Op_PLA, &CPU6502::Addr_IMP);
    instr_table[0x28] = new CPU6502::Instr(this, 4, &CPU6502::Op_PLP, &CPU6502::Addr_IMP);

    instr_table[0x2A] = new CPU6502::Instr(this, 2, &CPU6502::Op_ROL_A, &CPU6502::Addr_ACC);
    instr_table[0x26] = new CPU6502::Instr(this, 5, &CPU6502::Op_ROL, &CPU6502::Addr_ZER);
    instr_table[0x36] = new CPU6502::Instr(this, 6, &CPU6502::Op_ROL, &CPU6502::Addr_ZEX);
    instr_table[0x2E] = new CPU6502::Instr(this, 6, &CPU6502::Op_ROL, &CPU6502::Addr_ABS);
    instr_table[0x3E] = new CPU6502::Instr(this, 7, &CPU6502::Op_ROL, &CPU6502::Addr_ABX);

    instr_table[0x2A] = new CPU6502::Instr(this, 2, &CPU6502::Op_ROR_A, &CPU6502::Addr_ACC);
    instr_table[0x26] = new CPU6502::Instr(this, 5, &CPU6502::Op_ROR, &CPU6502::Addr_ZER);
    instr_table[0x36] = new CPU6502::Instr(this, 6, &CPU6502::Op_ROR, &CPU6502::Addr_ZEX);
    instr_table[0x2E] = new CPU6502::Instr(this, 6, &CPU6502::Op_ROR, &CPU6502::Addr_ABS);
    instr_table[0x3E] = new CPU6502::Instr(this, 7, &CPU6502::Op_ROR, &CPU6502::Addr_ABX);

    instr_table[0x40] = new CPU6502::Instr(this, 6, &CPU6502::Op_RTI, &CPU6502::Addr_IMP);
    instr_table[0x60] = new CPU6502::Instr(this, 6, &CPU6502::Op_RTS, &CPU6502::Addr_IMP);

    instr_table[0xE9] = new CPU6502::Instr(this, 2, &CPU6502::Op_SBC, &CPU6502::Addr_IMM);
    instr_table[0xE5] = new CPU6502::Instr(this, 3, &CPU6502::Op_SBC, &CPU6502::Addr_ZER);
    instr_table[0xF5] = new CPU6502::Instr(this, 4, &CPU6502::Op_SBC, &CPU6502::Addr_ZEX);
    instr_table[0xED] = new CPU6502::Instr(this, 4, &CPU6502::Op_SBC, &CPU6502::Addr_ABS);
    instr_table[0xFD] = new CPU6502::Instr(this, 4, &CPU6502::Op_SBC, &CPU6502::Addr_ABX);
    instr_table[0xF9] = new CPU6502::Instr(this, 4, &CPU6502::Op_SBC, &CPU6502::Addr_ABY);
    instr_table[0xE1] = new CPU6502::Instr(this, 6, &CPU6502::Op_SBC, &CPU6502::Addr_INX);
    instr_table[0xF1] = new CPU6502::Instr(this, 5, &CPU6502::Op_SBC, &CPU6502::Addr_INY);

    instr_table[0x38] = new CPU6502::Instr(this, 2, &CPU6502::Op_SEC, &CPU6502::Addr_IMP);
    instr_table[0xF8] = new CPU6502::Instr(this, 2, &CPU6502::Op_SED, &CPU6502::Addr_IMP);
    instr_table[0x78] = new CPU6502::Instr(this, 2, &CPU6502::Op_SEI, &CPU6502::Addr_IMP);

    instr_table[0x85] = new CPU6502::Instr(this, 3, &CPU6502::Op_STA, &CPU6502::Addr_ZER);
    instr_table[0x95] = new CPU6502::Instr(this, 4, &CPU6502::Op_STA, &CPU6502::Addr_ZEX);
    instr_table[0x8D] = new CPU6502::Instr(this, 4, &CPU6502::Op_STA, &CPU6502::Addr_ABS);
    instr_table[0x9D] = new CPU6502::Instr(this, 5, &CPU6502::Op_STA, &CPU6502::Addr_ABS);
    instr_table[0x99] = new CPU6502::Instr(this, 5, &CPU6502::Op_STA, &CPU6502::Addr_ABY);
    instr_table[0x81] = new CPU6502::Instr(this, 6, &CPU6502::Op_STA, &CPU6502::Addr_INX);
    instr_table[0x91] = new CPU6502::Instr(this, 6, &CPU6502::Op_STA, &CPU6502::Addr_INY);

    instr_table[0x86] = new CPU6502::Instr(this, 3, &CPU6502::Op_STX, &CPU6502::Addr_ZER);
    instr_table[0x96] = new CPU6502::Instr(this, 4, &CPU6502::Op_STX, &CPU6502::Addr_ZEY);
    instr_table[0x8E] = new CPU6502::Instr(this, 4, &CPU6502::Op_STX, &CPU6502::Addr_ABS);

    instr_table[0x84] = new CPU6502::Instr(this, 3, &CPU6502::Op_STY, &CPU6502::Addr_ZER);
    instr_table[0x94] = new CPU6502::Instr(this, 4, &CPU6502::Op_STY, &CPU6502::Addr_ZEX);
    instr_table[0x8C] = new CPU6502::Instr(this, 4, &CPU6502::Op_STY, &CPU6502::Addr_ABS);

    instr_table[0xAA] = new CPU6502::Instr(this, 2, &CPU6502::Op_TAX, &CPU6502::Addr_IMP);
    instr_table[0xA8] = new CPU6502::Instr(this, 2, &CPU6502::Op_TAY, &CPU6502::Addr_IMP);
    instr_table[0xBA] = new CPU6502::Instr(this, 2, &CPU6502::Op_TSX, &CPU6502::Addr_IMP);
    instr_table[0x8A] = new CPU6502::Instr(this, 2, &CPU6502::Op_TXA, &CPU6502::Addr_IMP);
    instr_table[0x9A] = new CPU6502::Instr(this, 2, &CPU6502::Op_TXS, &CPU6502::Addr_IMP);
    instr_table[0x98] = new CPU6502::Instr(this, 2, &CPU6502::Op_TYA, &CPU6502::Addr_IMP);
}

/*
 * Addressing modes
 * Return the address an instruction will use to reference memory
 * Does NOT return the VALUE at that memory (hence IMM returns PC+1)
 */
uint16_t CPU6502::Addr_ACC() { return 0; } // Not implemented
uint16_t CPU6502::Addr_IMM() { return PC+1; }
uint16_t CPU6502::Addr_ABS() { return mem->read_word(PC+1); }
uint16_t CPU6502::Addr_ZER() { return mem->read_byte(PC+1); }
uint16_t CPU6502::Addr_ZEX() { return mem->read_byte(PC+1) + (int8_t) X; } // TODO Fix looping around
uint16_t CPU6502::Addr_ZEY() { return mem->read_byte(PC+1) + (int8_t) Y; } // TODO Fix looping around
uint16_t CPU6502::Addr_ABX() { return mem->read_word(PC+1) + (int8_t) X; } // TODO Fix looping around
uint16_t CPU6502::Addr_ABY() { return mem->read_word(PC+1) + (int8_t) Y; } // TODO Fix looping around
uint16_t CPU6502::Addr_IMP() { return 0; }
uint16_t CPU6502::Addr_REL() { return PC + (int8_t) mem->read_byte(PC+1); }
uint16_t CPU6502::Addr_INX() { return mem->read_word(mem->read_word(PC+1) + (int8_t) X); } // TODO Fix looping around
uint16_t CPU6502::Addr_INY() { return mem->read_word(mem->read_word(PC+1)) + (int8_t) Y; } // TODO Fix looping around
uint16_t CPU6502::Addr_ABI() { return mem->read_word(PC+1); } // TODO: Check this; should it just return the indirect?

void CPU6502::Op_ADC(uint16_t src) {
    // TODO: Fix various behaviors
    // TODO: Implement overflow
    // http://www.6502.org/tutorials/decimal_mode.html
    uint8_t m = mem->read_byte(src);
    unsigned int sum = A + m + get_flag(CARRY);
    set_flag(OVERFLOW, sum>0xFF);
    if (get_flag(DECIMAL)) {
        sum = from_bcd(A) + from_bcd(m) + get_flag(CARRY);
        set_flag(ZERO, sum == 0); // TODO: What about carry and overflow?
        set_flag(NEGATIVE, sum & 0x80);
        set_flag(CARRY, sum > 99);
        A = to_bcd(sum % 100);
    }
    else {
        set_flag(ZERO, sum == 0); // TODO: What about carry and overflow?
        set_flag(NEGATIVE, sum & 0x80);
        set_flag(CARRY, sum > 0xFF);
        A = sum & 0xFF;
    }
}

void CPU6502::Op_AND(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    A &= m;
    set_flag(ZERO, A == 0);
    set_flag(NEGATIVE, A & 0x80);
}

void CPU6502::Op_ASL(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    set_flag(CARRY, m & 0x80);
    m = (m << 1) & 0xFE;
    set_flag(NEGATIVE, m & 0x80);
    set_flag(ZERO, m == 0);
    mem->write_byte(src, m);
}

void CPU6502::Op_ASL_A(uint16_t) {
    set_flag(CARRY, A & 0x80);
    A = (A << 1) & 0xFE;
    set_flag(NEGATIVE, A & 0x80);
    set_flag(ZERO, A == 0);
}

void CPU6502::Op_BCC(uint16_t src) {
    if (!get_flag(CARRY)) {
        PC = mem->read_byte(src);
    }
}

void CPU6502::Op_BCS(uint16_t src) {
    if (get_flag(CARRY)) {
        PC = mem->read_byte(src);
    }
}

void CPU6502::Op_BEQ(uint16_t src) {
    if (get_flag(ZERO)) {
        PC = mem->read_byte(src);
    }
}

void CPU6502::Op_BIT(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    P &= (m & 0xC0) | 0x3F;
    set_flag(ZERO, m & A);
}

void CPU6502::Op_BMI(uint16_t src) {
    if (get_flag(NEGATIVE)) {
        PC = mem->read_byte(src);
    }
}

void CPU6502::Op_BNE(uint16_t src) {
    if (!get_flag(ZERO)) {
        PC = mem->read_byte(src);
    }
}

void CPU6502::Op_BPL(uint16_t src) {
    if (!get_flag(NEGATIVE)) {
        PC = mem->read_byte(src);
    }
}

void CPU6502::Op_BRK(uint16_t src) {
    set_flag(INTERRUPT, 1);
    stack_push_word(PC+2);
    stack_push(P);
}

void CPU6502::Op_BVC(uint16_t src) {
    if (!get_flag(OVERFLOW)) {
        PC = mem->read_byte(src);
    }
}

void CPU6502::Op_BVS(uint16_t src) {
    if (get_flag(OVERFLOW)) {
        PC = mem->read_byte(src);
    }
}

void CPU6502::Op_CLC(uint16_t src) { set_flag(CARRY, 0); }
void CPU6502::Op_CLD(uint16_t src) { set_flag(DECIMAL, 0); }
void CPU6502::Op_CLI(uint16_t src) { set_flag(INTERRUPT, 0); }
void CPU6502::Op_CLV(uint16_t src) { set_flag(OVERFLOW, 0); }

void CPU6502::Op_CMP(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    int diff = A - m;
    set_flag(NEGATIVE, diff < 0);
    set_flag(ZERO, diff == 0);
    set_flag(CARRY, diff >= 0);
}

void CPU6502::Op_CPX(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    int diff = X - m;
    set_flag(NEGATIVE, diff < 0);
    set_flag(ZERO, diff == 0);
    set_flag(CARRY, diff >= 0);
}

void CPU6502::Op_CPY(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    int diff = Y - m;
    set_flag(NEGATIVE, diff < 0);
    set_flag(ZERO, diff == 0);
    set_flag(CARRY, diff >= 0);
}

void CPU6502::Op_DEC(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    m--;
    set_flag(NEGATIVE, m & 0x80);
    set_flag(ZERO, m == 0);
    mem->write_byte(src, m);
}

void CPU6502::Op_DEX(uint16_t src) {
    X--;
    set_flag(NEGATIVE, X & 0x80);
    set_flag(ZERO, X == 0);
}

void CPU6502::Op_DEY(uint16_t src) {
    Y--;
    set_flag(NEGATIVE, Y & 0x80);
    set_flag(ZERO, Y == 0);
}

void CPU6502::Op_EOR(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    A ^= m;
    set_flag(ZERO, A == 0);
    set_flag(NEGATIVE, A & 0x80);
}

void CPU6502::Op_INC(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    m++;
    set_flag(NEGATIVE, m & 0x80);
    set_flag(ZERO, m == 0);
    mem->write_byte(src, m);
}

void CPU6502::Op_INX(uint16_t src) {
    X++;
    set_flag(NEGATIVE, X & 0x80);
    set_flag(ZERO, X == 0);
}

void CPU6502::Op_INY(uint16_t src) {
    Y++;
    set_flag(NEGATIVE, Y & 0x80);
    set_flag(ZERO, Y == 0);
}

void CPU6502::Op_JMP(uint16_t src) {
    PC = mem->read_byte(src); // TODO: ??
}

void CPU6502::Op_JSR(uint16_t src) {
    stack_push_word(PC);
    PC = mem->read_byte(src); // TODO: ??
}

void CPU6502::Op_LDA(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    A = m;
    set_flag(NEGATIVE, A & 0x80);
    set_flag(ZERO, A == 0);
}

void CPU6502::Op_LDX(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    X = m;
    set_flag(NEGATIVE, X & 0x80);
    set_flag(ZERO, X == 0);
}

void CPU6502::Op_LDY(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    X = m;
    set_flag(NEGATIVE, X & 0x80);
    set_flag(ZERO, X == 0);
}

void CPU6502::Op_LSR(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    set_flag(CARRY, m & 0x1);
    m = (m >> 1) & 0x7F;
    set_flag(NEGATIVE, 0);
    set_flag(ZERO, m == 0);
    mem->write_byte(src, m);
}

void CPU6502::Op_LSR_A(uint16_t) {
    set_flag(CARRY, A & 0x1);
    A = (A >> 1) & 0x7F;
    set_flag(NEGATIVE, 0);
    set_flag(ZERO, A == 0);
}

void CPU6502::Op_NOP(uint16_t src) {}

void CPU6502::Op_ORA(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    A |= m;
    set_flag(ZERO, A == 0);
    set_flag(NEGATIVE, A & 0x80);
}

void CPU6502::Op_PHA(uint16_t src) { stack_push(A); }
void CPU6502::Op_PHP(uint16_t src) { stack_push(P); }
void CPU6502::Op_PLA(uint16_t src) { A = stack_pop(); }
void CPU6502::Op_PLP(uint16_t src) { P = stack_pop(); }

void CPU6502::Op_ROL(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    unsigned char temp = get_flag(CARRY);
    set_flag(CARRY, m & 0x80);
    m = (m << 1);
    m = (temp) ? (m | 0x1) : (m & 0xFE);
    set_flag(NEGATIVE, m & 0x80);
    set_flag(ZERO, m == 0);
    mem->write_byte(src, m);
}

void CPU6502::Op_ROL_A(uint16_t src) {
    unsigned char temp = get_flag(CARRY);
    set_flag(CARRY, A & 0x80);
    A = (A << 1);
    A = (temp) ? (A | 0x1) : (A & 0xFE);
    set_flag(NEGATIVE, A & 0x80);
    set_flag(ZERO, A == 0);
}

void CPU6502::Op_ROR(uint16_t src) {
    uint8_t m = mem->read_byte(src);
    unsigned char temp = get_flag(CARRY);
    set_flag(CARRY, m & 0x1);
    m = (m >> 1);
    m = (temp) ? (m | 0x80) : (m & 0x7F);
    set_flag(NEGATIVE, m & 0x80);
    set_flag(ZERO, m == 0);
    mem->write_byte(src, m);
}

void CPU6502::Op_ROR_A(uint16_t src) {
    unsigned char temp = get_flag(CARRY);
    set_flag(CARRY, A & 0x1);
    A = (A >> 1);
    A = (temp) ? (A | 0x80) : (A & 0x7F);
    set_flag(NEGATIVE, A & 0x80);
    set_flag(ZERO, A == 0);
}

void CPU6502::Op_RTI(uint16_t src) {
    P = stack_pop();
    PC = stack_pop_word();
}

void CPU6502::Op_RTS(uint16_t src) {
    PC = stack_pop_word() + 1;
}

void CPU6502::Op_SBC(uint16_t src) {
    // TODO: Fix various behaviors
    // TODO: Implement overflow
    // http://www.6502.org/tutorials/decimal_mode.html
    uint8_t m = mem->read_byte(src);
    unsigned int diff = A - m - get_flag(CARRY);
    set_flag(OVERFLOW, diff>0xFF);
    if (get_flag(DECIMAL)) {
        diff = from_bcd(A) - from_bcd(m) - get_flag(CARRY);
        set_flag(ZERO, diff == 0); // TODO: What about carry and overflow?
        set_flag(NEGATIVE, diff & 0x80);
        set_flag(CARRY, ~(diff & 0x80));
        A = to_bcd(diff % 100);
    }
    else {
        set_flag(ZERO, diff == 0); // TODO: What about carry and overflow?
        set_flag(NEGATIVE, diff & 0x80);
        set_flag(CARRY, ~(diff & 0x80));
        A = diff & 0xFF;
    }
}

void CPU6502::Op_SEC(uint16_t src) { set_flag(CARRY, 1); }
void CPU6502::Op_SED(uint16_t src) { set_flag(DECIMAL, 1); }
void CPU6502::Op_SEI(uint16_t src) { set_flag(INTERRUPT, 1); }

void CPU6502::Op_STA(uint16_t src) {
    mem->write_byte(src, A);
}

void CPU6502::Op_STX(uint16_t src) {
    mem->write_byte(src, X);
}

void CPU6502::Op_STY(uint16_t src) {
    mem->write_byte(src, Y);
}

void CPU6502::Op_TAX(uint16_t src) {
    X = A;
    set_flag(NEGATIVE, X & 0x80);
    set_flag(ZERO, X == 0);
}

void CPU6502::Op_TAY(uint16_t src) {
    Y = A;
    set_flag(NEGATIVE, Y & 0x80);
    set_flag(ZERO, Y == 0);
}

void CPU6502::Op_TSX(uint16_t src) {
    X = S;
    set_flag(NEGATIVE, X & 0x80);
    set_flag(ZERO, X == 0);
}

void CPU6502::Op_TXA(uint16_t src) {
    A = X;
    set_flag(NEGATIVE, A & 0x80);
    set_flag(ZERO, A == 0);
}

void CPU6502::Op_TXS(uint16_t src) {
    S = X;
    set_flag(NEGATIVE, S & 0x80);
    set_flag(ZERO, S == 0);
}

void CPU6502::Op_TYA(uint16_t src) {
    A = Y;
    set_flag(NEGATIVE, A & 0x80);
    set_flag(ZERO, A == 0);
}

#include <iostream>
int main(int argc, char **argv) {
    std::shared_ptr<Memory> mem = std::make_shared<Memory>(Memory());
    mem->write_byte(0x10+1, -0x4);
    CPU6502 cpu(mem);
    std::cout << cpu.Addr_REL() << std::endl;
    std::cout << "0x" << std::hex << (unsigned) cpu.to_bcd(0) << std::dec << std::endl;
    std::cout << "0x" << std::hex << (unsigned) cpu.to_bcd(15) << std::dec << std::endl;
    std::cout << "0x" << std::hex << (unsigned) cpu.to_bcd(49) << std::dec << std::endl;
    std::cout << "0x" << std::hex << (unsigned) cpu.to_bcd(99) << std::dec << std::endl;
    std::cout <<  (unsigned) cpu.from_bcd(0x0) << std::endl;
    std::cout <<  (unsigned) cpu.from_bcd(0x15) << std::endl;
    std::cout <<  (unsigned) cpu.from_bcd(0x49) << std::endl;
    std::cout <<  (unsigned) cpu.from_bcd(0x99) << std::endl;
    return 0;
}
#include "cpu_6502.h"

CPU6502::CPU6502(std::shared_ptr<Memory> mem)
        : mem{mem},
          A{0}, X{0}, Y{0}, P{0x20}, S{0xff},
          PC{0x600}, offset(0) {
    mode_funcs["ACC"] = bind_mode(&CPU6502::Addr_ACC);
    mode_funcs["IMM"] = bind_mode(&CPU6502::Addr_IMM);
    mode_funcs["ABS"] = bind_mode(&CPU6502::Addr_ABS);
    mode_funcs["ZER"] = bind_mode(&CPU6502::Addr_ZER);
    mode_funcs["ZEX"] = bind_mode(&CPU6502::Addr_ZEX);
    mode_funcs["ZEY"] = bind_mode(&CPU6502::Addr_ZEY);
    mode_funcs["ABX"] = bind_mode(&CPU6502::Addr_ABX);
    mode_funcs["ABY"] = bind_mode(&CPU6502::Addr_ABY);
    mode_funcs["IMP"] = bind_mode(&CPU6502::Addr_IMP);
    mode_funcs["REL"] = bind_mode(&CPU6502::Addr_REL);
    mode_funcs["INX"] = bind_mode(&CPU6502::Addr_INX);
    mode_funcs["INY"] = bind_mode(&CPU6502::Addr_INY);
    mode_funcs["ABI"] = bind_mode(&CPU6502::Addr_ABI);

    // TODO: See which of these bind_ops can be replaced
    instr_funcs["ADC"] = bind_op(&CPU6502::Op_ADC);
    instr_funcs["AND"] = bit_op(std::bit_and<uint8_t>());
    instr_funcs["ASL"] = bind_op(&CPU6502::Op_ASL);
    instr_funcs["BCC"] = branch_op(CARRY, false);
    instr_funcs["BCS"] = branch_op(CARRY);
    instr_funcs["BEQ"] = branch_op(ZERO);
    instr_funcs["BIT"] = bind_op(&CPU6502::Op_BIT);
    instr_funcs["BMI"] = branch_op(NEGATIVE);
    instr_funcs["BNE"] = branch_op(ZERO, false);
    instr_funcs["BPL"] = branch_op(NEGATIVE, false);
    instr_funcs["BRK"] = bind_op(&CPU6502::Op_BRK);
    instr_funcs["BVC"] = branch_op(OVERFLOW, false);
    instr_funcs["BVS"] = branch_op(OVERFLOW);
    instr_funcs["CLC"] = set_op(CARRY, false);
    instr_funcs["CLD"] = set_op(DECIMAL, false);
    instr_funcs["CLI"] = set_op(INTERRUPT, false);
    instr_funcs["CLV"] = set_op(OVERFLOW, false);
    instr_funcs["CMP"] = compare_op(A);
    instr_funcs["CPX"] = compare_op(X);
    instr_funcs["CPY"] = compare_op(Y);
    instr_funcs["DEC"] = step_op(true);
    instr_funcs["DEX"] = step_reg_op(X, true);
    instr_funcs["DEY"] = step_reg_op(Y, true);
    instr_funcs["EOR"] = bit_op(std::bit_xor<uint8_t>());
    instr_funcs["INC"] = step_op();
    instr_funcs["INX"] = step_reg_op(X);
    instr_funcs["INY"] = step_reg_op(Y);
    instr_funcs["JMP"] = bind_op(&CPU6502::Op_JMP); // TODO: See if these two can be generated
    instr_funcs["JSR"] = bind_op(&CPU6502::Op_JSR);
    instr_funcs["LDA"] = load_op(A);
    instr_funcs["LDX"] = load_op(X);
    instr_funcs["LDY"] = load_op(Y);
    instr_funcs["LSR"] = bind_op(&CPU6502::Op_LSR);
    instr_funcs["NOP"] = [](uint8_t&) {};
    instr_funcs["ORA"] = bit_op(std::bit_or<uint8_t>());
    instr_funcs["PHA"] = push_op(A);
    instr_funcs["PHP"] = push_op(P);
    instr_funcs["PLA"] = pop_op(A);
    instr_funcs["PLP"] = pop_op(P);
    instr_funcs["ROL"] = bind_op(&CPU6502::Op_ROL); // TODO: Maybe these too
    instr_funcs["ROR"] = bind_op(&CPU6502::Op_ROR);
    instr_funcs["RTI"] = bind_op(&CPU6502::Op_RTI);
    instr_funcs["RTS"] = bind_op(&CPU6502::Op_RTS);
    instr_funcs["SBC"] = bind_op(&CPU6502::Op_SBC);
    instr_funcs["SEC"] = set_op(CARRY);
    instr_funcs["SED"] = set_op(DECIMAL);
    instr_funcs["SEI"] = set_op(INTERRUPT);
    instr_funcs["STA"] = store_op(A);
    instr_funcs["STX"] = store_op(X);
    instr_funcs["STY"] = store_op(Y);
    instr_funcs["TAX"] = transfer_op(A, X);
    instr_funcs["TAY"] = transfer_op(A, Y);
    instr_funcs["TSX"] = transfer_op(S, X);
    instr_funcs["TXA"] = transfer_op(X, A);
    instr_funcs["TXS"] = transfer_op(X, S);
    instr_funcs["TYA"] = transfer_op(Y, A);
}

std::function<void(uint8_t&)> CPU6502::bit_op(std::function<uint8_t(uint8_t,uint8_t)> f) {
    return [this, f](uint8_t &data) {
        A = f(A, data);
        set_flag(ZERO, A == 0);
        set_flag(NEGATIVE, A & 0x80);
    };
}

std::function<void(uint8_t&)> CPU6502::branch_op(uint8_t flag, bool value) {
    return [this, flag, value](uint8_t &data) {
        unsigned char curr = get_flag(flag);
        if ((!value && !curr) || (value && curr)) {
            PC = (uint16_t&) data + offset;
        }
    };
}

std::function<void(uint8_t&)> CPU6502::set_op(uint8_t flag, bool value) {
    return [this, flag, value](uint8_t &data) {
        set_flag(flag, value);
    };
}

std::function<void(uint8_t&)> CPU6502::compare_op(uint8_t &reg) {
    return [this, &reg](uint8_t &data) {
        int temp = reg - data;
        set_flag(NEGATIVE, temp & 0x80);
        set_flag(ZERO, temp == 0);
        set_flag(CARRY, temp > 0xff);
    };
}

std::function<void(uint8_t&)> CPU6502::step_op(bool decrement) {
    return [this, decrement](uint8_t &data) {
        if (decrement) {
            data--;
        }
        else {
            data++;
        }
        set_flag(NEGATIVE, data & 0x80);
        set_flag(ZERO, data == 0);
    };
}

std::function<void(uint8_t&)> CPU6502::step_reg_op(uint8_t &reg, bool decrement) {
    return [this, &reg, decrement](uint8_t&) { step_op(decrement)(reg); };
}

std::function<void(uint8_t&)> CPU6502::load_op(uint8_t &reg) {
    return [this, &reg](uint8_t &data) {
        reg = data;
        set_flag(NEGATIVE, reg & 0x80);
        set_flag(ZERO, reg == 0);
    };
}

std::function<void(uint8_t&)> CPU6502::store_op(const uint8_t &reg) {
    return [&reg](uint8_t &data) { data = reg; };
}

std::function<void(uint8_t&)> CPU6502::push_op(const uint8_t &reg) {
    return [this, &reg](uint8_t &data) { stack_push(reg); };
}

std::function<void(uint8_t&)> CPU6502::pop_op(uint8_t &reg) {
    return [this, &reg](uint8_t &data) { reg = stack_pop(); };
}

std::function<void(uint8_t&)> CPU6502::transfer_op(const uint8_t &reg_a, uint8_t &reg_b) {
    return [this, &reg_a, &reg_b](uint8_t &data) {
        reg_b = reg_a;
        set_flag(NEGATIVE, reg_b & 0x80);
        set_flag(ZERO, reg_b == 0);
    };
}

/*
 * Addressing modes
 * TODO: Document reference behavior
 */
uint8_t &CPU6502::Addr_ACC() { return A; } // Not implemented
uint8_t &CPU6502::Addr_IMM() { return mem->ref_byte(++PC); }
uint8_t &CPU6502::Addr_ABS() { offset = 0; int temp = PC; PC += 2; return mem->ref_byte(temp+1); }
uint8_t &CPU6502::Addr_ZER() { return mem->ref_byte(mem->read_byte(++PC)); }
uint8_t &CPU6502::Addr_ZEX() { return mem->ref_byte(mem->read_byte(++PC) + X); } // TODO Fix looping around
uint8_t &CPU6502::Addr_ZEY() { return mem->ref_byte(mem->read_byte(++PC) + Y); } // TODO Fix looping around
uint8_t &CPU6502::Addr_ABX() { int temp = PC; PC += 2; return mem->ref_byte(mem->read_word(temp+1) + X); } // TODO Fix looping around
uint8_t &CPU6502::Addr_ABY() { int temp = PC; PC += 2; return mem->ref_byte(mem->read_word(temp+1) + Y); } // TODO Fix looping around
uint8_t &CPU6502::Addr_IMP() { return mem->ref_byte(0); } // Dummy implementation by DavidBuchanan314
uint8_t &CPU6502::Addr_REL() { int temp = PC; PC++; offset=(int8_t) mem->read_byte(temp+1); return (uint8_t&) PC; }
uint8_t &CPU6502::Addr_INX() { return mem->ref_byte(mem->read_word(mem->read_byte(++PC) + (int8_t) X)); } // TODO Fix looping around
uint8_t &CPU6502::Addr_INY() { return mem->ref_byte(mem->read_word(mem->read_byte(++PC)) + (int8_t) Y); } // TODO Fix looping around
uint8_t &CPU6502::Addr_ABI() { offset=0; int temp = PC; PC += 2; return mem->ref_byte(mem->read_word(temp+1)); } // TODO: Check this; should it just return the indirect?

void CPU6502::Op_ADC(uint8_t &data) {
    // TODO: Fix various behaviors
    // TODO: Implement overflow
    // http://www.6502.org/tutorials/decimal_mode.html
    unsigned int sum = A + data + get_flag(CARRY);
    set_flag(OVERFLOW, sum>0xFF);
    if (get_flag(DECIMAL)) {
        sum = from_bcd(A) + from_bcd(data) + get_flag(CARRY);
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

void CPU6502::Op_ASL(uint8_t &data) {
    set_flag(CARRY, data & 0x80);
    data = (data << 1) & 0xFE;
    set_flag(NEGATIVE, data & 0x80);
    set_flag(ZERO, data == 0);
}

void CPU6502::Op_BIT(uint8_t &data) {
    P &= (data & 0xC0) | 0x3F;
    set_flag(ZERO, (data & A) == 0);
}

void CPU6502::Op_BRK(uint8_t &data) {
    set_flag(INTERRUPT, 1);
    stack_push_word(PC+2);
    stack_push(P | BREAK);
    set_flag(INTERRUPT, true);
}

void CPU6502::Op_JMP(uint8_t &data) {
    PC = ((uint16_t&) data+offset-1); // TODO: ??
}

void CPU6502::Op_JSR(uint8_t &data) {
    stack_push_word(PC);
    PC = ((uint16_t&) data+offset-1); // TODO: ??
}

void CPU6502::Op_LSR(uint8_t &data) {
    set_flag(CARRY, data & 0x1);
    data = (data >> 1) & 0x7F;
    set_flag(NEGATIVE, 0);
    set_flag(ZERO, data == 0);
}

void CPU6502::Op_ROL(uint8_t &data) {
    unsigned char temp = get_flag(CARRY);
    set_flag(CARRY, data & 0x80);
    data = (data << 1);
    data = (temp) ? (data | 0x1) : (data & 0xFE);
    set_flag(NEGATIVE, data & 0x80);
    set_flag(ZERO, data == 0);
}

void CPU6502::Op_ROR(uint8_t &data) {
    unsigned char temp = get_flag(CARRY);
    set_flag(CARRY, data & 0x1);
    data = (data >> 1);
    data = (temp) ? (data | 0x80) : (data & 0x7F);
    set_flag(NEGATIVE, data & 0x80);
    set_flag(ZERO, data == 0);
}

void CPU6502::Op_RTI(uint8_t &data) {
    P = stack_pop();
    PC = stack_pop_word();
}

void CPU6502::Op_RTS(uint8_t &data) {
    PC = stack_pop_word();
}

void CPU6502::Op_SBC(uint8_t &data) {
    // TODO: Fix various behaviors
    // TODO: Implement overflow
    // http://www.6502.org/tutorials/decimal_mode.html
    unsigned int diff = A - data - get_flag(CARRY);
    set_flag(OVERFLOW, diff>0xFF);
    if (get_flag(DECIMAL)) {
        diff = from_bcd(A) - from_bcd(data) - get_flag(CARRY);
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

#include <iostream> // TODO: Remove
int CPU6502::step() {
    uint8_t opcode = mem->read_byte(PC);
    InstrInfo info = Instructions::instr_map.at(opcode);
    execute(info);
    PC++;

    // TODO: Something with cycles
    return info.cycles;
}

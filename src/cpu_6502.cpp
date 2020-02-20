#include "cpu_6502.h"
#include "memory.h"

CPU6502::CPU6502(std::shared_ptr<Memory> mem)
        : mem{std::move(mem)},
          A{0}, X{0}, Y{0}, P{0}, S{0},
          PC{0x600} {
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
    instr_funcs["NOP"] = [](uint16_t&) {};
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

std::function<void(uint16_t&)> CPU6502::bit_op(std::function<uint8_t(uint8_t,uint8_t)> f) {
    return [this, f](uint16_t &data) {
        data = f(A, data);
        set_flag(ZERO, data == 0);
        set_flag(NEGATIVE, data & 0x80);
    };
}

std::function<void(uint16_t&)> CPU6502::branch_op(uint8_t &flag, bool value) {
    return [this, &flag, value](uint16_t &data) {
        unsigned char curr = get_flag(flag);
        if ((!value && curr) || (value && !curr)) {
            PC = data;
        }
    };
}

std::function<void(uint16_t&)> CPU6502::set_op(uint8_t &flag, bool value) {
    return [this, &flag, value](uint16_t &data) {
        set_flag(flag, value);
    };
}

std::function<void(uint16_t&)> CPU6502::compare_op(uint8_t &var) {
    return [this, var](uint16_t &data) {
        // TODO
    };
}

std::function<void(uint16_t&)> CPU6502::step_op(bool decrement) {
    return [this, decrement](uint16_t &data) {
        if (decrement) {
            data--;
        }
        else {
            data++;
        }
        set_flag(NEGATIVE, data & 0x80);
        set_flag(ZERO, data == 0);
    };

std::function<void(uint16_t&)> CPU6502::step_reg_op(uint8_t &reg, bool decrement) {
    return [this, reg, decrement](uint16_t&) { step_op(decrement)((uint16_t&) reg); };
}

std::function<void(uint16_t&)> CPU6502::load_op(uint8_t &reg) {
    return [this, &reg](uint16_t &data) {
        reg = data;
        set_flag(NEGATIVE, reg & 0x80);
        set_flag(ZERO, reg == 0);
    }
}

std::function<void(uint16_t&)> CPU6502::store_op(uint8_t reg) {
    return [this, &reg](uint16_t &data) { data = reg; }
}

std::function<void(uint16_t&)> CPU6502::push_op(uint8_t reg) {
    return [this, &reg](uint16_t &data) { stack_push(reg); }
}

std::function<void(uint16_t&)> CPU6502::pop_op(uint8_t &reg) {
    return [this, &reg](uint16_t &data) { reg = stack_pop(); }
}

std::function<void(uint16_t&)> CPU6502::transfer_op(uint8_t reg_a, uint8_t &reg_b) {
    return [this, &reg](uint16_t &data) {
        reg_b = reg_a;
        set_flag(NEGATIVE, reg_b & 0x80);
        set_flag(ZERO, reg_b == 0);
    }
}

/*
 * Addressing modes
 * Return the address an instruction will use to reference memory
 * Does NOT return the VALUE at that memory (hence IMM returns PC+1)
 */
uint16_t CPU6502::Addr_ACC() { return 0; } // Not implemented
uint16_t CPU6502::Addr_IMM() { return PC++; }
uint16_t CPU6502::Addr_ABS() { int temp = PC; PC += 2; return mem->read_word(temp+1); }
uint16_t CPU6502::Addr_ZER() { return mem->read_byte(PC++); }
uint16_t CPU6502::Addr_ZEX() { return mem->read_byte(PC++) + (int8_t) X; } // TODO Fix looping around
uint16_t CPU6502::Addr_ZEY() { return mem->read_byte(PC++) + (int8_t) Y; } // TODO Fix looping around
uint16_t CPU6502::Addr_ABX() { int temp = PC; PC += 2; return mem->read_word(temp+1) + (int8_t) X; } // TODO Fix looping around
uint16_t CPU6502::Addr_ABY() { int temp = PC; PC += 2; return mem->read_word(temp+1) + (int8_t) Y; } // TODO Fix looping around
uint16_t CPU6502::Addr_IMP() { return 0; }
uint16_t CPU6502::Addr_REL() { int temp = PC; PC++; return temp + mem->read_byte(temp+1); }
uint16_t CPU6502::Addr_INX() { int temp = PC; PC += 2; return mem->read_word(mem->read_word(temp+1) + (int8_t) X); } // TODO Fix looping around
uint16_t CPU6502::Addr_INY() { int temp = PC; PC += 2; return mem->read_word(mem->read_word(temp+1)) + (int8_t) Y; } // TODO Fix looping around
uint16_t CPU6502::Addr_ABI() { int temp = PC; PC += 2; return mem->read_word(temp+1); } // TODO: Check this; should it just return the indirect?

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

void CPU6502::Op_ASL_A(uint16_t str) {
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

void CPU6502::Op_LSR_A(uint16_t src) {
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

#include <iostream> // TODO: Remove
int CPU6502::step() {
    uint8_t opcode = mem->read_byte(PC);
    // std::cout << std::hex << (int) PC << " " << (int) opcode << '\n';
    std::cout << std::hex << (int) opcode << " ";
    // if (opcode == 0x00) return 0; // TEMP
    const Instr instr = instr_table[opcode];
    execute_instr(instr);
    PC++;

    // TODO: Something with cycles
    return instr.cycles;
}

#include "disassembler.h"
int main(int argc, char **argv) {
    std::shared_ptr<Memory> mem = std::make_shared<Memory>(Memory());
    // mem->write_byte(0x10+1, -0x4);
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

    Disassembler dis(0x600);
    // std::cout << dis.instr_to_string(dis.instr_table[0x29], 0x1010) << std::endl << std::endl;

    std::ifstream file(argv[1], std::ifstream::binary);
    if (!file) {
        std::cerr << "Could not open file " << argv[1] << '\n';
        std::cout << "Usage: " << argv[0] << " <binary file>\n";
        return 1;
    }
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    mem->load_file(file, 0, length, 0x600);
    mem->print();
    file.clear();
    file.seekg(0, file.beg);
    dis.file_to_strings(file);
    for (int i = 0; i < 100; i++) cpu.step();
    /* for (auto s : dis.instructions) {
        std::cout << s << std::endl;
    } */

    return 0;
}

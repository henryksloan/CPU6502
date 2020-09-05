#include "cpu_6502.h"

CPU6502::CPU6502(std::shared_ptr<Memory> mem)
        : mem{mem} {
    reset();

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

    // https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes
    // TODO: Several of these could be implemented as combined_op(vector<string>)
    instr_funcs["LAX"] = bind_op(&CPU6502::Op_LAX);
    instr_funcs["SAX"] = bind_op(&CPU6502::Op_SAX);
    instr_funcs["DCP"] = bind_op(&CPU6502::Op_DCP);
    instr_funcs["ISC"] = bind_op(&CPU6502::Op_ISC);
    instr_funcs["SLO"] = bind_op(&CPU6502::Op_SLO);
    instr_funcs["RLA"] = bind_op(&CPU6502::Op_RLA);
    instr_funcs["SRE"] = bind_op(&CPU6502::Op_SRE);
    instr_funcs["RRA"] = bind_op(&CPU6502::Op_RRA);
}

void CPU6502::step() {
    if (cycles_left > 0) {
        cycles_left--;
    }
    else {
        uint8_t opcode = mem->read_byte(PC);
        // std::cout << std::hex << "0x" << (int) PC << ": 0x" << (int) opcode << std::endl;
        std::cout << std::hex << "0x" << (int) PC << ": 0x" << (int) opcode
            << " A:" << std::hex << std::setfill('0') << std::setw(2) << (int) A
            << " X:" << std::hex << std::setfill('0') << std::setw(2) << (int) X
            << " Y:" << std::hex << std::setfill('0') << std::setw(2) << (int) Y
            << " P:" << std::hex << std::setfill('0') << std::setw(2) << (int) P
            << " SP:" << std::hex << std::setfill('0') << std::setw(2) << (int) S
            << std::endl;
        // InstrInfo info = Instructions::instr_map.at(opcode);
        InstrInfo info = Instructions::get_info(opcode);

        execute(info);
        PC++;
        cycles_left = info.cycles; // TODO: Account for extra cycles
    }
}

void CPU6502::reset() {
    A = 0x00;
    X = 0x00;
    Y = 0x00;
    P = CONSTANT | INTERRUPT;
    S = 0xfd;

    PC = mem->read_word(RST_VEC);
    offset = 0;
    jump = 0;

    cycles_left = 0;
}

void CPU6502::nmi() {
    stack_push_word(PC+2);
    stack_push(P);
    set_flag(INTERRUPT, 1);
    PC = mem->read_word(NMI_VEC);
}

void CPU6502::irq() {
    if (get_flag(INTERRUPT)) return;

    stack_push_word(PC+2);
    stack_push(P);
    set_flag(INTERRUPT, 1);
    PC = mem->read_word(IRQ_VEC);
}

// Returns the result of a binary logic operation (e.g. AND) between A and memory
std::function<void(uint8_t&)> CPU6502::bit_op(std::function<uint8_t(uint8_t,uint8_t)> f) {
    return [this, f](uint8_t &data) {
        A = f(A, data);
        set_flag(ZERO, A == 0);
        set_flag(NEGATIVE, A & 0x80);
    };
}

// Branch if value
std::function<void(uint8_t&)> CPU6502::branch_op(uint8_t flag, bool value) {
    return [this, flag, value](uint8_t &data) {
        unsigned char curr = get_flag(flag);
        if ((!value && !curr) || (value && curr)) {
            PC = (uint16_t&) data + offset;
        }
    };
}

// Set a flag to a predefined value
std::function<void(uint8_t&)> CPU6502::set_op(uint8_t flag, bool value) {
    return [this, flag, value](uint8_t &data) {
        set_flag(flag, value);
    };
}

// Compare a register to memory, then set flags
std::function<void(uint8_t&)> CPU6502::compare_op(uint8_t &reg) {
    return [this, &reg](uint8_t &data) {
        int temp = reg - data;
        set_flag(NEGATIVE, temp & 0x80);
        set_flag(ZERO, temp == 0);
        set_flag(CARRY, temp >= 0x0);
    };
}

// Either increment or decrement data
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

// Increment or decrement a register
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
    return [this, &reg](uint8_t &data) {
        std::cout << "YAYA\n";
        data = reg;
        mem->ref_callback(data);
    };
}

std::function<void(uint8_t&)> CPU6502::push_op(const uint8_t &reg) {
    return [this, &reg](uint8_t &data) { stack_push(reg); };
}

std::function<void(uint8_t&)> CPU6502::pop_op(uint8_t &reg) {
    return [this, &reg](uint8_t &data) {
        auto temp = P;
        reg = stack_pop();
        if (temp == P) {
            set_flag(NEGATIVE, A & 0x80);
            set_flag(ZERO, A == 0);
        }
    };
}

std::function<void(uint8_t&)> CPU6502::transfer_op(const uint8_t &reg_a, uint8_t &reg_b) {
    return [this, &reg_a, &reg_b](uint8_t &data) {
        auto temp = S;
        reg_b = reg_a;
        if (temp == S) {
            set_flag(NEGATIVE, A & 0x80);
            set_flag(ZERO, A == 0);
        }
    };
}

uint8_t &CPU6502::Addr_ACC() { return A; }
uint8_t &CPU6502::Addr_IMM() { return mem->ref_byte(++PC); }
// uint8_t &CPU6502::Addr_ABS() { offset = 0; int temp = PC; PC += 2; return mem->ref_byte(temp+1); }
uint8_t &CPU6502::Addr_ABS() { std::cout << "Got " << std::setfill('0') << std::setw(4) << (int) mem->read_word(PC + 1) << std::endl; jump = mem->read_word(PC+1)-1; offset = 0; int temp = PC; PC += 2; return mem->ref_byte(mem->read_word(temp+1)); }
uint8_t &CPU6502::Addr_ZER() { return mem->ref_byte(mem->read_byte(++PC)); }
uint8_t &CPU6502::Addr_ZEX() { return mem->ref_byte((mem->read_byte(++PC) + X) & 0xFF); }
uint8_t &CPU6502::Addr_ZEY() { return mem->ref_byte((mem->read_byte(++PC) + Y) & 0xFF); }
uint8_t &CPU6502::Addr_ABX() { int temp = PC; PC += 2; return mem->ref_byte(mem->read_word(temp+1) + X); }
uint8_t &CPU6502::Addr_ABY() { int temp = PC; PC += 2; return mem->ref_byte(mem->read_word(temp+1) + Y); }
uint8_t &CPU6502::Addr_IMP() { return mem->ref_byte(0); } // Dummy implementation
uint8_t &CPU6502::Addr_REL() { int temp = PC; PC++; offset=(int8_t) mem->read_byte(temp+1); return (uint8_t&) PC; }
uint8_t &CPU6502::Addr_INX() { 
    PC++;
    return mem->ref_byte(mem->read_byte((mem->read_byte(PC) + (int8_t) X) & 0xFF) | (mem->read_byte((mem->read_byte(PC) + (int8_t) X + 1) & 0xFF) << 8));
}
uint8_t &CPU6502::Addr_INY() {
    PC++;
    return mem->ref_byte((mem->read_byte(mem->read_byte(PC)) | (mem->read_byte((mem->read_byte(PC) + 1) & 0xFF) << 8)) + (int16_t) Y);
}
uint8_t &CPU6502::Addr_ABI() {
    uint16_t lo = mem->read_byte(++PC);
    uint16_t hi = mem->read_byte(++PC);
    uint16_t ptr = (hi << 8) | lo;

    if (lo == 0x00FF) {
        jump = ((mem->read_byte(ptr & 0xFF00) << 8) | mem->read_byte(ptr)) - 1;
    } else {
        jump = ((mem->read_byte(ptr + 1) << 8) | mem->read_byte(ptr)) - 1;
    }

    return mem->ref_byte(0);
}

// Add memory to accumulator with carry
void CPU6502::Op_ADC(uint8_t &data) {
    unsigned int sum;
    // if (get_flag(DECIMAL)) {
    if (false && get_flag(DECIMAL)) {
        sum = from_bcd(A) + from_bcd(data) + get_flag(CARRY);
        set_flag(CARRY, sum > 99);
        sum = to_bcd(sum % 100);
    } else {
        sum = (uint16_t) A + (uint16_t) data + (uint16_t) get_flag(CARRY);
        set_flag(CARRY, sum > 0xFF);
    }
    set_flag(OVERFLOW, ((A^sum)&(data^sum)&0x80) != 0);
    A = sum & 0xFF;
    set_flag(ZERO, A == 0); // sum == 0);
    set_flag(NEGATIVE, sum & 0x80);
}

// Arithmetic shift left, with carry
// C <- [76543210] <- 0
void CPU6502::Op_ASL(uint8_t &data) {
    set_flag(CARRY, data & 0x80);
    data = (data << 1) & 0xFE;
    mem->ref_callback(data);
    set_flag(NEGATIVE, data & 0x80);
    set_flag(ZERO, data == 0);
}

// Test bits in memory with accumulator
// 2 most significant bits are transferred from data to P [Flags N and V]
// Then Flag Z is set according to data & A
void CPU6502::Op_BIT(uint8_t &data) {
    P |= 0xC0;
    P &= (data & 0xC0) | 0x3F;
    set_flag(ZERO, (data & A) == 0);
}

// Force a system interrupt
void CPU6502::Op_BRK(uint8_t &data) {
    stack_push_word(PC+2);
    stack_push(P | BREAK);
    set_flag(INTERRUPT, 1);
    PC = mem->read_word(IRQ_VEC);
}

// Jump PC to a given address
void CPU6502::Op_JMP(uint8_t &data) {
    // PC = ((uint16_t&) data+offset-1);
    PC = jump;
}

// Jump PC to a given address, storing the return address
void CPU6502::Op_JSR(uint8_t &data) {
    stack_push_word(PC);
    // PC = ((uint16_t&) data+offset-1);
    PC = jump;
}


// Shift right with carry
// 0 -> [76543210] -> C
void CPU6502::Op_LSR(uint8_t &data) {
    set_flag(CARRY, data & 0x1);
    data = (data >> 1) & 0x7F;
    mem->ref_callback(data);
    set_flag(NEGATIVE, 0);
    set_flag(ZERO, data == 0);
}

// Rotate left
// C <- [76543210] <- C
void CPU6502::Op_ROL(uint8_t &data) {
    unsigned char temp = get_flag(CARRY);
    set_flag(CARRY, data & 0x80);
    data = (data << 1);
    data = (temp) ? (data | 0x1) : (data & 0xFE);
    mem->ref_callback(data);
    set_flag(NEGATIVE, data & 0x80);
    set_flag(ZERO, data == 0);
}

// Rotate right
// C -> [76543210] -> C
void CPU6502::Op_ROR(uint8_t &data) {
    unsigned char temp = get_flag(CARRY);
    set_flag(CARRY, data & 0x1);
    data = (data >> 1);
    data = (temp) ? (data | 0x80) : (data & 0x7F);
    mem->ref_callback(data);
    set_flag(NEGATIVE, data & 0x80);
    set_flag(ZERO, data == 0);
}

// Return from interrupt
void CPU6502::Op_RTI(uint8_t &data) {
    P = stack_pop();
    PC = stack_pop_word()-1; // TODO: Check this subtract
}

// Return from subroutine
void CPU6502::Op_RTS(uint8_t &data) {
    PC = stack_pop_word();
}

// Add memory to accumulator with carry
void CPU6502::Op_SBC(uint8_t &data) {
    unsigned int diff;
    // if (get_flag(DECIMAL)) {
    if (false && get_flag(DECIMAL)) {
        diff = from_bcd(A) - from_bcd(data) - 1 + get_flag(CARRY);
        diff = to_bcd(diff % 100);
    }
    else {
        diff = A - data - 1 + get_flag(CARRY);
    }
    set_flag(OVERFLOW, ((A^diff)&(A^data)&0x80) != 0);
    A = diff & 0xFF;
    set_flag(CARRY, diff < 0x100);
    set_flag(NEGATIVE, A & 0x80);
    set_flag(ZERO, A == 0);
}

// UNOFFICIAL: Load A and X with data
void CPU6502::Op_LAX(uint8_t &data) {
    A = data;
    X = data;
    set_flag(NEGATIVE, A & 0x80);
    set_flag(ZERO, X == 0);
}

// UNOFFICIAL: Store (A & X) in memory
void CPU6502::Op_SAX(uint8_t &data) {
    data = A & X;
}

// UNOFFICIAL: Decrement data and then compare with A
void CPU6502::Op_DCP(uint8_t &data) {
    instr_funcs["DEC"](data);
    instr_funcs["CMP"](data);
}

// UNOFFICIAL: Increment data and then set A to (A - data)
void CPU6502::Op_ISC(uint8_t &data) {
    instr_funcs["INC"](data);
    instr_funcs["SBC"](data);
}

// UNOFFICIAL: Arithmetic shift data left by 1 bit and then OR with A
void CPU6502::Op_SLO(uint8_t &data) {
    instr_funcs["ASL"](data);
    instr_funcs["ORA"](data);
}

// UNOFFICIAL: Rotate data left by 1 bit and then AND with A
void CPU6502::Op_RLA(uint8_t &data) {
    instr_funcs["ROL"](data);
    instr_funcs["AND"](data);
}

// UNOFFICIAL: Shift data right by 1 bit and then EOR with A
void CPU6502::Op_SRE(uint8_t &data) {
    instr_funcs["LSR"](data);
    instr_funcs["EOR"](data);
}

// UNOFFICIAL: Rotate data right by 1 bit and then set A to (A + data)
void CPU6502::Op_RRA(uint8_t &data) {
    instr_funcs["ROR"](data);
    instr_funcs["ADC"](data);
}

void CPU6502::execute(InstrInfo info) {
    // TODO: Do something with cycles
    uint8_t &data = mode_funcs[info.mode_str]();
    std::cout << info.mode_str << " " << ((uint16_t&) data) << std::endl;
    auto instr_func = instr_funcs.find(info.op_str);
    if (instr_func != instr_funcs.end()) {
        instr_func->second(data);
    }
}

unsigned char CPU6502::get_flag(uint8_t mask) {
    return (P & mask) ? 1 : 0;
}

void CPU6502::set_flag(uint8_t mask, unsigned char val) {
    P = (val) ? (P | mask) : (P & ~mask);
}

// Translates a binary integer to a "Binary Coded Decimal"
// i.e. decimal(49) => 0x49
uint8_t CPU6502::to_bcd(uint8_t x) {
    if (x > 99) throw std::invalid_argument("Invalid BCD");

    return (x%10) + ((x/10) << 4);
}

// Translates "Binary Coded Decimal" to a binary integer
// i.e. 0x49 => decimal(49)
uint8_t CPU6502::from_bcd(uint8_t x) {
    if (x > 0x99) throw std::invalid_argument("Invalid BCD");

    return 10*((x & 0xF0) >> 4) + (x&0x0F);
}

void CPU6502::stack_push(uint8_t data) {
    mem->write_byte(0x100+S, data);
    if (S == 0x00) S = 0xFF;
    else S--;
}

void CPU6502::stack_push_word(uint16_t data) {
    stack_push((data >> 8) & 0xFF);
    stack_push(data & 0xFF);
}

uint8_t CPU6502::stack_pop() {
    if (S == 0xFF) S = 0x00;
    else S++;
    uint8_t temp = mem->read_byte(0x100+S);
    return temp;
}

uint16_t CPU6502::stack_pop_word() {
    return stack_pop() | (stack_pop() << 8);
}

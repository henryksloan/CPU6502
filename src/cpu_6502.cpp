#include "cpu_6502.h"

CPU6502::CPU6502(std::shared_ptr<Memory> mem)
        : mem{std::move(mem)},
          A{0}, X{0}, Y{0}, P{0}, S{0xff},
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
        data = f(A, data);
        set_flag(ZERO, data == 0);
        set_flag(NEGATIVE, data & 0x80);
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

std::function<void(uint8_t&)> CPU6502::store_op(uint8_t &reg) {
    return [this, &reg](uint8_t &data) { data = reg; };
}

std::function<void(uint8_t&)> CPU6502::push_op(uint8_t &reg) {
    return [this, &reg](uint8_t &data) { stack_push(reg); };
}

std::function<void(uint8_t&)> CPU6502::pop_op(uint8_t &reg) {
    return [this, &reg](uint8_t &data) { reg = stack_pop(); };
}

std::function<void(uint8_t&)> CPU6502::transfer_op(uint8_t reg_a, uint8_t &reg_b) {
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
uint8_t &CPU6502::Addr_INX() { int temp = PC; PC += 2; return mem->ref_byte(mem->read_word(mem->read_byte(temp+1) + (int8_t) X)); } // TODO Fix looping around
uint8_t &CPU6502::Addr_INY() { int temp = PC; PC += 2; return mem->ref_byte(mem->read_word(mem->read_byte(temp+1)) + (int8_t) Y); } // TODO Fix looping around
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
    set_flag(ZERO, data & A);
}

void CPU6502::Op_BRK(uint8_t &data) {
    set_flag(INTERRUPT, 1);
    stack_push_word(PC+2);
    stack_push(P);
}

void CPU6502::Op_JMP(uint8_t &data) {
    PC = ((uint16_t&) data+offset); // TODO: ??
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
    // if (opcode == 0x00) return 0; // TEMP
    InstrInfo info = Instructions::instr_map.at(opcode);
    execute(info);
    PC++;

    // TODO: Something with cycles
    return info.cycles;
}

<<<<<<< Updated upstream
#include "disassembler.h"
int main(int argc, char **argv) {
    std::shared_ptr<Memory> mem = std::make_shared<Memory>(Memory());
    // mem->write_byte(0x10+1, -0x4);
    CPU6502 cpu(mem);
    // std::cout << cpu.Addr_REL() << std::endl;
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
=======
// #include "disassembler.h"
// #include <ctime>
// #include <cstdlib>
// #include <chrono>
// #include <thread>
// int main(int argc, char **argv) {
//     std::shared_ptr<Memory> mem = std::make_shared<Memory>(Memory());
//     // mem->write_byte(0x10+1, -0x4);
//     CPU6502 cpu(mem);
//     // std::cout << cpu.Addr_REL() << std::endl;
//     std::cout << "0x" << std::hex << (unsigned) cpu.to_bcd(0) << std::dec << std::endl;
//     std::cout << "0x" << std::hex << (unsigned) cpu.to_bcd(15) << std::dec << std::endl;
//     std::cout << "0x" << std::hex << (unsigned) cpu.to_bcd(49) << std::dec << std::endl;
//     std::cout << "0x" << std::hex << (unsigned) cpu.to_bcd(99) << std::dec << std::endl;
//     std::cout <<  (unsigned) cpu.from_bcd(0x0) << std::endl;
//     std::cout <<  (unsigned) cpu.from_bcd(0x15) << std::endl;
//     std::cout <<  (unsigned) cpu.from_bcd(0x49) << std::endl;
//     std::cout <<  (unsigned) cpu.from_bcd(0x99) << std::endl;
// 
//     Disassembler dis(0x600);
//     // std::cout << dis.instr_to_string(dis.instr_table[0x29], 0x1010) << std::endl << std::endl;
// 
//     std::ifstream file(argv[1], std::ifstream::binary);
//     if (!file) {
//         std::cerr << "Could not open file " << argv[1] << '\n';
//         std::cout << "Usage: " << argv[0] << " <binary file>\n";
//         return 1;
//     }
//     file.seekg (0, file.end);
//     int length = file.tellg();
//     file.seekg (0, file.beg);
//     mem->load_file(file, 0, length-1, 0x600);
//     // mem->print();
//     file.clear();
//     file.seekg(0, file.beg);
//     dis.file_to_strings(file);
// 
//     // $0: Black
//     // $1: White
//     // $2: Red
//     // $3: Cyan
//     // $4: Purple
//     // $5: Green
//     // $6: Blue
//     // $7: Yellow
//     // $8: Orange
//     // $9: Brown
//     // $a: Light red
//     // $b: Dark grey
//     // $c: Grey
//     // $d: Light green
//     // $e: Light blue
//     // $f: Light grey
//     std::unordered_map<int, std::string> col {
//         {0x0, "40"},
//         {0x1, "1;47"},
//         {0x2, "41"},
//         {0x3, "46"},
//         {0x4, "45"},
//         {0x5, "42"},
//         {0x6, "44"},
//         {0x7, "43"},
//         {0x8, ""},
//         {0x9, ""},
//         {0xa, "1;41"},
//         {0xb, "1;40"},
//         {0xc, "47"},
//         {0xd, "1;42"},
//         {0xe, "1;44"},
//         {0xf, "47"}
//     };
//     // for (int i = 0x200; i < 0x600; i++) {
//     //     std::cout << "\033[" << col[mem->read_byte(i)] << "m*\033[0m";// << " ";
//     // }
//     for (int i = 0x200; i < 0x600; i += 0x20) {
//         for (int j = i; j < i+0x20; j++) {
//             std::cout << "\033[" << col[mem->read_byte(j)] << "m*\033[0m";// << " ";
//         }
//         std::cout << std::endl;
//     }
// 
//     std::srand(std::time(nullptr));
//     /*for (int i = 0; i < 10000; i++) {
//         std::clock_t c_start = std::clock();
//         auto t_start = std::chrono::high_resolution_clock::now();
//         mem->write_byte(0xfe, std::rand()%0x100);
//         cpu.step();
//         std::clock_t c_end = std::clock();
//         auto t_end = std::chrono::high_resolution_clock::now();
//         int a = (1000/60) - std::chrono::duration<double, std::milli>(t_end-t_start).count();
//         for (int i = 0x200; i < 0x600; i += 0x20) {
//             for (int j = i; j < i+0x20; j++) {
//                 std::cout << "\033[" << col[mem->read_byte(j)&0xF] << "m*\033[0m";// << " ";
//             }
//             std::cout << std::endl;
//         }
//         for (int i = 0x0; i < 0xff; i++) {
//             std::cout << std::hex << std::setfill('0') << std::setw(2) << (int) mem->read_byte(i);// << " ";
//         }
//         getchar();
//     }*/
// 
//     for (int i = 0; i < 100000; i++) {
//         std::cout << "\033[2J\033[1;1H";
//         std::clock_t c_start = std::clock();
//         auto t_start = std::chrono::high_resolution_clock::now();
//         mem->write_byte(0xfe, std::rand()%0x100);
//         if (i%100==0) {
//             switch (std::rand()%4) {
//                 case 0:
//                     mem->write_byte(0xff, 0x77);
//                     break;
//                 case 1:
//                     mem->write_byte(0xff, 0x64);
//                     break;
//                 case 2:
//                     mem->write_byte(0xff, 0x73);
//                     break;
//                 case 3:
//                     mem->write_byte(0xff, 0x61);
//                     break;
//             }
//         }
//         cpu.step();
//         for (int i = 0x200; i < 0x600; i += 0x20) {
//             for (int j = i; j < i+0x20; j++) {
//                 std::cout << "\033[" << col[mem->read_byte(j)&0xf] << "m*\033[0m";// << " ";
//             }
//             std::cout << std::endl;
//         }
//         std::clock_t c_end = std::clock();
//         auto t_end = std::chrono::high_resolution_clock::now();
//         int a = (1000/120) - std::chrono::duration<double, std::milli>(t_end-t_start).count();
//         std::this_thread::sleep_for(std::chrono::milliseconds((a < 0) ? 0 : a));
//         // std::this_thread::sleep_for(std::chrono::milliseconds(4));
//     }
// 
//     // mem->print();
//     std::cout << "\n";
//     for (int i = 0x200; i < 0x600; i += 0x20) {
//         for (int j = i; j < i+0x20; j++) {
//             std::cout << "\033[" << col[mem->read_byte(j)] << "m*\033[0m";// << " ";
//         }
//         std::cout << std::endl;
//     }
//     /* for (auto s : dis.instructions) {
//         std::cout << s << std::endl;
//     } */
//     // for (int i = 0x200; i < 0x600; i++) {
//     //     std::cout << std::hex << std::setfill('0') << std::setw(2) << (int) mem->read_byte(i);// << " ";
//     // }
//     // std::cout << std::endl;
// 
//     return 0;
// }
>>>>>>> Stashed changes

#ifndef CPU_6502
#define CPU_6502

#include <functional>
#include <memory>
#include <map>
#include <string>
#include <stdexcept>

#include "instruction.h"
#include "memory.h"

// Flag masks from github.com/gianlucag/mos6502
#define NEGATIVE  0x80
#define OVERFLOW  0x40
#define CONSTANT  0x20
#define BREAK     0x10
#define DECIMAL   0x08
#define INTERRUPT 0x04
#define ZERO      0x02
#define CARRY     0x01

class CPU6502 {
 public:
    CPU6502(std::shared_ptr<Memory> mem);

 // TODO: Uncomment
 // private:
    std::shared_ptr<Memory> mem;

    /*
     * 8-bit registers
     * A: Accumulator
     * X, Y: Index registers
     * P: Flag bits (NV-BDIZC)
     * S: Stack pointer
     */
    uint8_t A, X, Y, P, S;

    // 16-bit program counter
    uint16_t PC;
    uint16_t offset;

    uint8_t &Addr_ACC(); // Accumulator
    uint8_t &Addr_IMM(); // Immediate
    uint8_t &Addr_ABS(); // Absolute
    uint8_t &Addr_ZER(); // Zero page
    uint8_t &Addr_ZEX(); // Zero page, X-indexed
    uint8_t &Addr_ZEY(); // Zero page, Y-indexed
    uint8_t &Addr_ABX(); // Absolute, X-indexed
    uint8_t &Addr_ABY(); // Absolute, Y-indexed
    uint8_t &Addr_IMP(); // Implied
    uint8_t &Addr_REL(); // Relative
    uint8_t &Addr_INX(); // Indirect, X-indexed
    uint8_t &Addr_INY(); // Indirect, Y-indexed
    uint8_t &Addr_ABI(); // Absolute indirect

    void Op_ADC(uint8_t&);
    void Op_ASL(uint8_t&);
    void Op_BIT(uint8_t&);
    void Op_BRK(uint8_t&);
    void Op_JMP(uint8_t&);
    void Op_JSR(uint8_t&);
    void Op_LSR(uint8_t&);
    void Op_ROL(uint8_t&);
    void Op_ROR(uint8_t&);
    void Op_RTI(uint8_t&);
    void Op_RTS(uint8_t&);
    void Op_SBC(uint8_t&);

    std::map<std::string, std::function<void(uint8_t&)>> instr_funcs;
    std::map<std::string, std::function<uint8_t&(void)>> mode_funcs;

    inline std::function<void(uint8_t&)> bind_op(void (CPU6502::*op_f)(uint8_t&)) {
        return std::bind(op_f, this, std::placeholders::_1);
    }

    inline std::function<uint8_t&(void)> bind_mode(uint8_t &(CPU6502::*mode_f)(void)) {
        return std::bind(mode_f, this);
    }

    inline std::function<void(uint8_t&)> bit_op(std::function<uint8_t(uint8_t,uint8_t)> f);
    inline std::function<void(uint8_t&)> branch_op(uint8_t flag, bool value=true);
    inline std::function<void(uint8_t&)> set_op(uint8_t flag, bool value=true);
    inline std::function<void(uint8_t&)> compare_op(uint8_t &reg);
    inline std::function<void(uint8_t&)> step_op(bool decrement=false);
    inline std::function<void(uint8_t&)> step_reg_op(uint8_t &reg, bool decrement=false);
    inline std::function<void(uint8_t&)> load_op(uint8_t &reg);
    inline std::function<void(uint8_t&)> store_op(const uint8_t &reg);
    inline std::function<void(uint8_t&)> push_op(const uint8_t &reg);
    inline std::function<void(uint8_t&)> pop_op(uint8_t &reg);
    inline std::function<void(uint8_t&)> transfer_op(const uint8_t &reg_a, uint8_t &reg_b); // a -> b

    inline void execute(InstrInfo info) {
        // TODO: Do something with cycles
        int temp = PC; // TODO: Remove temp
        uint8_t &data = mode_funcs[info.mode_str]();
        instr_funcs[info.op_str](data);
        std::cout << (int)temp << " " << info.op_str << " " << info.mode_str << " " << (int) ((uint16_t&) data) << std::endl;;
        std::cout << "(A, X, Y, P, S, offset) = (" << std::hex << (int) A << ", " << (int) X << ", " << (int) Y << ", " << (int) P << ", " << (int) S << ", " << (int) offset << ")" << std::endl;
        // std::cout << '[' << std::hex;
        // for (int i = S; i <= 0xff; i++) std::cout << mem->read_byte(i) << ' ';
        // std::cout << ']' << std::endl;
        // std::cout << "S(0xff) " << mem->read_word(0xff) << std::endl;
    }

    int step();

    inline unsigned char get_flag(uint8_t mask) {
        return (P & mask) ? 1 : 0;
    }

    inline void set_flag(uint8_t mask, unsigned char val) {
        if (val) {
            P |= mask;
        }
        else {
            P &= ~mask;
        }
    }

    inline uint8_t to_bcd(uint8_t x) {
        if (x > 99) throw std::invalid_argument("Invalid BCD");

        return (x%10) + ((x/10) << 4);
    }

    inline uint8_t from_bcd(uint8_t x) {
        if (x > 0x99) throw std::invalid_argument("Invalid BCD");

        return 10*((x & 0xF0) >> 4) + (x&0x0F);
    }

    inline void stack_push(uint8_t data) {
        mem->write_byte(0x100+S, data);
        S--;
    }

    inline void stack_push_word(uint16_t data) {
        mem->write_word(0x100+S, data);
        S -= 2;
    }

    inline uint8_t stack_pop() {
        S++;
        uint8_t temp = mem->read_byte(0x100+S);
        return temp;
    }

    inline uint16_t stack_pop_word() {
        S += 2;
        uint16_t temp = mem->read_word(0x100+S);
        return temp;
    }
};

#endif // CPU_6502

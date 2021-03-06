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
// Single-bit masks (e.g. 0x40 = 01000000)
#define NEGATIVE  0x80
#define OVERFLOW  0x40
#define CONSTANT  0x20
#define BREAK     0x10
#define DECIMAL   0x08
#define INTERRUPT 0x04
#define ZERO      0x02
#define CARRY     0x01

#define NMI_VEC 0xFFFA
#define RST_VEC 0xFFFC
#define IRQ_VEC 0xFFFE

class CPU6502 {
 public:
    CPU6502(std::shared_ptr<Memory> mem);

    void step();
    void reset();
    void nmi();
    void irq();

 private:
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

    // Number of cycles remaining for current instruction
    int cycles_left;

    // Addressing modes return a reference to the appropriate data
    // Note that all of them return actual data, IMM returns the uint16_t at PC+1
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

    // Maps between the names of intructions and modes to their implementations
    std::map<std::string, std::function<void(uint8_t&)>> instr_funcs;
    std::map<std::string, std::function<uint8_t&(void)>> mode_funcs;

    /*
     * These functions abstract similar instructions
     */

    // Binds a function to the type of an instr_funcs
    inline std::function<void(uint8_t&)> bind_op(void (CPU6502::*op_f)(uint8_t&)) {
        return std::bind(op_f, this, std::placeholders::_1);
    }

    // Binds a mode to the type of mode_funcs
    inline std::function<uint8_t&(void)> bind_mode(uint8_t &(CPU6502::*mode_f)(void)) {
        return std::bind(mode_f, this);
    }

    // Various abstractions of similar instructions
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

    // More complex or unique instructions, to be used with bind_op
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

    void execute(InstrInfo info);

    unsigned char get_flag(uint8_t mask);
    void set_flag(uint8_t mask, unsigned char val);

    // Translates a binary integer to a "Binary Coded Decimal"
    // i.e. decimal(49) => 0x49
    uint8_t to_bcd(uint8_t x);

    // Translates "Binary Coded Decimal" to a binary integer
    // i.e. 0x49 => decimal(49)
    uint8_t from_bcd(uint8_t x);

    void stack_push(uint8_t data);
    void stack_push_word(uint16_t data);
    uint8_t stack_pop();
    uint16_t stack_pop_word();
};

#endif // CPU_6502

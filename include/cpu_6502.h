#ifndef CPU_6502
#define CPU_6502

#include <functional>
#include <memory>
#include <map>
#include <string>
#include <stdexcept>

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

    std::shared_ptr<Memory> mem;

    /*
     * Instr defines an instruction with cycle count, behavior and addressing mode
     * op_f is a function that accepts a src word
     * addr_f is an addressing mode, that returns a memory address
     */
    typedef struct Instr {
        Instr() {}
        Instr(CPU6502 *cpu,
              short cycles,
              void (CPU6502::*run_f)(uint16_t),
              uint16_t (CPU6502::*addr_f)(void))
            : cycles(cycles),
              run{std::bind(run_f, cpu, std::placeholders::_1)},
              addr{std::bind(addr_f, cpu)} {}
        short cycles;
        std::function<void(uint16_t)> run;
        std::function<uint16_t(void)> addr;
    } Instr;

    uint16_t Addr_ACC(); // Accumulator
    uint16_t Addr_IMM(); // Immediate
    uint16_t Addr_ABS(); // Absolute
    uint16_t Addr_ZER(); // Zero page
    uint16_t Addr_ZEX(); // Zero page, X-indexed
    uint16_t Addr_ZEY(); // Zero page, Y-indexed
    uint16_t Addr_ABX(); // Absolute, X-indexed
    uint16_t Addr_ABY(); // Absolute, Y-indexed
    uint16_t Addr_IMP(); // Implied
    uint16_t Addr_REL(); // Relative
    uint16_t Addr_INX(); // Indirect, X-indexed
    uint16_t Addr_INY(); // Indirect, Y-indexed
    uint16_t Addr_ABI(); // Absolute indirect

    void Op_ADC(uint16_t&);
    void Op_ASL(uint16_t&);
    void Op_BIT(uint16_t&);
    void Op_BRK(uint16_t&);
    void Op_JMP(uint16_t&);
    void Op_JSR(uint16_t&);
    void Op_LSR(uint16_t&);
    void Op_ROL(uint16_t&);
    void Op_ROR(uint16_t&);
    void Op_RTI(uint16_t&);
    void Op_RTS(uint16_t&);
    void Op_SBC(uint16_t&);

    std::map<std::string, std::function<void(uint16_t&)>> instr_funcs;

    inline std::function<void(uint16_t&)> bind_op(void (CPU6502::*op_f)(uint16_t&)) {
        return std::bind(op_f, this, std::placeholders::_1);
    }

    inline std::function<void(uint16_t&)> bit_op(std::function<uint8_t(uint8_t,uint8_t)> f);
    inline std::function<void(uint16_t&)> branch_op(uint8_t &flag, bool value=true);
    inline std::function<void(uint16_t&)> set_op(uint8_t &flag, bool value=true);
    inline std::function<void(uint16_t&)> compare_op(uint8_t &var);
    inline std::function<void(uint16_t&)> step_op(bool decrement=false);
    inline std::function<void(uint16_t&)> step_reg_op(uint8_t &reg, bool decrement=false);
    inline std::function<void(uint16_t&)> load_op(uint8_t reg);
    inline std::function<void(uint16_t&)> store_op(uint8_t &reg);
    inline std::function<void(uint16_t&)> push_op(uint8_t reg);
    inline std::function<void(uint16_t&)> pop_op(uint8_t &reg);
    inline std::function<void(uint16_t&)> transfer_op(uint8_t reg_a, uint8_t &reg_b); // b -> a

    inline void execute_instr(const Instr instr) {
        // TODO: Do something with cycles
        int temp = PC;
        uint16_t src = instr.addr();
        std::cout << std::hex << (int) temp << ": " << " " << (int) src << '\n';
        instr.run(src);
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
        mem->write_byte(S, data);
        S++;
    }

    inline void stack_push_word(uint16_t data) {
        mem->write_word(S, data);
        S += 2;
    }

    inline uint8_t stack_pop() {
        uint8_t temp = mem->read_byte(S);
        S--;
        return temp;
    }

    inline uint16_t stack_pop_word() {
        uint16_t temp = mem->read_word(S);
        S -= 2;
        return temp;
    }
};

#endif // CPU_6502
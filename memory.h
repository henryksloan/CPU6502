#ifndef MEMORY_H
#define MEMORY_H

#include <array>

class Memory {
 public:
    inline void write_byte(short addr, uint8_t data) {
        mem[addr] = data;
    }

    inline void write_word(short addr, uint16_t data) {
        mem[addr] = data & 0x00FF;
        mem[addr+1] = (data & 0xFF00) >> 8;
    }

    inline uint16_t read_byte(uint16_t addr) {
        return mem[addr];
    }

    inline short read_word(uint16_t addr) {
        return mem[addr] + (mem[addr+1] << 8);
    }

 private:
    std::array<uint8_t, 0xffff+1> mem;
};

#endif // MEMORY_H
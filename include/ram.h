#ifndef RAM_H
#define RAM_H

#include "memory.h"

// Basic RAM class without a bus
// For testing and trivial purposes
// Emulators should implement Memory to support bussing, etc.
template <size_t SIZE>
class RAM : public Memory {
 public:
    virtual inline void write_byte(uint16_t addr, uint8_t data) {
        mem[addr] = data;
    }

    virtual inline void write_word(uint16_t addr, uint16_t data) {
        mem[addr] = data & 0x00FF;
        mem[addr+1] = (data & 0xFF00) >> 8;
    }

    virtual inline uint8_t read_byte(uint16_t addr) {
        return mem[addr];
    }

    virtual inline uint16_t read_word(uint16_t addr) {
        return mem[addr] + (mem[addr+1] << 8);
    }

    virtual inline uint8_t &ref_byte(uint16_t addr) {
        return mem.at(addr);
    }

    virtual inline void ref_callback(uint16_t addr) {}

    virtual void load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start) {
        file.seekg(in_start, file.beg);
        int head = in_start;
        int i = mem_start;
        char temp;
        while (head <= in_end) {
            file.read(&temp, 1);
            // std::cout << temp;
            mem[i] = (uint8_t) temp;
            head++;
            i++;
        }
    }

    virtual void print() {
        for (auto& b : mem) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << (int) b;
        }
        std::cout << "\n";
    }

 protected:
    std::array<uint8_t, SIZE> mem;
};

template class RAM<0x100>;
template class RAM<0x20>;

#endif // RAM_H

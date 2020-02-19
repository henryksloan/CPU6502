#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <fstream>

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

    inline void load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start) {
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

    /* inline void load_file(std::ifstream &file, uint16_t mem_start) {
        load_file(file, 0, std::ios::end, mem_start);
    } */

    void print() {
        for (auto& b : mem) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << (int) b << " ";
        }
        std::cout << "\n";
    }

 private:
    std::array<uint8_t, 0x10000> mem;
};

#endif // MEMORY_H

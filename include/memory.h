#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <fstream>

class Memory {
 public:
    virtual void write_byte(uint16_t addr, uint8_t data) = 0;
    virtual void write_word(uint16_t addr, uint16_t data) = 0;
    virtual uint16_t read_byte(uint16_t addr) = 0;
    virtual uint16_t read_word(uint16_t addr) = 0;
    virtual uint8_t &ref_byte(uint16_t addr) = 0;
    // If addr is in memory, this tells memory that a write has been done
    virtual void ref_callback(uint16_t addr) = 0;
    virtual void load_file(std::ifstream &file, std::istream::pos_type in_start, std::istream::pos_type in_end, uint16_t mem_start) = 0;
    virtual void print() = 0;
};

#endif // MEMORY_H

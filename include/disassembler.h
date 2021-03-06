#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>

#include "instruction.h"

class Disassembler {
 public:
    Disassembler(uint16_t base) : base(base) {};
    void file_to_strings(std::ifstream &file);

 private:
    std::string instr_to_string(InstrInfo info, uint16_t PC, uint16_t src);

    uint16_t base; // Start address for program memory
    std::vector<std::string> instructions;
};

#endif // DISASSEMBLER_H

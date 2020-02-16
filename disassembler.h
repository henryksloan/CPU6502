#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>

class Disassembler {
 public:
    Disassembler(uint16_t base);
    ~Disassembler();

 // TODO: Uncomment
 // private:
     typedef struct AddrMode {
        short length;
        const char *format;
    } AddrMode;
    static const std::map<std::string, AddrMode> addr_modes;

    typedef struct Instr {
        Instr(std::string opcode,
              const AddrMode *mode)
            : opcode(opcode),
              mode(mode) {}
        std::string opcode;
        const AddrMode *mode;
    } Instr;
    std::array<Instr*, 0xff+1> instr_table;

    inline void register_instr(uint8_t num, std::string opcode, std::string mode) {
        instr_table[num] = new Disassembler::Instr(opcode, &addr_modes.at(mode));
    }

    std::string instr_to_string(Instr *instr, uint16_t PC, uint16_t src);
    void file_to_strings(std::ifstream &file);

    uint16_t base;
    std::vector<std::string> instructions;
};

#endif // DISASSEMBLER_H

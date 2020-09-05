#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

// Functional data for an addressing mode
typedef struct AddrMode {
    short length; // Number of bytes, not including opcode
    const std::string format; // A specially formatted string for (dis)assembly
} AddrMode;

// Functional information for an instruction
typedef struct InstrMode {
    uint8_t opcode;
    std::string mode_str;
    int cycles;
} InstrMode;

// Identifying information for an instruction
typedef struct InstrInfo {
    std::string op_str;
    std::string mode_str;
    int cycles;
} InstrInfo;

class Instructions {
 public:
    using instr_map_t = std::unordered_map<uint8_t, InstrInfo>;

    static InstrInfo get_info(uint8_t);
    static const instr_map_t instr_map; // Opcode uint8_t => Strings for op and mode
    static const std::unordered_map<std::string, AddrMode> mode_map; // mode_str => mode data

 private:
    static void add_instr(instr_map_t &map, std::string op_str, std::vector<InstrMode> modes);
    static instr_map_t generate_instr_map();
};

#endif // INSTRUCTION_H

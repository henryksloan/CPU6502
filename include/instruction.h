#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

typedef struct AddrMode {
    short length;
    const std::string format;
} AddrMode;

typedef struct InstrMode {
    uint8_t opcode;
    std::string mode_str;
    int cycles;
} InstrMode;

typedef struct InstrInfo {
    std::string op_str;
    std::string mode_str;
    int cycles;
} InstrInfo;

class Instructions {
 public:
    using instr_map_t = std::unordered_map<uint8_t, InstrInfo>;
    static const instr_map_t instr_map;
    static const std::unordered_map<std::string, AddrMode> mode_map;

 private:
    static void add_instr(instr_map_t &map, std::string op_str, std::vector<InstrMode> modes);
    static instr_map_t generate_instr_map();
};

#endif // INSTRUCTION_H

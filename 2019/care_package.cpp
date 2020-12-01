#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <algorithm>

// This puzzle makes use of the final Intcode Computer from Day 9

// Utilty functions

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ts(s);
    while (std::getline(ts, token, delimiter)) tokens.push_back(token);
    return tokens;
}
std::string join(const std::vector<std::string>& v, const std::string& delimiter) {
    std::string s;
    for (auto i = v.begin(); i != v.end(); ++i) s += (*i) + (i == v.end() - 1 ? "" : delimiter);
    return s;
}

// Actual code

int block_tiles = 0, score = 0;

struct GameTile {
    int x, y, id;

    GameTile(): x(0), y(0), id(0) {}
    GameTile(int x, int y, int id): x(x), y(y), id(id) {
        if (id == 2) block_tiles++;
    }

    char getTile() {
        switch (id) {
            case 1: return '#';
            case 2: return 'o';
            case 3: return '_';
            case 4: return '*';
            default: return ' ';
        }
    }
};
GameTile tiles[50][50];

void print_tiles() {
    for (int i = 0; i < 50; ++i) {
        for (int j = 0; j < 50; ++j) {
            std::cout << tiles[i][j].getTile();
        }
        std::cout << std::endl;
    }
}

// Intcode Computer

// True if you want diagnostic output
const bool DEBUG = false;
const bool SHOW_STATES = false;
const int EXTRA_MEMORY = 1000; // Extra memory tokens

std::vector<std::string> interpret_code(std::vector<std::string> tokens) {
    // Instruction pointer
    int ip = 0;

    // Relative base
    int relative_base = 0;

    const auto add_memory = [&tokens](std::size_t size) {
        if (tokens.size() < size) {
            tokens.resize(tokens.size() + size + 50, "0");
        }
    };

    // Start with extra tokens of memory
    add_memory(EXTRA_MEMORY);

    // Extra variables
    int x = -1, y = -1, id = -1;
    bool is_score = false;

    // Function to get arguments from opcode
    const auto op_arg = [&](int arg_index, const std::string& modes, const std::vector<std::string>& params, const bool is_output = false) -> long long {
        if (modes.length() <= arg_index) {
            std::string str_param = params[arg_index], result{};
            result = is_output ? str_param : tokens.at(std::stoi(str_param));
            return std::stoll(result);
        }

        std::string str_param = params[arg_index], result{};

        int param = std::stoi(str_param);
        switch (modes[arg_index]) {
            case '0': {
                // Position mode
                result = is_output ? str_param : tokens.at(param);
                break;
            }
            case '1': {
                // Immediate mode
                result = str_param;
                break;
            }
            case '2': {
                // Relative mode
                result = is_output ? std::to_string(relative_base + param) : tokens.at(relative_base + param);
                break;
            }
            default: {
                // Default to position mode
                result = is_output ? str_param : tokens.at(param);
            }
        }

        return std::stoll(result);
    };

    // Opcodes
    std::unordered_map<std::string, std::function<void(const std::string&, const std::vector<std::string>&)>> opcodes {
        {"ADD", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);
            auto outp = op_arg(2, modes, params, true);
            add_memory(outp);

            if (DEBUG) std::cout << ">>>> ADD | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << arg1 + arg2 << std::endl;
            tokens[outp] = std::to_string(arg1 + arg2);
            ip += 4;
        }},
        {"MUL", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);
            auto outp = op_arg(2, modes, params, true);
            add_memory(outp);

            if (DEBUG) std::cout << ">>>> MUL | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << arg1 * arg2 << std::endl;
            tokens[outp] = std::to_string(arg1 * arg2);
            ip += 4;
        }},
        {"INP", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto outp = op_arg(0, modes, params, true);
            add_memory(outp);

            if (DEBUG) std::cout << ">>>> INP | Output position = " << outp << std::endl;
            std::cout << "The program needs your input: ";
            std::cin >> tokens[outp];
            ip += 2;
        }},
        {"OUT", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);

            if (DEBUG) std::cout << ">>>> OUT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << std::endl;
            
            if (x == -1) {
                x = arg1;
            } else if (y == -1) {
                y = arg1;
            } else if (id == -1) {
                id = arg1;

                if (x == -1 && y == 0) {
                    score += id;
                } else {
                    tiles[x][y] = GameTile(x, y, id);
                }

                x = y = id = -1;
            }

            ip += 2;
        }},
        {"JIT", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);

            if (DEBUG) std::cout << ">>>> JIT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output value = " << (arg1 == 0 ? "is zero" : "not zero") << ", ip = " << ip << std::endl;
            ip = arg1 != 0 ? arg2 : ip + 3;
        }},
        {"JIF", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);

            if (DEBUG) std::cout << ">>>> JIF | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output value = " << (arg1 == 0 ? "is zero" : "not zero") << ", ip = " << ip << std::endl;
            ip = arg1 == 0 ? arg2 : ip + 3;
        }},
        {"SLT", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);
            int outp = op_arg(2, modes, params, true);
            add_memory(outp);

            if (DEBUG) std::cout << ">>>> SLT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << (arg1 < arg2 ? "less" : "greater") << std::endl;
            tokens[outp] = (arg1 < arg2 ? "1" : "0");
            ip += 4;
        }},
        {"SIE", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);
            int outp = op_arg(2, modes, params, true);
            add_memory(outp);

            if (DEBUG) std::cout << ">>>> SIE | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << (arg1 == arg2 ? "equals" : "not equals") << std::endl;
            tokens[outp] = (arg1 == arg2 ? "1" : "0");
            ip += 4;
        }},
        {"ARB", [&tokens, &ip, &relative_base, op_arg](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);

            if (DEBUG) std::cout << ">>>> ARB | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", new relative base = " << relative_base + arg1 << std::endl;
            relative_base += arg1;
            ip += 2;
        }}
    };

    try {
        for (; ip < tokens.size();) {
            // Parse instruction
            std::string instruction = tokens.at(ip);
            std::string opcode = instruction.length() > 2 ? instruction.substr(instruction.length() - 2) : instruction;
            std::string parameter_modes = instruction.length() > 2 ? instruction.substr(0, instruction.length() - 2) : "";
            std::reverse(parameter_modes.begin(), parameter_modes.end());

            int token = opcode == "99" ? 99 : std::stoi(opcode.length() == 2 ? opcode.substr(1) : opcode);
            if (DEBUG) std::cout << ">>>> OPCODE: " << opcode << " | PARAM MODES: " << parameter_modes << " | IP: " << ip << std::endl;
            switch (token) {
                case 1: opcodes["ADD"](parameter_modes, { tokens[ip + 1], tokens[ip + 2], tokens[ip + 3] }); break;
                case 2: opcodes["MUL"](parameter_modes, { tokens[ip + 1], tokens[ip + 2], tokens[ip + 3] }); break;
                case 3: opcodes["INP"](parameter_modes, { tokens[ip + 1] });                                 break;
                case 4: opcodes["OUT"](parameter_modes, { tokens[ip + 1] });                                 break;
                case 5: opcodes["JIT"](parameter_modes, { tokens[ip + 1], tokens[ip + 2] });                 break;
                case 6: opcodes["JIF"](parameter_modes, { tokens[ip + 1], tokens[ip + 2] });                 break;
                case 7: opcodes["SLT"](parameter_modes, { tokens[ip + 1], tokens[ip + 2], tokens[ip + 3] }); break;
                case 8: opcodes["SIE"](parameter_modes, { tokens[ip + 1], tokens[ip + 2], tokens[ip + 3] }); break;
                case 9: opcodes["ARB"](parameter_modes, { tokens[ip + 1] });                                 break;
                case 99: return tokens;
            }

            if (SHOW_STATES) {
                std::cout << std::endl << join(tokens, ",") << std::endl;
            }
        }

        std::cout << "Didn't find a 99 opcode. Returning from input end..." << std::endl;
        return tokens;
    } catch (const std::exception& e) {
        std::cerr << "ERROR! Failed to parse code: " << e.what() << std::endl;
        return {};
    }
}

int main() {
    std::vector<std::string> input = split("2,380,379,385,1008,2341,437598,381,1005,381,12,99,109,2342,1101,0,0,383,1102,0,1,382,21001,382,0,1,21002,383,1,2,21102,37,1,0,1105,1,578,4,382,4,383,204,1,1001,382,1,382,1007,382,37,381,1005,381,22,1001,383,1,383,1007,383,23,381,1005,381,18,1006,385,69,99,104,-1,104,0,4,386,3,384,1007,384,0,381,1005,381,94,107,0,384,381,1005,381,108,1105,1,161,107,1,392,381,1006,381,161,1101,-1,0,384,1105,1,119,1007,392,35,381,1006,381,161,1101,1,0,384,20102,1,392,1,21101,0,21,2,21101,0,0,3,21102,138,1,0,1105,1,549,1,392,384,392,21001,392,0,1,21102,21,1,2,21101,3,0,3,21102,1,161,0,1105,1,549,1101,0,0,384,20001,388,390,1,21001,389,0,2,21101,180,0,0,1106,0,578,1206,1,213,1208,1,2,381,1006,381,205,20001,388,390,1,21001,389,0,2,21102,1,205,0,1106,0,393,1002,390,-1,390,1101,0,1,384,20102,1,388,1,20001,389,391,2,21101,0,228,0,1106,0,578,1206,1,261,1208,1,2,381,1006,381,253,21001,388,0,1,20001,389,391,2,21102,253,1,0,1106,0,393,1002,391,-1,391,1101,1,0,384,1005,384,161,20001,388,390,1,20001,389,391,2,21102,279,1,0,1106,0,578,1206,1,316,1208,1,2,381,1006,381,304,20001,388,390,1,20001,389,391,2,21101,304,0,0,1106,0,393,1002,390,-1,390,1002,391,-1,391,1102,1,1,384,1005,384,161,20102,1,388,1,21001,389,0,2,21102,1,0,3,21102,338,1,0,1105,1,549,1,388,390,388,1,389,391,389,21002,388,1,1,21001,389,0,2,21101,4,0,3,21101,0,365,0,1106,0,549,1007,389,22,381,1005,381,75,104,-1,104,0,104,0,99,0,1,0,0,0,0,0,0,344,16,18,1,1,18,109,3,22101,0,-2,1,21202,-1,1,2,21101,0,0,3,21101,0,414,0,1106,0,549,22101,0,-2,1,22101,0,-1,2,21101,0,429,0,1105,1,601,2101,0,1,435,1,386,0,386,104,-1,104,0,4,386,1001,387,-1,387,1005,387,451,99,109,-3,2106,0,0,109,8,22202,-7,-6,-3,22201,-3,-5,-3,21202,-4,64,-2,2207,-3,-2,381,1005,381,492,21202,-2,-1,-1,22201,-3,-1,-3,2207,-3,-2,381,1006,381,481,21202,-4,8,-2,2207,-3,-2,381,1005,381,518,21202,-2,-1,-1,22201,-3,-1,-3,2207,-3,-2,381,1006,381,507,2207,-3,-4,381,1005,381,540,21202,-4,-1,-1,22201,-3,-1,-3,2207,-3,-4,381,1006,381,529,21201,-3,0,-7,109,-8,2105,1,0,109,4,1202,-2,37,566,201,-3,566,566,101,639,566,566,2101,0,-1,0,204,-3,204,-2,204,-1,109,-4,2106,0,0,109,3,1202,-1,37,593,201,-2,593,593,101,639,593,593,21002,0,1,-2,109,-3,2105,1,0,109,3,22102,23,-2,1,22201,1,-1,1,21102,431,1,2,21101,653,0,3,21102,851,1,4,21102,630,1,0,1106,0,456,21201,1,1490,-2,109,-3,2105,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,2,2,2,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,2,2,0,2,0,1,1,0,2,0,2,2,0,2,2,0,0,2,2,0,2,2,2,2,0,2,2,2,2,0,2,0,2,2,0,2,0,2,2,2,2,0,1,1,0,0,2,2,2,2,2,2,0,2,2,0,2,2,2,2,2,2,2,2,0,2,2,2,0,2,2,2,2,2,2,0,0,2,0,1,1,0,0,2,2,2,2,0,2,2,2,0,2,2,0,2,2,2,2,2,2,2,2,0,2,2,0,2,0,2,2,0,2,2,2,0,1,1,0,2,2,2,0,2,0,2,0,2,2,2,0,0,0,2,0,2,2,2,2,2,2,0,2,0,2,0,2,2,2,2,2,2,0,1,1,0,2,2,2,2,2,2,0,2,0,2,0,0,0,2,0,2,2,2,2,0,2,2,0,2,2,0,2,2,0,0,2,2,0,0,1,1,0,2,0,0,2,2,0,2,2,2,0,0,0,0,2,2,2,2,2,2,0,0,0,2,0,2,2,2,2,2,0,0,0,0,0,1,1,0,2,0,0,2,2,0,0,2,2,2,2,2,2,2,2,0,2,0,2,2,0,2,2,2,0,2,0,2,2,0,2,2,2,0,1,1,0,0,2,2,2,2,2,0,2,2,0,0,0,0,2,2,2,2,0,2,2,0,2,2,0,2,0,0,2,2,2,2,2,2,0,1,1,0,2,0,2,0,2,2,0,2,0,0,2,2,2,0,0,2,2,0,2,0,2,2,2,0,2,2,2,2,0,2,2,0,0,0,1,1,0,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,0,2,2,2,2,0,2,0,2,2,2,0,0,2,2,2,2,0,0,1,1,0,2,0,2,2,0,0,0,0,2,2,2,2,0,2,2,0,0,2,0,2,0,2,0,2,0,2,0,0,2,0,2,2,2,0,1,1,0,2,0,2,2,2,2,0,2,2,0,2,2,2,0,2,0,2,0,2,0,2,2,2,2,0,2,2,2,0,2,2,0,0,0,1,1,0,2,2,2,0,2,2,0,0,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,1,1,0,2,2,2,0,2,2,2,2,0,0,2,2,2,0,2,0,2,2,0,0,2,2,2,0,2,2,0,2,2,2,2,2,2,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,68,85,54,62,55,66,45,25,17,78,41,1,66,35,5,38,72,17,31,57,18,19,35,53,33,17,10,15,46,71,34,46,84,46,71,81,9,64,73,66,65,89,6,86,23,81,19,13,59,12,2,83,74,11,82,9,80,27,22,85,92,3,91,79,47,85,68,45,39,50,80,14,7,34,68,98,67,63,10,15,40,85,3,87,14,91,76,37,45,78,77,8,64,62,83,74,48,54,61,76,5,89,27,96,78,60,86,45,97,11,21,62,49,52,24,74,23,19,82,28,6,14,1,14,45,37,13,9,61,74,67,18,62,13,6,24,27,67,60,80,68,25,53,80,31,67,41,8,93,23,93,95,6,47,4,25,33,45,56,37,12,62,75,85,42,83,16,40,78,38,87,1,96,18,73,96,91,29,44,58,47,71,90,28,74,12,97,15,32,86,9,81,54,92,94,95,36,34,67,48,71,57,60,39,50,31,7,58,5,43,63,98,55,56,87,83,70,92,63,51,72,11,73,26,94,81,62,38,15,92,4,38,91,32,12,34,26,46,46,54,51,16,36,98,65,68,79,65,3,34,6,24,70,49,27,78,41,50,38,46,23,27,53,11,45,5,24,94,56,56,40,85,13,52,36,86,31,92,16,72,66,77,12,5,2,7,56,7,88,67,49,31,73,10,19,83,87,96,47,86,35,91,38,85,24,6,32,78,20,28,88,8,5,97,9,17,32,91,52,74,92,13,92,1,35,62,68,17,44,4,51,93,61,60,65,46,52,91,7,85,78,28,68,31,16,7,70,10,64,56,73,57,49,19,1,54,62,50,68,98,76,48,39,62,19,50,34,46,41,36,54,47,5,37,86,6,22,22,21,32,98,54,29,41,86,78,52,78,53,28,39,24,65,40,3,25,85,19,81,1,82,56,40,33,87,48,77,3,67,27,89,22,87,30,14,82,72,90,86,49,57,79,27,41,34,35,90,90,51,80,77,75,45,52,93,17,8,12,17,45,94,92,86,7,15,42,70,14,91,4,88,86,94,5,96,74,2,48,80,14,43,78,96,53,65,46,8,52,64,79,39,3,44,42,31,96,41,40,75,7,83,84,70,86,97,31,56,1,60,78,58,20,15,35,18,48,54,15,66,11,87,20,65,4,10,28,10,1,52,2,88,64,31,47,57,29,92,8,91,82,73,7,17,2,92,10,11,31,19,44,71,6,9,17,97,11,50,5,61,74,18,21,73,32,81,64,81,37,23,97,15,34,85,18,64,46,74,39,14,65,47,10,26,31,77,35,55,30,59,78,86,35,85,80,51,9,11,1,46,81,37,55,3,59,28,2,89,68,32,82,73,7,64,63,25,34,96,37,67,53,1,98,74,20,14,41,29,3,25,9,8,5,96,60,53,4,14,40,94,72,25,95,9,56,12,3,46,75,39,64,75,59,23,13,69,90,40,12,69,5,94,86,23,73,23,22,45,46,20,66,97,68,56,25,55,14,39,78,23,84,58,34,36,4,10,72,95,83,22,81,5,23,32,72,33,58,52,87,28,12,93,1,2,97,76,43,78,98,73,76,23,13,79,51,65,27,9,21,44,4,11,88,93,82,93,91,93,3,53,55,95,64,59,26,3,95,61,25,87,72,80,35,70,79,82,38,3,86,90,85,78,25,3,42,18,63,96,60,49,65,68,2,57,27,70,37,97,90,5,4,31,17,8,27,39,77,67,45,44,59,60,66,84,75,60,18,61,71,64,92,97,43,25,94,7,50,67,78,43,93,66,25,27,5,93,2,4,85,21,9,84,67,6,81,93,16,38,90,84,29,41,38,95,75,5,41,28,16,7,8,33,13,64,60,18,93,58,74,45,49,4,33,24,43,76,38,3,32,36,3,32,3,64,96,85,19,80,56,96,65,83,12,27,64,91,14,65,52,8,49,48,437598", ',');
    interpret_code(input);
    return 0;
}
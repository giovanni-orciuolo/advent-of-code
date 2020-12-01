#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <cmath>

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

const int CANVAS_WIDTH = 400, CANVAS_HEIGHT = 400;
std::string canvas[CANVAS_WIDTH][CANVAS_HEIGHT] {};

void reset_canvas() {
    for (int i = 0; i < CANVAS_WIDTH; ++i) {
        for (int j = 0; j < CANVAS_HEIGHT; ++j) {
            canvas[i][j] = "░";
        }
    }
}

void print_canvas() {
    for (int i = 0; i < CANVAS_WIDTH; ++i) {
        for (int j = 0; j < CANVAS_HEIGHT; ++j) {
            std::cout << canvas[i][j];
        }
        std::cout << std::endl;
    }
}

// P1C4SS0 Robot
using Coords = std::pair<int, int>;

Coords robot_position { 320, 320 };
int robot_angle = 90; // It starts facing "up"

std::set<std::string> painted_coords{};
std::unordered_map<std::string, bool> paint_info{
    { "320,320", true }
};

std::string robot_coords() {
    return std::to_string(robot_position.first) + "," + std::to_string(robot_position.second);
}
void robot_paint(bool white) {
    auto coords = robot_coords();
    paint_info[coords] = white;
    painted_coords.insert(coords);
    canvas[robot_position.second][robot_position.first] = white ? "▓" : "░";
}
void robot_move(bool rotate_right) {
    robot_angle += (rotate_right ? -90 : 90) % 360;
    robot_position.first += cosf((robot_angle * M_PI) / 180);
    robot_position.second += sinf((robot_angle * M_PI) / 180);
}
bool robot_on_white() {
    auto coords = robot_coords();
    if (paint_info.find(coords) == paint_info.end()) {
        return false; // I must be on black
    }
    return paint_info[coords];
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

    // Special flag to see if I painted and I should turn and move the robot
    bool painted = false;

    // Function to get arguments from opcode
    const auto op_arg = [&](int arg_index, const std::string& modes, const std::vector<std::string>& params, const bool is_output = false) -> long long {
        if (modes.length() <= arg_index) {
            std::string str_param = params[arg_index], result{};
            result = is_output ? str_param : tokens.at(std::stoll(str_param));
            return std::stoll(result);
        }

        std::string str_param = params[arg_index], result{};

        int param = std::stoll(str_param);
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
            tokens[outp] = robot_on_white() ? "1" : "0";
            ip += 2;
        }},
        {"OUT", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);

            if (DEBUG) std::cout << ">>>> OUT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << std::endl;
            !painted ? robot_paint(arg1) : robot_move(arg1);
            painted = !painted;
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
    reset_canvas();

    std::vector<std::string> input = split("3,8,1005,8,328,1106,0,11,0,0,0,104,1,104,0,3,8,1002,8,-1,10,1001,10,1,10,4,10,1008,8,0,10,4,10,1001,8,0,29,1,104,7,10,3,8,1002,8,-1,10,101,1,10,10,4,10,1008,8,0,10,4,10,1001,8,0,55,1,2,7,10,1006,0,23,3,8,102,-1,8,10,1001,10,1,10,4,10,1008,8,0,10,4,10,1001,8,0,84,1006,0,40,1,1103,14,10,1,1006,16,10,3,8,102,-1,8,10,101,1,10,10,4,10,108,1,8,10,4,10,1002,8,1,116,1006,0,53,1,1104,16,10,3,8,102,-1,8,10,101,1,10,10,4,10,1008,8,1,10,4,10,102,1,8,146,2,1104,9,10,3,8,102,-1,8,10,101,1,10,10,4,10,1008,8,1,10,4,10,1001,8,0,172,1006,0,65,1,1005,8,10,1,1002,16,10,3,8,102,-1,8,10,1001,10,1,10,4,10,108,0,8,10,4,10,102,1,8,204,2,1104,9,10,1006,0,30,3,8,102,-1,8,10,101,1,10,10,4,10,108,0,8,10,4,10,102,1,8,233,2,1109,6,10,1006,0,17,1,2,6,10,3,8,102,-1,8,10,101,1,10,10,4,10,108,1,8,10,4,10,102,1,8,266,1,106,7,10,2,109,2,10,2,9,8,10,3,8,102,-1,8,10,101,1,10,10,4,10,1008,8,1,10,4,10,1001,8,0,301,1,109,9,10,1006,0,14,101,1,9,9,1007,9,1083,10,1005,10,15,99,109,650,104,0,104,1,21102,1,837548789788,1,21101,0,345,0,1106,0,449,21101,0,846801511180,1,21101,0,356,0,1106,0,449,3,10,104,0,104,1,3,10,104,0,104,0,3,10,104,0,104,1,3,10,104,0,104,1,3,10,104,0,104,0,3,10,104,0,104,1,21101,235244981271,0,1,21101,403,0,0,1105,1,449,21102,1,206182744295,1,21101,0,414,0,1105,1,449,3,10,104,0,104,0,3,10,104,0,104,0,21102,837896937832,1,1,21101,0,437,0,1106,0,449,21101,867965862668,0,1,21102,448,1,0,1106,0,449,99,109,2,22102,1,-1,1,21101,40,0,2,21102,1,480,3,21101,0,470,0,1106,0,513,109,-2,2106,0,0,0,1,0,0,1,109,2,3,10,204,-1,1001,475,476,491,4,0,1001,475,1,475,108,4,475,10,1006,10,507,1101,0,0,475,109,-2,2106,0,0,0,109,4,1201,-1,0,512,1207,-3,0,10,1006,10,530,21102,1,0,-3,22102,1,-3,1,21201,-2,0,2,21102,1,1,3,21102,549,1,0,1106,0,554,109,-4,2105,1,0,109,5,1207,-3,1,10,1006,10,577,2207,-4,-2,10,1006,10,577,21202,-4,1,-4,1106,0,645,21202,-4,1,1,21201,-3,-1,2,21202,-2,2,3,21101,596,0,0,1106,0,554,21201,1,0,-4,21102,1,1,-1,2207,-4,-2,10,1006,10,615,21101,0,0,-1,22202,-2,-1,-2,2107,0,-3,10,1006,10,637,22102,1,-1,1,21101,637,0,0,105,1,512,21202,-2,-1,-2,22201,-4,-2,-4,109,-5,2106,0,0", ',');
    interpret_code(input);

    std::cout << "Unique tiles painted: " << painted_coords.size() << std::endl;
    print_canvas();
    
    return 0;
}
#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <algorithm>

// Day 9 version of intcode_computer (improved from Day 5)

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

// True if you want diagnostic output
const bool DEBUG = true;
const bool SHOW_STATES = true;

std::vector<std::string> interpret_code(std::vector<std::string> tokens) {
    // Instruction pointer
    int ip = 0;

    // Relative base
    int relative_base = 0;

    // External memory
    std::vector<std::string> external_memory(500, "0");

    // Function to get arguments from opcode
    const auto op_arg = [&](int index, const std::string& modes, const std::vector<std::string>& params) -> long long {
        if (modes.length() <= index) {
            int idx = std::stoi(params[index]);
            return std::stoll(index < tokens.size() ? tokens.at(index) : external_memory.at(index));
        }

        std::string arg{};
        switch (modes[index]) {
            case '0': {
                // Position mode
                int idx = std::stoi(params[index]);
                arg = index < tokens.size() ? tokens.at(index) : external_memory.at(index);
                break;
            }
            case '1': {
                // Immediate mode
                arg = params[index];
                break;
            }
            case '2': {
                // Relative mode
                int idx = std::stoi(params[index]);
                arg = index < tokens.size() ? tokens.at(relative_base + idx) : external_memory.at(relative_base + idx);
                break;
            }
            default: {
                // Default to position mode
                int idx = std::stoi(params[index]);
                arg = index < tokens.size() ? tokens.at(index) : external_memory.at(index);
            }
        }

        return std::stoll(arg);
    };

    // Opcodes
    std::unordered_map<std::string, std::function<void(const std::string&, const std::vector<std::string>&)>> opcodes {
        {"ADD", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);
            auto outp = std::stoi(params[2]);

            if (DEBUG) std::cout << ">>>> ADD | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << arg1 + arg2 << std::endl;
            outp < tokens.size() ? tokens[outp] = std::to_string(arg1 + arg2) : external_memory[outp] = std::to_string(arg1 + arg2);
            ip += 4;
        }},
        {"MUL", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);
            auto outp = std::stoi(params[2]);

            if (DEBUG) std::cout << ">>>> MUL | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << arg1 * arg2 << std::endl;
            outp < tokens.size() ? tokens[outp] = std::to_string(arg1 * arg2) : external_memory[outp] = std::to_string(arg1 * arg2);
            ip += 4;
        }},
        {"INP", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto outp = std::stoi(params[0]);

            if (DEBUG) std::cout << ">>>> INP | Output position = " << outp << std::endl;
            std::cout << "The program needs your input: ";
            std::cin >> (outp < tokens.size() ? tokens[outp] : external_memory[outp]);
            ip += 2;
        }},
        {"OUT", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);

            if (DEBUG) std::cout << ">>>> OUT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << std::endl;
            std::cout << arg1 << std::endl;
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
            int outp = std::stoi(params[2]);

            if (DEBUG) std::cout << ">>>> SLT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << (arg1 < arg2 ? "less" : "greater") << std::endl;
            outp < tokens.size() ? tokens[outp] = (arg1 < arg2 ? "1" : "0") : external_memory[outp] = (arg1 < arg2 ? "1" : "0");
            ip += 4;
        }},
        {"SIE", [&](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);
            auto arg2 = op_arg(1, modes, params);
            int outp = std::stoi(params[2]);

            if (DEBUG) std::cout << ">>>> SIE | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << (arg1 == arg2 ? "equals" : "not equals") << std::endl;
            outp < tokens.size() ? tokens[outp] = (arg1 == arg2 ? "1" : "0") : external_memory[outp] = (arg1 == arg2 ? "1" : "0");
            ip += 4;
        }},
        {"ARB", [&tokens, &ip, &relative_base, op_arg](const std::string& modes, const std::vector<std::string>& params) {
            auto arg1 = op_arg(0, modes, params);

            if (DEBUG) std::cout << ">>>> ARB | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << std::endl;
            relative_base += arg1;
            ip += 2;
        }}
    };

    try {
        for (; ip < tokens.size();) {
            // Parse instruction
            std::string instruction = ip < tokens.size() ? tokens.at(ip) : external_memory.at(ip);
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
                std::cout << "External memory:" << std::endl << join(external_memory, ",") << std::endl << std::endl;
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
    std::vector<std::string> input = split("109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99", ',');
    interpret_code(input);
    return 0;
}
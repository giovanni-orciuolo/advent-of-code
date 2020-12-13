#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <algorithm>
#include <regex>

bool starts_with(const std::string& s, const std::string& prefix) {
    return s.rfind(prefix, 0) == 0;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ts(s);
    while (std::getline(ts, token, delimiter)) tokens.push_back(token);
    return tokens;
}

// Modifies the accumulator and returns an offset to jump relative from
int parse_instruction(const std::string& instruction, const std::vector<std::string>& args, int& accumulator) {
    // Each instruction should return the modified value of the accumulator
    const std::unordered_map<std::string, std::function<int(const std::vector<std::string>&)>> interpreter{
        {"acc", [&accumulator](const std::vector<std::string>& args) -> int {
            accumulator += std::stoi(args[0]);
            return 1;
        }},
        {"jmp", [](const std::vector<std::string>& args) -> int {
            return std::stoi(args[0]);
        }},
        {"nop", [](const std::vector<std::string>& args) -> int {
            return 1;
        }}
    };

    return interpreter.at(instruction)(args);
}

// Returns true if the program never executed the same instruction twice, false otherwise
bool run_code(const std::vector<std::string>& instructions, int& accumulator) {
    std::vector<int> executed_addresses{};

    for (int ip = 0; ip < instructions.size();) {
        if (std::find(executed_addresses.begin(), executed_addresses.end(), ip) != executed_addresses.end()) {
            return false;
        }
        executed_addresses.push_back(ip);

        std::string instruction = instructions[ip];
        std::vector<std::string> cmd = split(instruction, ' ');
        std::vector<std::string> args(cmd.begin() + 1, cmd.end());

        ip += parse_instruction(cmd[0], args, accumulator);
    }

    return true;
}

int main() {
    // Read input
    std::ifstream file("inputs/handheld_boot.txt");
    std::vector<std::string> instructions{};
    
    for (std::string instruction; std::getline(file, instruction);) {
        instructions.push_back(instruction);
    }

    file.close();

    // Part one
    int accumulator = 0;

    if (!run_code(instructions, accumulator)) {
        std::cout << "An instruction is about to be executed twice! The current value of the accumulator is: " << accumulator << std::endl;
    }

    // Part two
    accumulator = 0;
    bool found_replacement = false;

    // Bruteforce first by changing one by one JMPs to NOPs and then NOPs to JMPs
    for (int i = 0; i < instructions.size(); ++i) {
        accumulator = 0;
        bool mod = false;
        if (starts_with(instructions[i], "jmp")) {
            instructions[i] = std::regex_replace(instructions[i], std::regex("jmp"), "nop");
            mod = true;
        }
        if (mod && run_code(instructions, accumulator)) {
            found_replacement = true;
            break;
        }
        if (mod) {
            instructions[i] = std::regex_replace(instructions[i], std::regex("nop"), "jmp");
        }
    }

    if (found_replacement) {
        std::cout << "The bootloader exited without looping! The current value of the accumulator is: " << accumulator << std::endl;
        return 0;
    }

    std::cout << "No fix found by replacing JMPs with NOPs. Trying to replace NOPs with JMPs..." << std::endl;

    for (int i = 0; i < instructions.size(); ++i) {
        accumulator = 0;
        bool mod = false;
        if (starts_with(instructions[i], "nop")) {
            instructions[i] = std::regex_replace(instructions[i], std::regex("nop"), "jmp");
            mod = true;
        }
        if (mod && run_code(instructions, accumulator)) {
            found_replacement = true;
            break;
        }
        if (mod) {
            instructions[i] = std::regex_replace(instructions[i], std::regex("jmp"), "nop");
        }
    }

    if (found_replacement) {
        std::cout << "The bootloader exited without looping! The current value of the accumulator is: " << accumulator << std::endl;
        return 0;
    }

    std::cout << "No fix found by replacing NOPs with JMPs. Wtf..." << std::endl;
    // Wtf bro??? I can't find a code that never repeats a single instruction!!! Bruh

    return 0;
}
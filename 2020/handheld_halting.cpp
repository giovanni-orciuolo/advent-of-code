#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <algorithm>

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

    // Bruteforce first by changing one by one JMPs to NOPs and then NOPs to JMPs
    for (int i = 0; i < instructions.size(); ++i) {
        bool mod = false;
        if (instructions[i] == "jmp") {
            instructions[i] = "nop";
            mod = true;
        }
        if (run_code(instructions, accumulator)) {
            std::cout << "The bootloader exited without looping! The current value of the accumulator is: " << accumulator << std::endl;
            break;
        }
        if (mod) {
            instructions[i] = "jmp";
        }
    }

    std::cout << "No fix found by replacing JMPs with NOPs. Trying to replace NOPs with JMPs..." << std::endl;

    for (int i = 0; i < instructions.size(); ++i) {
        bool mod = false;
        if (instructions[i] == "nop") {
            instructions[i] = "jmp";
            mod = true;
        }
        if (run_code(instructions, accumulator)) {
            std::cout << "The bootloader exited without looping! The current value of the accumulator is: " << accumulator << std::endl;
            break;
        }
        if (mod) {
            instructions[i] = "nop";
        }
    }

    return 0;
}
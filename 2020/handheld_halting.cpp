#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main() {
    // Read input
    std::ifstream file("inputs/handheld_boot.txt");
    std::vector<std::string> instructions{};
    
    for (std::string instruction; std::getline(file, instruction);) {
        instructions.push_back(instruction);
    }

    // Part one
    std::vector<int> executed_addresses{};

    for (const std::string& instruction : instructions) {
        
    }

    return 0;
}
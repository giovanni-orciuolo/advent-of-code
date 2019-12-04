#include <iostream>
#include <vector>
#include <sstream>

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

std::vector<std::string> interpret_code(std::vector<std::string> tokens) {
    try {
        for (int i = 0; i < tokens.size(); ++i) {
            int token = std::stoi(tokens[i]);
            switch (token) {
                case 1: {
                    int arg1 = std::stoi(tokens[i + 1]);
                    int arg2 = std::stoi(tokens[i + 2]);
                    int outp = std::stoi(tokens[i + 3]);
                    tokens[outp] = std::to_string(std::stoi(tokens[arg1]) + std::stoi(tokens[arg2]));
                    i += 3;
                    break;
                }
                case 2: {
                    int arg1 = std::stoi(tokens[i + 1]);
                    int arg2 = std::stoi(tokens[i + 2]);
                    int outp = std::stoi(tokens[i + 3]);
                    tokens[outp] = std::to_string(std::stoi(tokens[arg1]) * std::stoi(tokens[arg2]));
                    i += 3;
                    break;
                }
                case 99: return tokens;
            }
        }

        std::cout << "Didn't find a 99 opcode. Returning from input end..." << std::endl;
        return tokens;
    } catch (const std::exception& e) {
        std::cerr << "ERROR! Failed to parse code: " << e.what() << std::endl;
        return {};
    }
}

int find_noun_and_verb(std::vector<std::string> tokens) {
    // Bruteforce values between 0 and 99
    for (int noun = 0; noun < 100; ++noun) {
        for (int verb = 0; verb < 100; ++verb) {
            tokens[1] = std::to_string(noun);
            tokens[2] = std::to_string(verb);
            auto interpreted = interpret_code(tokens);
            if (interpreted[0] == "19690720") {
                std::cout << "Found noun and verb!" << std::endl;
                return 100 * noun + verb;
            }
        }
    }
    std::cout << "Failed to find noun and verb..." << std::endl;
    return -1;
}

int main() {
    std::vector<std::string> input = split("1,12,2,3,1,1,2,3,1,3,4,3,1,5,0,3,2,10,1,19,1,6,19,23,2,23,6,27,1,5,27,31,1,31,9,35,2,10,35,39,1,5,39,43,2,43,10,47,1,47,6,51,2,51,6,55,2,55,13,59,2,6,59,63,1,63,5,67,1,6,67,71,2,71,9,75,1,6,75,79,2,13,79,83,1,9,83,87,1,87,13,91,2,91,10,95,1,6,95,99,1,99,13,103,1,13,103,107,2,107,10,111,1,9,111,115,1,115,10,119,1,5,119,123,1,6,123,127,1,10,127,131,1,2,131,135,1,135,10,0,99,2,14,0,0", ',');
    std::cout << join(interpret_code(input), ",") << std::endl;
    std::cout << find_noun_and_verb(input) << std::endl;
    return 0;
}
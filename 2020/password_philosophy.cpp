#include <iostream>
#include <fstream>
#include <string>
#include <regex>

int main() {
    // Read input
    std::ifstream file("inputs/passwords.txt");
    std::vector<std::string> patterns{};
    
    for (std::string p; std::getline(file, p);) {
        patterns.push_back(p);
    }

    file.close();

    // Part one
    std::regex regex("([0-9]*)-([0-9]*) ([a-z]|[A-Z]|[0-9]): (.*)");
    std::smatch matches;
    int num_valids = 0;
    
    for (const std::string& p : patterns) {
        if (std::regex_search(p, matches, regex)) {
            int low = std::stoi(matches[1].str()), high = std::stoi(matches[2].str());
            char token = matches[3].str()[0];
            std::string password = matches[4].str();

            int token_count = 0;
            for (char c : password) {
                if (c == token) {
                    token_count++;
                }
            }

            if (token_count >= low && token_count <= high) {
                num_valids++;
            }
        }
    }

    std::cout << "[Part one] Number of valid passwords: " << num_valids << std::endl;

    // Part two
    num_valids = 0;
    
    for (const std::string& p : patterns) {
        if (std::regex_search(p, matches, regex)) {
            int first_idx = std::stoi(matches[1].str()) - 1, second_idx = std::stoi(matches[2].str()) - 1;
            char token = matches[3].str()[0];
            std::string password = matches[4].str();

            if (password[first_idx] == token ^ password[second_idx] == token) {
                // std::cout << "Valid: " << "[first_idx] = " << first_idx << " | [second_idx] = " << second_idx << " | [token] = " << token << " | [password] = " << password << std::endl;
                num_valids++;
            }
        }
    }

    std::cout << "[Part two] Number of valid passwords: " << num_valids << std::endl;
}
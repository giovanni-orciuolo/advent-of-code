#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sstream>

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ts(s);
    while (std::getline(ts, token, delimiter)) tokens.push_back(token);
    return tokens;
}

int main() {
    // Read input
    std::ifstream file("inputs/customs.txt");
    std::vector<std::string> answers{};
    std::string group{};
    
    for (std::string line; std::getline(file, line);) {
        if (line.empty()) {
            // Reached blank line, parse new group
            answers.push_back(group);
            group = "";
            continue;
        }
        if (group.length() > 0) {
            group += "|";
        }
        
        group += line;
    }

    answers.push_back(group);

    // Part one
    int sum = 0;

    for (std::string a : answers) {
        std::unordered_map<char, bool> unique_questions{};
        a.erase(std::remove(a.begin(), a.end(), '|'), a.end());
        
        for (char c : a) {
            unique_questions[c] = true;
        }
        sum += unique_questions.size();
    }

    std::cout << "Sum of answers count: " << sum << std::endl;

    // Part two
    const std::vector<char> questions{
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
    }; // Listen, we are not code golfing here ok!?
    sum = 0;

    for (const std::string& a : answers) {
        int count = 0;
        std::vector<std::string> separate_answers = split(a, '|');
        for (char q : questions) {
            bool appears = true;
            for (const std::string& s : separate_answers) {
                if (s.find(q) == std::string::npos) {
                    appears = false;
                    break;
                }
            }
            if (appears) {
                count++;
            }
        }
        sum += count;
    }

    std::cout << "Sum of global answers count: " << sum << std::endl;

    return 0;
}
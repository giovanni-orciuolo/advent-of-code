#include <iostream>
#include <vector>
#include <sstream>

// Utility functions

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ts(s);
    while (std::getline(ts, token, delimiter)) tokens.push_back(token);
    return tokens;
}
int ctoi(const char c) {
    return std::stoi(std::to_string(c));
}

// Actual code

std::vector<std::string> find_passwords(int range_min, int range_max) {
    const auto is_valid = [range_min, range_max](int password) -> bool {
        int couple = -1, prev = -1, tail = -1;
        for (char c : std::to_string(password)) {
            int digit = ctoi(c);
            if (digit < prev) return false;
            if (digit == prev && digit != tail && couple == -1) couple = digit;
            if (digit == prev && digit == tail && couple == digit) couple = -1;
            tail = prev;
            prev = digit;
        }
        return couple != -1 && (password > range_min && password < range_max);
    };
    std::vector<std::string> valid_passwords{};

    for (int p = range_min; p < range_max; ++p) {
        if (is_valid(p)) {
            valid_passwords.push_back(std::to_string(p));
        }
    }

    return valid_passwords;
}

int main() {
    const auto passwords = find_passwords(138241, 674034);
    for (const std::string& pass : passwords) {
        std::cout << "Valid password: " << pass << std::endl;
    }

    std::cout << "Number of valid passwords: " << passwords.size() << std::endl;
    return 0;
}
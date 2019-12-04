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

std::vector<std::string> find_passwords(const std::string& range) {
    const std::string& range_min = split(range, '-')[0];
    const std::string& range_max = split(range, '-')[1];

    int prev_digit = -1, start_index = range_min.length() - 1;
    for (int i = 0; i < range_min.length(); ++i) {
        const char c = range_min[i];
        int digit = ctoi(c);

        if (prev_digit == -1) {
            prev_digit = digit;
            continue;
        }

        if (prev_digit > digit) {
            start_index = i;
        }

        prev_digit = digit;
    }
}

int main() {
    // TODO: FINISH THIS!!!

    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

std::vector<std::string> read_input_file(std::string file_name) {
    std::ifstream file("inputs/" + file_name);
    std::vector<std::string> values{};
    
    for (std::string value; std::getline(file, value);) {
        values.push_back(value);
    }

    file.close();
    return values;
}

template<typename T> 
void print_vector(std::vector<T> vec) {
    for (auto v : vec) {
        std::cout << v << std::endl;
    }
}

int main() {
    auto input = read_input_file("calories.txt");
    std::vector<int> calories{};

    // Group calories into a single vector
    int sum = 0;
    for (auto v : input) {
        if (v.empty()) {
            calories.push_back(sum);
            sum = 0;
            continue;
        }

        sum += stoi(v);
    }
    calories.push_back(sum);

    // Find top three largest calories
    std::sort(calories.begin(), calories.end(), [](int a, int b) {
        return b < a;
    });

    std::cout 
        << "1. " << calories[0] << std::endl
        << "2. " << calories[1] << std::endl
        << "3. " << calories[2] << std::endl
        << "Total: " << calories[0] + calories[1] + calories[2] << std::endl;

    return 0;
}
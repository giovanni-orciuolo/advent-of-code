#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// Change type used in vector here
// long is good enough, but you can use long if you want
// or int idk
// it depends on how big the numbers get in the input
typedef long number;

int main() {
    // Read input
    std::ifstream file("inputs/xmas_encoded.txt");
    std::vector<number> numbers{};
    
    for (std::string n; std::getline(file, n);) {
        numbers.push_back(std::stol(n));
    }

    file.close();

    // Part one
    const short PREAMBLE_LENGTH = 25;
    number invalid = -1;

    for (auto it = numbers.begin() + PREAMBLE_LENGTH; it < numbers.end(); ++it) {
        number n = *it;
        bool sum_exists = false;
        std::vector<number> slice(it - PREAMBLE_LENGTH, it);
        for (number i : slice) {
            for (number k : slice) {
                if (i != k && i + k == n) {
                    sum_exists = true;
                    break;
                }
            }
            if (sum_exists) {
                break;
            }
        }
        if (!sum_exists) {
            std::cout << "Found invalid number N = " << n << std::endl;
            invalid = n;
            break;
        }
    }

    if (invalid == -1) {
        std::cout << "Invalid number does not exist. Can't break XMAS encryption without it :(" << std::endl;
        return 1;
    }

    // Part two
    std::vector<number> slice_sum_to_invalid{};
    bool found_sum_to_invalid = false;

    for (auto it = numbers.begin(); it < numbers.end(); ++it) {
        int sum = 0;
        for (auto kt = it + 1; kt < numbers.end(); ++kt) {
            sum += *kt;
            if (sum == invalid) {
                found_sum_to_invalid = true;
                std::vector<number> slice(it, kt);
                slice_sum_to_invalid = slice;
                break;
            }
        }
        if (found_sum_to_invalid) {
            break;
        }
    }

    if (!found_sum_to_invalid) {
        std::cout << "Encryption weakness does not exist. Can't break XMAS encryption without it :(" << std::endl;
        return 1;
    }

    number min = *std::min_element(slice_sum_to_invalid.begin(), slice_sum_to_invalid.end());
    number max = *std::max_element(slice_sum_to_invalid.begin(), slice_sum_to_invalid.end());
    number weakness = min + max;

    std::cout << "XMAS encryption weakness value = " << weakness << std::endl;

    return 0;
}
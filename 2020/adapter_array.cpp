#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

void scan_adapters(const std::vector<int>& adapters, std::vector<int>& sorted_adapters, int adapter, int& count_delta1, int& count_delta3) {
    sorted_adapters.push_back(adapter);

    std::vector<int> deltas{};
    // std::cout << "Adapter: " << adapter << std::endl;

    for (int a : adapters) {
        int delta = a - adapter;
        if (delta < 1 || delta > 3) {
            // Not a valid sequential adapter
            continue;
        }

        deltas.push_back(delta);
    }

    // Exit condition: there are no more valid deltas
    if (deltas.size() == 0) {
        // Since we are exiting, it means we are at the end of the chain
        // And my device's built-in adapter is always 3 higher than the highest adapter
        count_delta3++;
        return;
    }

    std::cout << "Number of deltas: " << deltas.size() << " { ";
    for (int d : deltas) {
        std::cout << d << " [" << adapter + d << "] ";
    }
    std::cout << "}" << std::endl;

    // Pick the lowest delta
    int chosen_delta = *std::min_element(deltas.begin(), deltas.end());
    if (chosen_delta == 1) { count_delta1++; }
    if (chosen_delta == 3) { count_delta3++; }

    // std::cout << "Chosen delta: " << chosen_delta << std::endl;

    // Call recursively to pick next adapter in chain
    scan_adapters(adapters, sorted_adapters, adapter + chosen_delta, count_delta1, count_delta3);
}

int main() {
    // Read input
    std::ifstream file("inputs/adapters.txt");
    std::vector<int> adapters{};
    
    for (std::string a; std::getline(file, a);) {
        adapters.push_back(std::stoi(a));
    }

    file.close();

    // Part one
    int count_delta1 = 0, count_delta3 = 0;
    std::vector<int> sorted_adapters{};
    scan_adapters(adapters, sorted_adapters, 0, count_delta1, count_delta3);
    
    std::cout << "There are " << count_delta1 << " differences of 1 jolt and " << count_delta3 << " differences of 3 jolts." << std::endl;
    std::cout << "Multiplied: " << count_delta1 * count_delta3 << std::endl;

    // Part two
    std::vector<int> consecutives{};
    int consecutive = 0;
    for (int i = 0; i < sorted_adapters.size(); ++i) {
        if (sorted_adapters[i + 1] - sorted_adapters[i] == 1) {
            consecutive++;
            continue;
        }

        consecutive--;
        if (consecutive >= 1) {
            consecutives.push_back(consecutive);
        }
        consecutive = 0;
    }

    long arrangements = 1; // Start from what we already have: the sorted array
    int run_combinations[4] { 1, 2, 4, 7 }; // No vector needed...
    for (int c : consecutives) {
        arrangements *= run_combinations[c];
    }

    std::cout << "Total arrangements count: " << arrangements << std::endl;

    return 0;
}
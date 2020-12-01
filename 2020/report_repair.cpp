#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main() {
    std::ifstream file("expense_report.txt");
    std::vector<int> values{};
    
    for (std::string value; getline(file, value);) {
        values.push_back(stoi(value));
    }

    file.close();
    bool found = false;

    // First part

    for (int v : values) {
        for (int vv : values) {
            if (v + vv == 2020) {
                std::cout << "First part answer: " << v << " * " << vv << " = " << v * vv << std::endl;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    // Second part
    found = false;

    for (int v : values) {
        for (int vv : values) {
            for (int vvv : values) {
                if (v + vv + vvv == 2020) {
                    std::cout << "Second part answer: " << v << " * " << vv << " * " << vvv << " = " << v * vv * vvv << std::endl;
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        if (found) break;
    }

    // O(n^2) and O(n^3) is a major BRUH moment but I just wanted to solve this quickly since I have to work.
    // This problem is a good candidate for Upping the Ante!

    std::cout << "Done!" << std::endl;
    return 0;
}
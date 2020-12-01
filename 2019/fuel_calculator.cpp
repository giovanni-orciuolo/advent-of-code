#include <iostream>
#include <fstream>
#include <cmath>

int calculate_fuel(int mass) {
    return floor(mass / 3) - 2;
}

int calculate_fuel_re(int mass, int sum) {
    int fuel = calculate_fuel(mass);
    if (fuel <= 0) return sum;
    sum += fuel;
    return calculate_fuel_re(fuel, sum);
}

int main() {
    std::ifstream file("module_masses.txt");
    int fuels_sum = 0, fuels_sum_re = 0;

    for (std::string module_mass; std::getline(file, module_mass);) {
        int mass = std::stoi(module_mass);
        int fuel = calculate_fuel(mass);
        fuels_sum += fuel;
        int fuel_re = calculate_fuel_re(mass, 0);
        fuels_sum_re += fuel_re;
    }

    std::cout << "Total fuel required: " << fuels_sum << std::endl;
    std::cout << "Total recursive fuel required: " << fuels_sum_re << std::endl;

    file.close();
    return 0;
}
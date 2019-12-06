#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
#include <functional>
#include <memory>

// Utility functions

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ts(s);
    while (std::getline(ts, token, delimiter)) tokens.push_back(token);
    return tokens;
}

// Actual code

class Planet {
    private:
        std::string const name;
        Planet* const parent; // This planet orbits around parent
        public: std::vector<Planet*> orbiters; // These orbiters orbit around this planet
    public:
        Planet(const std::string& name, Planet* parent):
            name(name), parent(parent) {};
        ~Planet() {
            for (Planet* orbiter : orbiters) delete orbiter;
        }

        void addOrbiter(Planet* orbiter) {
            orbiters.emplace_back(orbiter);
        }

        Planet* getParent() const { return parent; }
        std::string getName() const { return name; }

        bool hasOrbiters() { return !orbiters.empty(); }
        bool hasSingleOrbiter() { return orbiters.size() > 1; }
};

int main() {
    std::ifstream map_file("orbit_map.txt");
    std::vector<std::pair<std::string, std::string>> map{};

    for (std::string orbit; getline(map_file, orbit);) {
        auto p = split(orbit, ')');
        auto orbited = p[0], orbiter = p[1];
        map.push_back({ orbited, orbiter });
    }

    std::vector<Planet*> branches_ends{};

    const std::function<void(Planet*, Planet*)> fill_tree = [&](Planet* center, Planet* orbiter) -> void {
        center->addOrbiter(orbiter);

        for (auto orbit : map) {
            if (orbiter->getName().find(orbit.first) != std::string::npos) {
                fill_tree(orbiter, new Planet(orbit.second, orbiter));
            }
        }

        if (!orbiter->hasOrbiters()) {
            std::cout << "Found the end of a branch! It's " << orbiter->getName() << std::endl;
            branches_ends.emplace_back(orbiter);
        }
    };

    Planet* root_planet = new Planet("COM", nullptr);
    fill_tree(root_planet, new Planet("B", root_planet));

    int sum = 0, checksum = 0;
    const std::function<Planet*(Planet*)> calculate_checksum = [&](Planet* from) -> Planet* {
        if (from->getParent() == nullptr) {
            return nullptr;
        }

        if (from->hasSingleOrbiter()) {
            sum++;
        }
        calculate_checksum(from->getParent());
        return from->getParent();
    };

    for (Planet* ends : branches_ends) {
        Planet* curr_node = ends;
        while (curr_node != nullptr) {
            curr_node = calculate_checksum(curr_node);
        }
        checksum += sum;
        sum = 0;
    }

    std::cout << "Map checksum is: " << checksum << std::endl;

    delete root_planet; // Free memory

    map_file.close();
    return 0;
}
#include <iostream>
#include <unordered_map>
#include <unordered_set>
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
        std::vector<Planet*> orbiters; // These orbiters orbit around this planet
    public:
        Planet(const std::string& name, Planet* parent):
            name(name), parent(parent) {};

        void addOrbiter(Planet* orbiter) {
            orbiters.emplace_back(orbiter);
        }

        Planet* getParent() const { return parent; }
        std::string getName() const { return name; }
        std::vector<Planet*> getOrbiters() const { return orbiters; }

        bool hasOrbiters() { return !orbiters.empty(); }
        bool hasSingleOrbiter() { return orbiters.size() <= 1; }

        void dispose() {
            for (Planet* orbiter : orbiters) {
                orbiter->dispose();
                delete orbiter;
            }
            orbiters.clear();
        }
};

int main() {
    std::vector<std::pair<std::string, std::string>> map{};

    std::ifstream map_file("orbit_map.txt");
    for (std::string orbit; getline(map_file, orbit);) {
        auto p = split(orbit, ')');
        auto orbited = p[0], orbiter = p[1];
        map.push_back({ orbited, orbiter });
    }
    map_file.close();

    std::vector<Planet*> branches_ends{};

    Planet* you_node,
          * san_node;

    const std::function<void(Planet*, Planet*)> fill_tree = [&](Planet* center, Planet* orbiter) -> void {
        center->addOrbiter(orbiter);

        for (auto orbit : map) {
            if (orbiter->getName().find(orbit.first) != std::string::npos) {
                fill_tree(orbiter, new Planet(orbit.second, orbiter));
            }
        }

        if (!orbiter->hasOrbiters()) {
            // std::cout << "Found the end of a branch! It's " << orbiter->getName() << std::endl;
            branches_ends.emplace_back(orbiter);
            if (orbiter->getName() == "YOU") {
                you_node = orbiter;
            }
            if (orbiter->getName() == "SAN") {
                san_node = orbiter;
            }
        }
    };

    // Must initialize with a correct, single Center of Mass (COM) and its only orbiter
    Planet* root_planet = new Planet("COM", nullptr);
    fill_tree(root_planet, new Planet("4FT", root_planet));

    int sum = 0, checksum = 0;
    const std::function<Planet*(Planet*)> calculate_checksum = [&](Planet* from) -> Planet* {
        if (from->getParent() == nullptr) {
            return nullptr;
        }

        sum++;
        calculate_checksum(from->getParent());
        return from->getParent();
    };

    std::unordered_set<std::string> scanned_nodes;
    for (Planet* ends : branches_ends) {
        Planet* curr_node = ends;
        while (curr_node != nullptr) {
            Planet old_node = *curr_node;

            curr_node = scanned_nodes.find(curr_node->getName()) == scanned_nodes.end()
                ? calculate_checksum(curr_node)
                : curr_node->getParent();

            scanned_nodes.insert(old_node.getName());
        }
        checksum += sum;
        sum = 0;
    }

    std::cout << "Map checksum is: " << checksum << std::endl;

    // Walk through both orbit paths and overlap them to find shortest route from YOU to SAN
    const std::function<void(std::vector<std::string>&, const Planet*)> walk_to_root = [&](std::vector<std::string>& path, const Planet* node) -> void {
        if (node->getParent()->getName() != "COM") {
            path.emplace_back(node->getParent()->getName());
            walk_to_root(path, node->getParent());
        }
    };

    std::vector<std::string> you_path{}, san_path{};
    walk_to_root(you_path, you_node);
    walk_to_root(san_path, san_node);

    int you_path_len = you_path.size();
    int san_path_len = san_path.size();

    // Make you_path the overlapping path
    you_path.insert(you_path.end(), san_path.begin(), san_path.end());

    std::unordered_set<std::string> overlapping{};
    std::copy(you_path.begin(), you_path.end(), std::inserter(overlapping, overlapping.end()));

    int overlapping_len = overlapping.size();

    // Calculate distance
    int overlap = you_path_len + san_path_len - overlapping_len;
    int best_distance = you_path_len + san_path_len - (overlap * 2);

    std::cout << "Best distance: " << best_distance << std::endl;

    root_planet->dispose();
    return 0;
}
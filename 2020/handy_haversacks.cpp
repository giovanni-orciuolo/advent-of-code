#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_map>

bool starts_with(const std::string& s, const std::string& prefix) {
    return s.rfind(prefix, 0) == 0;
}

int count_holders_recursive(
    const std::vector<std::string>& rules,
    const std::string& holder,
    std::unordered_map<std::string, bool>& visited
) {
    // std::cout << "Holder: " << holder << std::endl;
    int count = 0;
    for (const std::string& rule : rules) {
        std::string new_holder = rule.substr(0, rule.find("contain") - 6);
        if (!starts_with(rule, holder) && rule.find(holder) != std::string::npos && visited.find(new_holder) == visited.end()) {
            // std::cout << "New holder: " << new_holder << std::endl;
            visited[new_holder] = true;

            count++;        
            count += count_holders_recursive(rules, new_holder, visited);
        }
    }
    if (count == 0) {
        return 0;
    }
    return count;
}

int count_bags_recursive(
    const std::vector<std::string>& rules,
    const std::string& bag
) {
    std::regex regex("((\\d+) (\\w+ \\w+))");
    std::smatch match;

    for (const std::string& rule : rules) {
        if (starts_with(rule, bag)) {
            int s = 1; // 1 because I'm counting myself
            std::string::const_iterator search(rule.cbegin());
            while (regex_search(search, rule.cend(), match, regex))
            {
                int coeff = std::stoi(match[2]);
                std::string child_bag = match[3];
                s += coeff * count_bags_recursive(rules, child_bag);
                search = match.suffix().first;
            }
            return s;
        }
    }

    return 0;
}

int main() {
    // Read input
    std::ifstream file("inputs/sacks_rules.txt");
    std::vector<std::string> rules{};
    
    for (std::string rule; std::getline(file, rule);) {
        rules.push_back(rule);
    }

    file.close();

    // Part one
    std::unordered_map<std::string, bool> visited{};
    int holders_shiny_bag = count_holders_recursive(rules, "shiny gold", visited);
    std::cout << "Number of bag colors that can eventually contain at least one shiny gold bag: " << holders_shiny_bag << std::endl;

    // Part two
    int count = count_bags_recursive(rules, "shiny gold");
    std::cout << "Individual bags required inside my shiny gold bag: " << count - 1 << std::endl;

    return 0;
}
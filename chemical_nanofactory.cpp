#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

// Utility functions

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

struct Chemical {
    int quantity;
    std::string name;

    Chemical(): quantity(0), name("") {}
    Chemical(int quantity, const std::string& name): quantity(quantity), name(name) {}

    bool operator==(const Chemical& rhs) const {
        return quantity == rhs.quantity && name.find(rhs.name) != std::string::npos;
    }
};

struct ChemicalHasher {
    int operator()(const Chemical& v) const {
        return ((v.quantity << 1) ^ (v.name.length() << v.quantity)) << 2;
    }
};

// Actual code

int ores_required = 0;

// A chemical output associated with a vector of ingredients
using Recipe = std::pair<Chemical, std::vector<Chemical>>;
using Recipes = std::unordered_map<Chemical, std::vector<Chemical>, ChemicalHasher>;

Recipes read_recipes(const std::string& file_path) {
    std::ifstream file(file_path);
    Recipes recipes{};

    for (std::string recipe; std::getline(file, recipe);) {
        const auto& recipe_split = split(recipe, " => ");
        
        const auto& output_split = split(recipe_split.at(1), " ");
        Chemical chemical_output = { std::stoi(output_split.at(0)), output_split.at(1) };

        const auto& ingredients = split(recipe_split.at(0), ", ");
        std::vector<Chemical> chemical_ingredients{};
        for (auto& i : ingredients) {
            const auto& chemical = split(i, " ");
            chemical_ingredients.push_back({ std::stoi(chemical.at(0)), chemical.at(1) });
        }

        recipes[chemical_output] = chemical_ingredients;
    }

    file.close();
    return recipes;
}

Recipe find_recipe_by_output(const Recipes& recipes, const std::string& output_name) {
    Recipe found_recipe{};

    for (auto& recipe : recipes) {
        if (recipe.first.name.find(output_name) != std::string::npos) {
            found_recipe = recipe;
            break;
        }
    }

    return found_recipe;
}

int find_required_ore(const Recipes& recipes, Chemical start, int depth = 0) {
    Recipe recipe_found = find_recipe_by_output(recipes, start.name);

    auto output = recipe_found.first;
    auto inputs = recipe_found.second;

    if (output.name.empty()) {
        std::cout << "No inputs found for start chemical: " << start.quantity << " " << start.name << std::endl;
        return -1;
    }

    int multiplier = ceil(static_cast<float>(start.quantity) / static_cast<float>(output.quantity));

    if (inputs.size() == 1 && inputs.at(0).name == "ORE") {
        const auto& ore = inputs.at(0);
        if (depth == 0) ores_required += ore.quantity * multiplier;
        return ore.quantity * multiplier;
    }

    for (auto& input : inputs) {
        input.quantity *= multiplier;
        ores_required += find_required_ore(recipes, input, ++depth);
    }
}

int main() {
    const Recipes& chemical_recipes = read_recipes("chemical_recipes.txt");
    const Recipe& fuel_recipe = find_recipe_by_output(chemical_recipes, "FUEL");

    const auto& fuel_inputs = fuel_recipe.second;
    for (auto& input : fuel_inputs) {
        find_required_ore(chemical_recipes, input);
    }

    std::cout << ores_required << std::endl;

    return 0;
}
#include <algorithm>
#include <bitset>
#include <fmt/format.h>
#include <functional>
#include <robin_hood.h>
#include <stack>

#include "advent.hpp"
#include "util.hpp"

struct food {
    std::vector<std::string> ingredients;
    std::vector<std::string> allergens;

    friend std::ostream& operator<<(std::ostream& os, food const& f)
    {
        std::cout << as_map(f.ingredients).transpose() << " : " << as_map(f.allergens).transpose();
        return os;
    }

    bool contains_ingredient(std::string const& ingredient) const
    {
        return std::binary_search(ingredients.begin(), ingredients.end(), ingredient);
    }

    bool contains_allergen(std::string const& allergen) const
    {
        return std::binary_search(allergens.begin(), allergens.end(), allergen);
    }

    void remove_ingredient(std::string const& ingredient)
    {
        ingredients.erase(std::remove(ingredients.begin(), ingredients.end(), ingredient), ingredients.end());
    }

    void remove_allergen(std::string const& allergen)
    {
        allergens.erase(std::remove(allergens.begin(), allergens.end(), allergen), allergens.end());
    }
};

int day21(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print("Provide an input file.\n");
        return 1;
    }

    std::string line;
    std::ifstream in(argv[1]);

    std::vector<food> recipes;
    robin_hood::unordered_set<std::string> ingredients;
    robin_hood::unordered_set<std::string> allergens;

    while (std::getline(in, line)) {
        auto tokens = split(line, ' ');

        bool a = false;
        food f;
        for (auto const& token : tokens) {
            if (token.find("(contains") != std::string::npos) {
                a = true;
                continue;
            }
            if (a) {
                f.allergens.push_back(token.substr(0, token.size() - 1));
                allergens.insert(f.allergens.back());
            } else {
                f.ingredients.push_back(token);
                ingredients.insert(f.ingredients.back());
            }
        }
        std::sort(f.ingredients.begin(), f.ingredients.end());
        std::sort(f.allergens.begin(), f.allergens.end());
        recipes.push_back(f);
    }

    auto n = ingredients.size();
    std::vector<std::pair<std::string, std::string>> list;

    while (ingredients.size() != n - allergens.size()) {
        for (auto const& allergen : allergens) {
            std::vector<std::string> v;

            for (auto const& recipe : recipes) {
                if (!recipe.contains_allergen(allergen)) {
                    continue;
                }
                if (v.empty()) {
                    v = recipe.ingredients;
                } else {
                    v.erase(std::remove_if(v.begin(), v.end(), [&](auto const& x) { return !recipe.contains_ingredient(x); }), v.end());
                }
            }

            if (v.size() == 1) {
                auto const& ingredient = v.front();
                ingredients.erase(ingredient);

                list.push_back({ ingredient, allergen });

                for (auto& recipe : recipes) {
                    recipe.remove_ingredient(ingredient);
                    recipe.remove_allergen(allergen);
                }
            }
        }
    }

    size_t count = 0;
    for (auto const& ingredient : ingredients) {
        count += std::count_if(recipes.begin(), recipes.end(), [&](auto const& recipe) { return recipe.contains_ingredient(ingredient); });
    }
    fmt::print("part 1: {}\n", count);

    std::sort(list.begin(), list.end(), [](auto const& x, auto const& y) { return x.second < y.second; });
    for (auto const& [ingredient, allergen] : list) {
        fmt::print("{},", ingredient);
    }

    return 0;
}

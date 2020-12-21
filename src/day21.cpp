#include <fmt/format.h>
#include <algorithm>
#include <bitset>
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
                if (!std::binary_search(recipe.allergens.begin(), recipe.allergens.end(), allergen)) {
                    continue;
                }

                if (v.empty()) {
                    v = recipe.ingredients;
                } else {
                    v.erase(std::remove_if(v.begin(), v.end(),
                                [&](auto const& x) { 
                                    return !std::binary_search(recipe.ingredients.begin(), recipe.ingredients.end(), x); 
                                }),
                            v.end());
                }
            }

            if (v.size() == 1) {
                ingredients.erase(v.front());

                list.push_back({ v.front(), allergen });

                for (auto& recipe : recipes) {
                    recipe.ingredients.erase(std::remove(recipe.ingredients.begin(), recipe.ingredients.end(), v.front()), recipe.ingredients.end());
                    recipe.allergens.erase(std::remove(recipe.allergens.begin(), recipe.allergens.end(), allergen), recipe.allergens.end());
                }
            }
        }
    }

    size_t count = 0;
    for (auto const& ingredient : ingredients) {
        count += std::count_if(recipes.begin(), recipes.end(),
                [&](auto const& recipe) {
                    return std::binary_search(
                            recipe.ingredients.begin(),
                            recipe.ingredients.end(),
                            ingredient);
                    });
    }
    fmt::print("part 1: {}\n", count);

    std::sort(list.begin(), list.end(), [](auto const& x, auto const& y) { return x.second < y.second; });
    for (auto const& [ingredient, allergen] : list) {
        fmt::print("{},", ingredient);
    }

    return 0;
}

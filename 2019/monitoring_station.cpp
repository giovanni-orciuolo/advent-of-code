#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <array>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <functional>

// Actual code

struct Meteor {
    int x, y, meteors_count;
    Meteor(): x(0), y(0), meteors_count(0) {}
    Meteor(int x, int y, int meteors_count): x(x), y(y), meteors_count(meteors_count) {}
};

int main() {
    const int MAP_WIDTH = 36, MAP_HEIGHT = 36;

    // Fuck data files... all my homies initialize their arrays directly!!!!
    std::array<std::string, MAP_HEIGHT> asteroids_map {
        ".#......#...#.....#..#......#..##..#",
        "..#.......#..........#..##.##.......",
        "##......#.#..#..#..##...#.##.###....",
        "..#........#...........#.......##...",
        ".##.....#.......#........#..#.#.....",
        ".#...#...#.....#.##.......#...#....#",
        "#...#..##....#....#......#..........",
        "....#......#.#.....#..#...#......#..",
        "......###.......#..........#.##.#...",
        "#......#..#.....#..#......#..#..####",
        ".##...##......##..#####.......##....",
        ".....#...#.........#........#....#..",
        "....##.....#...#........#.##..#....#",
        "....#........#.###.#........#...#..#",
        "....#..#.#.##....#.........#.....#.#",
        "##....###....##..#..#........#......",
        ".....#.#.........#.......#....#....#",
        ".###.....#....#.#......#...##.##....",
        "...##...##....##.........#...#......",
        ".....#....##....#..#.#.#...##.#...#.",
        "#...#.#.#.#..##.#...#..#..#..#......",
        "......#...#...#.#.....#.#.....#.####",
        "..........#..................#.#.##.",
        "....#....#....#...#..#....#.....#...",
        ".#####..####........#...............",
        "#....#.#..#..#....##......#...#.....",
        "...####....#..#......#.#...##.....#.",
        "..##....#.###.##.#.##.#.....#......#",
        "....#.####...#......###.....##......",
        ".#.....#....#......#..#..#.#..#.....",
        "..#.......#...#........#.##...#.....",
        "#.....####.#..........#.#.......#...",
        "..##..#..#.....#.#.........#..#.#.##",
        ".........#..........##.#.##.......##",
        "#..#.....#....#....#.#.......####..#",
        "..............#.#...........##.#.#.."
    };

    const auto for_each_meteor = [asteroids_map](int excluded_x, int excluded_y, std::function<void(int, int)> callback) -> void {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                if ((x == excluded_x && y == excluded_y) || asteroids_map[y][x] != '#') {
                    continue;
                }
                callback(x, y);
            }
        }
    };

    // PART 1: Scanning visible meteors

    std::vector<Meteor> meteors{};

    const auto scan_visible_meteors = [asteroids_map, for_each_meteor](int meteor_x, int meteor_y) -> int {
        std::set<float> angles{};

        for_each_meteor(meteor_x, meteor_y, [&](int x, int y) {
            angles.insert(atan2(y - meteor_y, x - meteor_x));
        });

        return angles.size();
    };

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (asteroids_map[y][x] != '#') {
                continue;
            }

            meteors.push_back(Meteor(x, y, scan_visible_meteors(x, y)));
        }
    }

    Meteor best_meteor;
    int max_count = 0;

    // Couldn't make std::max_element work for some reason...
    for (const auto& m : meteors) {
        if (m.meteors_count > max_count) {
            max_count = m.meteors_count;
            best_meteor = m;
        }
    }

    std::cout << "Best meteor is located at (" << best_meteor.x << "," << best_meteor.y << ") and has " << best_meteor.meteors_count << " visible meteors! Epic..." << std::endl;

    // PART 2: Destroying meteors

    std::unordered_map<float, std::vector<int>> scanned_angles;

    for_each_meteor(best_meteor.x, best_meteor.y, [&](int x, int y) {
        float angle = atan2(best_meteor.x - x, best_meteor.y - y);
        angle = angle <= 0 ? abs(angle) : M_PI * 2 - angle;
        scanned_angles[angle].push_back(x * 100 + y);
    });

    for (auto& angle : scanned_angles) {
        if (angle.second.size() < 2) {
            continue;
        }

        std::sort(angle.second.begin(), angle.second.end(), [best_meteor](int a, int b) {
            return (sqrt(pow(b / 100 - best_meteor.x, 2) + pow(b % 100 - best_meteor.y, 2))) < (sqrt(pow(a / 100 - best_meteor.x, 2) + pow(a % 100 - best_meteor.y, 2)));
        });
    }

    std::vector<int> destroyed{};

    while (destroyed.size() < 200) {
        for (auto& angle : scanned_angles) {
            if (angle.second.empty()) {
                continue;
            }

            destroyed.push_back(angle.second.back());
            angle.second.pop_back();
        }
    }

    std::cout << "200 meteors have been destroyed! The 200th destroyed meteor data is: " << destroyed[199] << std::endl;

    return 0;
}
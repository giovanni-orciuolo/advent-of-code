#include <iostream>
#include <fstream>
#include <cmath>

// Actual code

const int MAP_WIDTH = 5, MAP_HEIGHT = 5;
char asteroids_map[MAP_HEIGHT][MAP_WIDTH] {};

struct Point {
    int x;
    int y;
    Point(int x, int y): x(x), y(y) {}
};

void inspect_quadrant(const Point& cur_pos, int step_x, int step_y, int& num_asteroids, int pivot_x, int pivot_y) {
    if (asteroids_map[cur_pos.y][cur_pos.x] == '#') {
        num_asteroids++;
        return;
    }

    if (cur_pos.y + step_y > MAP_HEIGHT || cur_pos.x + step_x > MAP_WIDTH) {
        return;
    }

    inspect_quadrant(
        Point(cur_pos.x + step_x, cur_pos.y + step_y),
        step_y == pivot_y ? pivot_x : --step_x,
        step_x == pivot_x ? pivot_y : ++step_y,
        num_asteroids,
        pivot_x,
        pivot_y
    );
}

int count_visible_asteroids(const Point& cur_pos) {
    int num_asteroids = 0;

    inspect_quadrant(cur_pos, abs((cur_pos.x - 1) - MAP_WIDTH), 1,   num_asteroids,  1,  1); // Bottom right quadrant
    inspect_quadrant(cur_pos, -1, abs((cur_pos.y - 1) - MAP_HEIGHT), num_asteroids, -1,  1); // Bottom left quadrant
    inspect_quadrant(cur_pos, -abs((cur_pos.x - 1) - MAP_WIDTH), -1, num_asteroids, -1, -1); // Top left quadrant
    inspect_quadrant(cur_pos, abs((cur_pos.x - 1) - MAP_WIDTH), 1,   num_asteroids,  1, -1); // Top right quadrant

    return num_asteroids;
}

int main() {
    std::ifstream map_file("asteroids_map.txt");

    int map_rows = 0, map_cols = 0;
    for (std::string map_row; getline(map_file, map_row);) {
        for (auto m : map_row) {
            asteroids_map[map_cols][map_rows] = m;
            map_cols++;
        }
        map_rows++;
    }

    std::cout << count_visible_asteroids(Point(3,4)) << std::endl;

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

typedef std::vector<std::vector<char>> seats_matrix;

const char SEAT_EMPTY = 'L', SEAT_OCCUPIED = '#', NO_SEAT = '.';

void print_grid(const seats_matrix& grid) {
    for (auto& row : grid) {
        for (auto& seat : row) {
            std::cout << seat;
        }
        std::cout << std::endl;
    }
}

// Returns true if the grids are the same
bool compare_grids(const seats_matrix& grid, const seats_matrix& other_grid) {
    bool equal = true;
    for (int i = 0; i < grid.size(); ++i) {
        for (int k = 0; k < grid[i].size(); ++k) {
            if (grid[i][k] != other_grid[i][k]) {
                equal = false;
                break;
            }
        }
        if (!equal) {
            break;
        }
    }
    return equal;
}

// Returns the count of occupied seats on a grid
int count_occupied_seats(const seats_matrix& grid) {
    int count = 0;
    for (auto& row : grid) {
        for (auto& seat : row) {
            if (seat == SEAT_OCCUPIED) {
                count++;
            }
        }
    }
    return count;
}

// Returns the new grid
seats_matrix assign_seats(const seats_matrix& grid) {
    seats_matrix new_grid = grid; // Clone the original grid
    for (int i = 0; i < grid.size(); ++i) {
        for (int k = 0; k < grid[i].size(); ++k) {
            char seat = grid[i][k];

            // If a seat is empty (L) and there are no occupied seats adjacent to it, the seat becomes occupied
            // If a seat is occupied (#) and four or more seats adjacent to it are also occupied, the seat becomes empty.
            // Otherwise, the seat's state does not change.
            switch (seat) {
                case SEAT_EMPTY:
                {
                    bool found_occupied = false;
                    for (int _i = -1; _i < 2; ++_i) {
                        for (int _k = -1; _k < 2; ++_k) {
                            if (
                                (i + _i >= 0 && i + _i < grid.size() && k + _k >= 0 && k + _k < grid[i].size())
                                && grid[i + _i][k + _k] == SEAT_OCCUPIED
                            ) {
                                found_occupied = true;
                                break;
                            }
                        }
                        if (found_occupied) {
                            break;
                        }
                    }
                    if (!found_occupied) {
                        new_grid[i][k] = SEAT_OCCUPIED;
                    }
                }
                break;
                case SEAT_OCCUPIED:
                {
                    int count_occupied = 0;
                    for (int _i = -1; _i < 2; ++_i) {
                        for (int _k = -1; _k < 2; ++_k) {
                            if (
                                (i + _i >= 0 && i + _i < grid.size() && k + _k >= 0 && k + _k < grid[i].size())
                                && !(_i == 0 && _k == 0)
                                && grid[i + _i][k + _k] == SEAT_OCCUPIED
                            ) {
                                count_occupied++;
                            }
                        }
                    }
                    if (count_occupied >= 4) {
                        new_grid[i][k] = SEAT_EMPTY;
                    }
                }
                case NO_SEAT: break;
            }
        }
    }
    return new_grid;
}

int main() {
    // Read input
    std::ifstream file("inputs/seats_grid.txt");
    std::vector<std::string> seats_lines{};
    
    for (std::string s; std::getline(file, s);) {
        seats_lines.push_back(s);
    }

    file.close();

    // Create seats matrix
    seats_matrix seats_grid(seats_lines[0].length(), std::vector<char>(seats_lines.size(), 0));
    for (int i = 0; i < seats_lines.size(); ++i) {
        for (int k = 0; k < seats_lines[i].length(); ++k) {
            seats_grid[i][k] = seats_lines[i][k];
        }
    }

    // Part one 2415 too low
    seats_matrix current_grid = seats_grid, new_grid = assign_seats(current_grid);
    while (true) {
        new_grid = assign_seats(current_grid);
        if (compare_grids(current_grid, new_grid)) {
            break;
        }
        current_grid = new_grid;
        print_grid(current_grid);
        std::cin.get();
    }

    std::cout << "Grid reached stability, occupied seats: " << count_occupied_seats(current_grid) << std::endl;

    return 0;
}
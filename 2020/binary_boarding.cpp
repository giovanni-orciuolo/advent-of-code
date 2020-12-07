#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

struct Seat {
    int row;
    int column;
    int id;
    Seat(int _row, int _col, int _id):
        row(_row),
        column(_col),
        id(_id)
    {}

    void print_info() const {
        std::cout << "Seat # ROW: " << row << " COL: " << column << " | ID: " << id << std::endl; 
    }
};

int decode(const std::string& input, int low, int high, char pick_low) {
    if (input.length() == 1) {
        return input[0] == pick_low ? low : high;
    }
    int partition = std::floor(low + ((high - low) / 2));
    return input[0] == pick_low
        ? decode(input.substr(1), low, partition, pick_low)
        : decode(input.substr(1), partition + 1, high, pick_low);
}
Seat decode_seat(const std::string& seat) {
    int row = decode(seat.substr(0, 7), 0, 127, 'F'), col = decode(seat.substr(7, 10), 0, 7, 'L');
    return Seat(row, col, row * 8 + col);
}

int main() {
    // Read input
    std::ifstream file("seats.txt");
    std::vector<std::string> seats{};
    
    for (std::string line; std::getline(file, line);) {
        seats.push_back(line);
    }

    file.close();

    // Part one

    int max_seat_id = -1;
    std::vector<Seat> decoded_seats{}; // Useful for part 2

    for (const std::string& s : seats) {
        Seat seat = decode_seat(s);
        if (seat.id > max_seat_id) {
            max_seat_id = seat.id;
        }
        decoded_seats.push_back(seat);
    }

    std::cout << "Max seat id: " << max_seat_id << std::endl;

    // Part two

    std::sort(decoded_seats.begin(), decoded_seats.end(), [](const Seat& a, const Seat& b) { return a.id < b.id; });
    int last_id = 0;
    for (const Seat& seat : decoded_seats) {
        if (last_id != 0 && (seat.id - last_id) != 1) {
            std::cout << "MISSING ID BETWEEN " << seat.id << " AND " << last_id << "!!!" << std::endl;
            break;
        }
        last_id = seat.id;
    }

    return 0;
}
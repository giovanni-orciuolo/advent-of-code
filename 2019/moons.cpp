#include <iostream>
#include <array>
#include <cmath>
#include <vector>
#include <unordered_set>

// Utility functions

uint64_t gcd(uint64_t a, uint64_t b)
{
    if (b == 0) {
        return a;
    }
 
    return gcd(b, a % b);
}

uint64_t lcm(uint64_t a, uint64_t b)
{
    return a * b / gcd(a, b);
}

// Actual code

struct Vector3 {
    int x, y, z;
    
    Vector3(): x(0), y(0), z(0) {}
    Vector3(int x, int y, int z): x(x), y(y), z(z) {}

    std::string toString() const {
        return "<x = " + std::to_string(x) + ", y = " + std::to_string(y) + ", z = " + std::to_string(z) + ">";
    }

    bool isZero() {
        return x == 0 && y == 0 && z == 0;
    }

    bool operator==(const Vector3& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator!=(const Vector3& rhs) const {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }
    int operator[](int i) const {
        switch (i) {
            case 0: return x; break;
            case 1: return y; break;
            case 2: return z; break;
        }
    }
};

struct Moon {
    Vector3 pos, vel;
    std::pair<Vector3, Vector3> initial;

    Moon() {
        initial = { pos, vel };
    }
    Moon(int x, int y, int z): pos(x, y, z) {
        initial = { pos, vel };
    }

    void applyGravity(const Moon& other) {
        vel.x += other.pos.x == pos.x ? 0 : (other.pos.x > pos.x ? 1 : -1);
        vel.y += other.pos.y == pos.y ? 0 : (other.pos.y > pos.y ? 1 : -1);
        vel.z += other.pos.z == pos.z ? 0 : (other.pos.z > pos.z ? 1 : -1);
    }
    void calculatePosition() {
        pos.x += vel.x;
        pos.y += vel.y;
        pos.z += vel.z;
    }

    int getPotentialEnergy() const {
        return abs(pos.x) + abs(pos.y) + abs(pos.z);
    }
    int getKineticEnergy() const {
        return abs(vel.x) + abs(vel.y) + abs(vel.z);
    }
    int getTotalEnergy() const {
        return getPotentialEnergy() * getKineticEnergy();
    }

    bool isCircular(int index) const {
        return pos[index] == initial.first[index] && vel[index] == initial.second[index];
    }

    std::string info() const {
        return "pos=" + pos.toString() + ", vel=" + vel.toString();
    }

    bool operator==(const Moon& rhs) const {
        return pos == rhs.pos && vel == rhs.vel;
    }
};

int main() {
    // Simulate gravity and velocity for N steps
    const int STEPS = 1000;

    // Puzzle input
    std::array<Moon, 4> initial_moons = {
        Moon(-1, 7, 3),
        Moon(12, 2, -13),
        Moon(14, 18, -8),
        Moon(17, 4, -4)
    };

    std::array<Moon, 4> moons = initial_moons;
    int periods[3] = { -1, -1, -1 };

    for (int t = 0;; ++t) {

        if (t == STEPS) {
            std::cout << "Reached " << STEPS << " steps!" << std::endl;

            // Calculate the total energy of the system
            int total_energy = 0;

            for (const auto& moon : moons) {
                total_energy += moon.getTotalEnergy();
            }

            std::cout << "Total energy of the system: " << total_energy << std::endl;
        }

        for (auto& moon : moons) {
            for (auto& other : moons) {
                moon.applyGravity(other);
            }
        }

        for (auto& moon : moons) {
            moon.calculatePosition();
        }

        for (int dim = 0; dim < 3; ++dim) {
            if (periods[dim] != -1) {
                continue;
            }

            bool periodic = true;

            for (int i = 0; i < moons.size(); ++i) {
                periodic &= moons[i].isCircular(dim);
            }

            if (periodic) {
                periods[dim] = t + 1;
            }
        }

        if (periods[0] != -1 && periods[1] != -1 && periods[2] != -1 && t >= STEPS) {
            // Found all the periods, time to apply LCM
            break;
        }

    }

    std::cout << "Reached a periodic point after " << lcm(lcm(periods[0], periods[1]), periods[2]) << " steps!" << std::endl;

}
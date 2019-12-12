#include <iostream>
#include <array>
#include <cmath>

struct Vector3 {
    int x, y, z;
    
    Vector3(): x(0), y(0), z(0) {}
    Vector3(int x, int y, int z): x(x), y(y), z(z) {}

    std::string toString() const {
        return "<x = " + std::to_string(x) + ", y = " + std::to_string(y) + ", z = " + std::to_string(z) + ">";
    }
};

struct Moon {
    Vector3 pos, vel;

    Moon(int x, int y, int z): pos(x, y, z) {}

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

    std::string info() const {
        return "pos=" + pos.toString() + ", vel=" + vel.toString();
    }
};

int main() {
    std::array<Moon, 4> moons {
        Moon(-1, 7, 3),
        Moon(12, 2, -13),
        Moon(14, 18, -8),
        Moon(17, 4, -4)
    };

    // Simulate gravity and velocity for N steps
    const int STEPS = 1000;

    for (int t = 0; t <= STEPS; ++t) {
        std::cout << "After " << t << " steps:" << std::endl;
        for (const auto& moon : moons) {
            std::cout << moon.info() << std::endl;
        }

        if (t == STEPS) {
            // Don't simulate again!
            break;
        }

        for (auto& moon : moons) {
            for (auto& other : moons) {
                moon.applyGravity(other);
            }
        }

        for (auto& moon : moons) {
            moon.calculatePosition();
        }
    }

    // Calculate the total energy of the system
    int total_energy = 0;

    for (const auto& moon : moons) {
        total_energy += moon.getTotalEnergy();
    }

    std::cout << "Total energy of the system: " << total_energy << std::endl;
}
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#define EPS 1e-7

struct Vector2 {
    double x, y;
    Vector2() {}
    Vector2(double x, double y) : x(x), y(y) {}
    Vector2(std::string text) {
        auto first_comma = text.find_first_of(',');
        auto last_comma = text.find_last_of(',');

        this->x = std::stod(text.substr(0, first_comma));
        this->y = std::stod(text.substr(first_comma + 1, last_comma - first_comma));
    }
};

struct Hailstone {
    Vector2 position, velocity;
    Hailstone(std::string line) {
        auto at_index = line.find('@');
        auto position = line.substr(0, at_index);
        auto velocity = line.substr(at_index + 1, line.length() - 1 - at_index);

        this->position = Vector2(position);
        this->velocity = Vector2(velocity);
    }
};

std::vector<Hailstone> read_hailstones(const char* filename) {
    std::vector<Hailstone> stones;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            stones.push_back(Hailstone(line));
        }
        file.close();
    }
    return stones;
}

double vec2_determinant(const Vector2& a, const Vector2& b) { return a.x * b.y - a.y * b.x; }

bool are_parallel(const Vector2& a, const Vector2& b) {
    auto det = vec2_determinant(a, b);
    return std::abs(det) < EPS;
}

bool is_on_line(const Vector2& start, const Vector2& dir, const Vector2& point) {
    // start + t * dir = point
    if (std::abs(dir.x) < EPS) {
        return std::abs(point.x - start.x) < EPS;
    }

    auto t = (point.x - start.x) / dir.x;
    return std::abs(point.y - (start.y + t * dir.y)) < EPS;
}

Vector2 intersection(const Vector2& a, const Vector2& a_dir, const Vector2& b, const Vector2& b_dir) {
    // a + m * a_dir = b + n * b_dir
    // b_x - a_x = a_dir_x * m - b_dir_x * n
    // b_y - a_y = a_dir_y * m - b_dir_y * n
    // j = A * p
    // p = inv(A) * j

    auto j_x = b.x - a.x;
    auto j_y = b.y - a.y;

    auto det_A = -a_dir.x * b_dir.y + a_dir.y * b_dir.x;

    auto inv_A_0_0 = -b_dir.y / det_A;
    auto inv_A_0_1 = b_dir.x / det_A;
    auto inv_A_1_0 = -a_dir.y / det_A;
    auto inv_A_1_1 = a_dir.x / det_A;

    auto m = inv_A_0_0 * j_x + inv_A_0_1 * j_y;
    auto n = inv_A_1_0 * j_x + inv_A_1_1 * j_y;

    return Vector2(m, n);
}

bool intersect(const Hailstone& a, const Hailstone& b, double lower, double upper) {
    if (are_parallel(a.velocity, b.velocity)) {
        return is_on_line(a.position, a.velocity, b.position);
    }

    auto p = intersection(a.position, a.velocity, b.position, b.velocity);
    if (p.x < 0 || p.y < 0) {
        return false;
    }

    Vector2 point(a.position.x + p.x * a.velocity.x, a.position.y + p.x * a.velocity.y);
    return lower <= point.x && point.x <= upper && lower <= point.y && point.y <= upper;
}

int main(int argc, const char** argv) {
    auto stones = read_hailstones(argv[1]);

    std::size_t part_1 = 0;
    for (std::size_t i = 0; i < stones.size() - 1; ++i) {
        for (std::size_t j = i + 1; j < stones.size(); ++j) {
            if (intersect(stones[i], stones[j], 200000000000000, 400000000000000)) {
                ++part_1;
            }
        }
    }

    std::cout << "Part 1: " << part_1 << "\n";

    return 0;
}
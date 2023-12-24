#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

struct Vector3 {
    int x, y, z;
    Vector3() {}
    Vector3(std::string text) {
        auto first_comma = text.find_first_of(',');
        auto last_comma = text.find_last_of(',');

        this->x = std::stoi(text.substr(0, first_comma));
        this->y = std::stoi(text.substr(first_comma + 1, last_comma - first_comma));
        this->z = std::stoi(text.substr(last_comma + 1, text.length() - 1 - last_comma));
    }
};

struct Brick {
    Vector3 start, end;

    Brick(const std::string line) {
        auto tilde_index = line.find('~');
        auto start = line.substr(0, tilde_index);
        auto end = line.substr(tilde_index + 1, line.length() - 1 - tilde_index);

        this->start = Vector3(start);
        this->end = Vector3(end);
    }

    int get_lowest_x() const { return std::min(start.x, end.x); }

    int get_highest_x() const { return std::max(start.x, end.x); }

    int get_lowest_y() const { return std::min(start.y, end.y); }

    int get_highest_y() const { return std::max(start.y, end.y); }

    int get_lowest_z() const { return std::min(start.z, end.z); }

    int get_highest_z() const { return std::max(start.z, end.z); }

    bool overlaps_with(const Brick& other) const {
        auto x_overlap =
            std::max(this->get_lowest_x(), other.get_lowest_x()) <= std::min(this->get_highest_x(), other.get_highest_x());
        auto y_overlap =
            std::max(this->get_lowest_y(), other.get_lowest_y()) <= std::min(this->get_highest_y(), other.get_highest_y());

        return x_overlap && y_overlap;
    }
};

struct SupportMap {
    std::vector<std::vector<std::size_t>> supports, supported_by;

    SupportMap(const std::vector<Brick>& bricks) {
        for (std::size_t i = 0; i < bricks.size(); ++i) {
            supports.push_back(std::vector<std::size_t>());
            supported_by.push_back(std::vector<std::size_t>());
        }

        for (std::size_t i = 0; i < bricks.size(); ++i) {
            for (std::size_t i_below = 0; i_below < i; ++i_below) {
                if (bricks[i].get_lowest_z() == 1 + bricks[i_below].get_highest_z() &&
                    bricks[i].overlaps_with(bricks[i_below])) {
                    supports[i_below].push_back(i);
                    supported_by[i].push_back(i_below);
                }
            }
        }
    }
};

bool brick_z_comparer(const Brick& lhs, const Brick& rhs) { return lhs.get_lowest_z() < rhs.get_lowest_z(); }

void drop_bricks(std::vector<Brick>& bricks) {
    for (std::size_t i = 0; i < bricks.size(); ++i) {
        int z = 1;
        for (std::size_t i_below = 0; i_below < i; ++i_below) {
            if (bricks[i].overlaps_with(bricks[i_below])) {
                z = std::max(z, bricks[i_below].get_highest_z() + 1);
            }
        }
        int fall_amout = bricks[i].get_lowest_z() - z;
        bricks[i].start.z -= fall_amout;
        bricks[i].end.z -= fall_amout;
    }
}

std::size_t part_1(const SupportMap& map) {
    std::size_t result = 0;
    for (std::size_t i = 0; i < map.supports.size(); ++i) {
        bool can_be_removed = true;
        for (const auto& supported : map.supports[i]) {
            if (map.supported_by[supported].size() < 2) {
                can_be_removed = false;
                break;
            }
        }
        if (can_be_removed) {
            ++result;
        }
    }
    return result;
}

std::size_t count_brick_falls(const SupportMap& map, std::size_t brick) {
    std::deque<std::size_t> queue;
    std::unordered_set<std::size_t> falling;
    queue.push_back(brick);
    falling.insert(brick);

    while (queue.size() > 0) {
        auto current = queue.front();
        queue.pop_front();

        for (auto supported : map.supports[current]) {
            if (falling.count(supported)) {
                continue;
            }

            bool all_supports_are_falling = true;
            for (auto supports : map.supported_by[supported]) {
                if (!falling.count(supports)) {
                    all_supports_are_falling = false;
                    break;
                }
            }

            if (all_supports_are_falling) {
                queue.push_back(supported);
                falling.insert(supported);
            }
        }
    }

    return falling.size() - 1;
}

std::size_t part_2(const SupportMap& map) {
    std::size_t result = 0;
    for (std::size_t i = 0; i < map.supports.size(); ++i) {
        result += count_brick_falls(map, i);
    }
    return result;
}

int main(int argc, const char** argv) {
    std::vector<Brick> bricks;
    std::ifstream file(argv[1]);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            bricks.push_back(Brick(line));
        }
        file.close();
    }

    std::sort(bricks.begin(), bricks.end(), brick_z_comparer);
    drop_bricks(bricks);
    std::sort(bricks.begin(), bricks.end(), brick_z_comparer);
    SupportMap map(bricks);

    std::cout << "Part 1: " << part_1(map) << "\n";
    std::cout << "Part 2: " << part_2(map) << "\n";

    return 0;
}
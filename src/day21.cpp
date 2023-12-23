#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

int DR[] = {1, -1, 0, 0};
int DC[] = {0, 0, 1, -1};

using Map = std::vector<std::string>;

struct Position {
    int row, col;
    Position(int row, int col) : row(row), col(col) {}
    bool operator==(const Position& pos) const { return this->row == pos.row && this->col == pos.col; }
};

class PositionHash {
   public:
    std::size_t operator()(const Position& pos) const { return pos.row * 10000 + pos.col; }
};

using PositionSet = std::unordered_set<Position, PositionHash>;

struct PositionState {
    Position pos;
    std::size_t steps;
    PositionState(int row, int col, std::size_t steps) : pos(row, col), steps(steps) {}
};

Map read_map(const char* filename) {
    Map map;

    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            map.push_back(line);
        }
        file.close();
    }

    return map;
}

Position find_start(const Map& map) {
    for (int row = 0; row < map.size(); ++row) {
        for (auto col = 0; col < map[row].length(); ++col) {
            if (map[row][col] == 'S') {
                return Position(row, col);
            }
        }
    }
    assert(false);
}

void assert_input_properties(const Map& map, const Position& pos, std::size_t steps) {
    auto rows = map.size();
    auto cols = map[0].length();

    assert(rows == cols);
    assert(pos.row == rows / 2);
    assert(pos.col == cols / 2);

    assert(steps % rows == rows / 2);
}
bool is_valid_pos(const Map& map, const Position& pos) {
    return 0 <= pos.row && pos.row < map.size() && 0 <= pos.col && pos.col < map[pos.row].length() &&
           map[pos.row][pos.col] != '#';
}

std::size_t count_fields(const Map& map, int row, int col, int steps) {
    std::deque<PositionState> queue;
    PositionSet seen;
    PositionSet answer;
    ;
    queue.push_back(PositionState(row, col, steps));

    while (queue.size() > 0) {
        auto current = queue.front();
        queue.pop_front();

        if (current.steps % 2 == 0) {
            answer.insert(current.pos);
        }

        if (current.steps == 0) {
            continue;
        }

        for (int iDelta = 0; iDelta < 4; ++iDelta) {
            int row = current.pos.row + DR[iDelta];
            int col = current.pos.col + DC[iDelta];
            Position pos(row, col);

            if (!is_valid_pos(map, pos) || seen.count(pos)) {
                continue;
            }

            seen.insert(pos);
            queue.push_back(PositionState(row, col, current.steps - 1));
        }
    }
    return answer.size();
}

std::size_t square(std::size_t x) { return x * x; }

int main(int argc, char** argv) {
    auto map = read_map(argv[1]);
    auto start = find_start(map);

    std::cout << "Part 1: " << count_fields(map, start.row, start.col, 64) << "\n";

    std::size_t steps = 26501365;
    assert_input_properties(map, start, steps);
    auto map_size = map.size();
    auto grid_radius = steps / map_size - 1;

    auto even_maps = square((grid_radius + 1) / 2 * 2);
    auto odd_maps = square(grid_radius / 2 * 2 + 1);

    // count the fully reachable maps
    std::size_t part_2 = 0;
    part_2 += even_maps * count_fields(map, start.row, start.col, map_size * 2);    // Some big enough even number
    part_2 += odd_maps * count_fields(map, start.row, start.col, map_size * 2 + 1); // Some big enough odd number

    std::cout << "Part 2: " << part_2 << "\n";

    // Count the four corner maps in the grid of maps
    part_2 += count_fields(map, map_size - 1, start.col, map_size - 1);
    part_2 += count_fields(map, start.row, 0, map_size - 1);
    part_2 += count_fields(map, 0, start.col, map_size - 1);
    part_2 += count_fields(map, start.row, map_size - 1, map_size - 1);

    std::cout << "Part 2: " << part_2 << "\n";

    // Count the outer edge maps
    part_2 += (grid_radius + 1) * count_fields(map, map_size - 1, 0, map_size / 2 - 1);
    part_2 += (grid_radius + 1) * count_fields(map, 0, 0, map_size / 2 - 1);
    part_2 += (grid_radius + 1) * count_fields(map, 0, map_size - 1, map_size / 2 - 1);
    part_2 += (grid_radius + 1) * count_fields(map, map_size - 1, map_size - 1, map_size / 2 - 1);

    std::cout << "Part 2: " << part_2 << "\n";
    // Coount the inner edge maps
    part_2 += grid_radius * count_fields(map, map_size - 1, 0, map_size * 3 / 2 - 1);
    part_2 += grid_radius * count_fields(map, 0, 0, map_size * 3 / 2 - 1);
    part_2 += grid_radius * count_fields(map, 0, map_size - 1, map_size * 3 / 2 - 1);
    part_2 += grid_radius * count_fields(map, map_size - 1, map_size - 1, map_size * 3 / 2 - 1);

    std::cout << "Part 2: " << part_2 << "\n";

    return 0;
}
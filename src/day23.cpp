#include <algorithm>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

const int DR[] = {-1, 1, 0, 0};
const int DC[] = {0, 0, -1, 1};

using Map = std::vector<std::string>;

struct Position {
    int row, col;
    Position(int row, int col) : row(row), col(col) {}
    bool operator==(const Position& other) const { return this->row == other.row && this->col == other.col; }
};

class PositionHash {
   public:
    std::size_t operator()(const Position& pos) const { return 1000 * pos.row + pos.col; }
};

using Edges = std::unordered_map<Position, std::size_t, PositionHash>;
using Graph = std::unordered_map<Position, Edges, PositionHash>;

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

int find_first_path_field(const std::string& line) {
    for (int i = 0; i < line.length(); ++i) {
        if (line[i] == '.') {
            return i;
        }
    }
    assert(false);
}

bool is_legal_waypoint(const Map& map, const Position& pos) {
    auto rows = map.size();
    auto cols = map[0].length();

    return pos.row >= 0 && pos.row < rows && pos.col >= 0 && pos.col < cols && map[pos.row][pos.col] != '#';
}

void collect_branch_positions(const Map& map, std::vector<Position>& vertices) {
    auto rows = map.size();
    auto cols = map[0].length();
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (map[row][col] == '#') {
                continue;
            }

            int num_neighbors = 0;
            for (int i_delta = 0; i_delta < 4; ++i_delta) {
                int n_row = row + DR[i_delta];
                int n_col = col + DC[i_delta];

                if (is_legal_waypoint(map, Position(n_row, n_col))) {
                    ++num_neighbors;
                }
            }

            if (num_neighbors > 2) {
                vertices.push_back(Position(row, col));
            }
        }
    }
}

bool contains(const std::vector<Position>& positions, const Position& pos) {
    for (const auto& current : positions) {
        if (current == pos) {
            return true;
        }
    }
    return false;
}

std::vector<Position> get_legal_deltas(char field, bool is_part2) {
    std::vector<Position> positions;

    if (field == '^' || field == '.' || is_part2) {
        positions.push_back(Position(-1, 0));
    }
    if (field == 'v' || field == '.' || is_part2) {
        positions.push_back(Position(1, 0));
    }
    if (field == '<' || field == '.' || is_part2) {
        positions.push_back(Position(0, -1));
    }
    if (field == '>' || field == '.' || is_part2) {
        positions.push_back(Position(0, 1));
    }

    return positions;
}

Graph build_densed_graph(const Map& map, const std::vector<Position>& vertices, bool is_part2) {
    Graph graph;

    for (const auto& vertex : vertices) {
        PositionSet seen;
        std::vector<PositionState> stack;
        Edges edges;

        seen.insert(vertex);
        stack.push_back(PositionState(vertex.row, vertex.col, 0));

        while (stack.size() > 0) {
            auto current = stack.back();
            stack.pop_back();

            if (current.steps != 0 && contains(vertices, current.pos)) {
                edges.insert({current.pos, current.steps});
                continue;
            }

            for (const auto& delta : get_legal_deltas(map[current.pos.row][current.pos.col], is_part2)) {
                int new_row = current.pos.row + delta.row;
                int new_col = current.pos.col + delta.col;

                Position next(new_row, new_col);
                if (is_legal_waypoint(map, next) && !seen.count(next)) {
                    stack.push_back(PositionState(new_row, new_col, current.steps + 1));
                    seen.insert(next);
                }
            }
        }

        graph.insert({vertex, edges});
    }

    return graph;
}

std::size_t find_longest_path_rec(const Graph& graph, const Position& current, PositionSet& seen, const Position& end,
                                  bool& valid) {
    if (current == end) {
        valid = true;
        return 0;
    }

    std::size_t result = 0;
    valid = false;
    for (const auto& edge : graph.at(current)) {
        auto& cost = edge.second;
        auto& target = edge.first;

        if (seen.count(target)) {
            continue;
        }

        bool target_valid = false;
        seen.insert(target);
        auto target_result = find_longest_path_rec(graph, target, seen, end, target_valid);
        seen.erase(target);

        if (target_valid) {
            valid = true;
            result = std::max(result, cost + target_result);
        }
    }
    return result;
}

std::size_t find_longest_path(const Graph& graph, const Position& start, const Position& end) {
    PositionSet seen;
    seen.insert(start);

    bool valid = false;
    auto result = find_longest_path_rec(graph, start, seen, end, valid);
    assert(valid);
    return result;
}

int main(int argc, const char** argv) {
    auto map = read_map(argv[1]);
    Position start(0, find_first_path_field(map[0]));
    Position end(map.size() - 1, find_first_path_field(map[map.size() - 1]));

    std::vector<Position> vertices;
    vertices.push_back(start);
    vertices.push_back(end);
    collect_branch_positions(map, vertices);

    auto graph1 = build_densed_graph(map, vertices, false);
    std::cout << "Part 1: " << find_longest_path(graph1, start, end) << "\n";

    auto graph2 = build_densed_graph(map, vertices, true);
    std::cout << "Part 2: " << find_longest_path(graph2, start, end) << "\n";

    return 0;
}
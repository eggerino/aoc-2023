import functools
import math
import sys

content = open(sys.argv[1]).read().strip()
lines = content.split("\n")

instructions = [*lines[0]]
graph = {}
for line in lines[2:]:
    key, value = line.split(" = ")
    left, right = value.replace("(", "").replace(")", "").split(", ")
    graph[key] = left, right


def solve(start_node, goal_condition):
    num_steps = 0
    node = start_node
    while not goal_condition(node):
        inst = instructions[num_steps % len(instructions)]
        left, right = graph[node]
        node = left if inst == "L" else right
        num_steps += 1
    return num_steps


def lcm(a, b):
    return a * b // math.gcd(a, b)


part_1 = solve("AAA", lambda x: x == "ZZZ")
print(f"Part 1: {part_1}")

# Part 2: solve each path separatly and abuse periodicity
# Every path in test and actual data set gets to a goal location after every n steps
# Lowest common multiple of all paths' periodic step count is the solution
nums_to_z = (solve(n, lambda x: x.endswith("Z"))
             for n in graph.keys() if n.endswith("A"))
part_2 = functools.reduce(lcm, nums_to_z)
print(f"Part 2: {part_2}")

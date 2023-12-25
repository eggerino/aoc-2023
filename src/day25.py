from collections import deque
import sys

graph = {}
for line in open(sys.argv[1]).read().splitlines():
    source, node = line.split(": ")
    for target in node.strip().split():
        graph[source] = [*graph.get(source, []), target]
        graph[target] = [*graph.get(target, []), source]

nodes = list(graph.keys())


def get_shortest_path(graph, start, end):
    queue = deque([(start, [])])
    seen = set()

    while queue:
        current, path = queue.popleft()

        if current == end:
            return path

        if current in seen:
            continue

        seen.add(current)

        for target in graph[current]:
            queue.append((target, [*path, (current, target)]))


count = {}
for i, start in enumerate(nodes):
    if len(nodes) > 20:
        if i == 10:
            break

    for end in nodes:
        if node == end:
            continue

        h = get_shortest_path(graph, start, end)
        for edge in h:
            key = tuple(sorted(edge))
            count[key] = count.get(key, 0) + 1


most_frequent_edges = sorted(count.items(), key=lambda x: x[1])[-3:]

for (source, target), _ in most_frequent_edges:
    graph[source].remove(target)
    graph[target].remove(source)

(start_subgraph_1, start_subgraph_2), _ = most_frequent_edges[0]


def count_connected_nodes(graph, start):
    queue = deque([start])
    seen = set()

    while queue:
        current = queue.popleft()

        if current in seen:
            continue

        seen.add(current)

        for target in graph[current]:
            queue.append(target)

    return len(seen)


count_subgraph_1 = count_connected_nodes(graph, start_subgraph_1)
count_subgraph_2 = count_connected_nodes(graph, start_subgraph_2)

assert count_subgraph_1 + count_subgraph_2 == len(nodes)

print(f"Part 1: {count_subgraph_1 * count_subgraph_2}")

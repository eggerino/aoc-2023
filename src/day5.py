import sys

content = ""
with open(sys.argv[1]) as file:
    content = file.read().strip()

seed_block, *mapping_blocks = content.split("\n\n")
seeds = [int(x) for x in seed_block.split(": ")[1].split()]


class Mapping:
    def __init__(self, block: str) -> None:
        lines = block.split("\n")[1:]
        self.maps = [[int(x) for x in line.split()] for line in lines]

    def apply_value(self, x: int):
        for dst, src, sz in self.maps:
            if src <= x < src + sz:
                return x + dst - src
        return x

    def apply_ranges(self, ranges: list[tuple[int, int]]):
        mapped = []
        for dst, src, sz in self.maps:
            end = src + sz

            unmapped = []
            while ranges:
                range_start, range_end = ranges.pop()

                # 3 possible sub ranges
                below = (range_start, min(range_end, src))
                middle = (max(range_start, src), min(range_end, end))
                above = (max(range_start, end), range_end)

                # subrange only exists if start < end
                if below[0] < below[1]:
                    unmapped.append(below)
                if middle[0] < middle[1]:
                    mapped.append(
                        (middle[0] + dst - src, middle[1] + dst - src))
                if above[0] < above[1]:
                    unmapped.append(above)

            # unmapped are possible ranges for next map, mapped already passed the map
            ranges = unmapped

        # some range stay unmapped -> they stay pass unmapped
        return mapped + ranges


mappings = [Mapping(block) for block in mapping_blocks]


def map_seed(seed: int):
    for mapping in mappings:
        seed = mapping.apply_value(seed)
    return seed


part_1 = min((map_seed(seed) for seed in seeds))
print(f"Part 1: {part_1}")

seed_ranges = list(zip(seeds[::2], seeds[1::2]))


def map_seed_range(seed_range):
    src, sz = seed_range
    range_list = [(src, src + sz)]
    for mapping in mappings:
        range_list = mapping.apply_ranges(range_list)
    return min((x for x, _ in range_list))


part_2 = min((map_seed_range(seed_range) for seed_range in seed_ranges))
print(f"Part 2: {part_2}")

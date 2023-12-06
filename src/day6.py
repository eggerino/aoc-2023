# d(x) = x * (T - x) = -x**2 + Tx
# d'(x) = -2x + T
# d'(x_0) = 0
# x_0 = T/2
# d(x): parabola with maximum at T/2, is positive in [0, T]
#
# Find x so that d(x) = D
# 0 = d(x) - D
# 0 = -x**2 + Tx - D
# a = -1
# b = T
# c = -D
# x_1,2 = (-T +- sqrt(T**2 - 4*D)) / (-2)
# if T**2 - 4*D < 0 -> no solution, not possible to beat
# The next bigger/smaller (whole integer) is the first/last that beats the record
# every charge time in between also beats the record
import math
import sys


content = open(sys.argv[1]).read().strip()
lines = content.split("\n")
times = [int(x) for x in lines[0].split(":")[1].split()]
distances = [int(x) for x in lines[1].split(":")[1].split()]
records = list(zip(times, distances))

single_time = int(lines[0].split(":")[1].replace(" ", ""))
single_distance = int(lines[1].split(":")[1].replace(" ", ""))


def ceil_exclusive(x: float):
    result = int(x)
    while result <= x:
        result += 1
    return result


def floor_exclusive(x: float):
    result = int(x) + 1
    while result >= x:
        result -= 1
    return result


def get_number_wins(time, distance):
    discriminant = time**2 - 4 * distance
    if discriminant < 0:
        return 0

    discriminant_sqrt = math.sqrt(discriminant)
    x_1 = (time - discriminant_sqrt) / 2
    x_2 = (time + discriminant_sqrt) / 2

    minimum_charge_time = ceil_exclusive(x_1)
    maximum_charge_time = floor_exclusive(x_2)

    number_wins = maximum_charge_time - minimum_charge_time + 1
    return max(number_wins, 0)


part_1 = 1
for time, distance in records:
    part_1 *= get_number_wins(time, distance)
print(f"Part 1: {part_1}")

part_2 = get_number_wins(single_time, single_distance)
print(f"Part 2: {part_2}")

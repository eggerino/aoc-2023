import functools
import sys


def card_rank(card: str, with_joker: bool):
    if card.isdigit():
        return int(card)

    if card == "T":
        return 10
    if card == "J":
        return 1 if with_joker else 11
    if card == "Q":
        return 12
    if card == "K":
        return 13
    if card == "A":
        return 14


def morph_jokers(grouping: list[int], num_jokers: int):
    if num_jokers == 0 or grouping[5] == 1:  # cant morph or already best
        return grouping

    if grouping[4] == 1 or (grouping[3] == 1 and grouping[2] == 1):
        return [0, 0, 0, 0, 0, 1]
    if grouping[3] == 1:
        return [0, 1, 0, 0, 1, 0]
    if grouping[2] == 2:
        return [0, 1, 0, 0, 1, 0] if num_jokers == 2 else [0, 0, 1, 1, 0, 0]
    if grouping[2] == 1:
        return [0, 2, 0, 1, 0, 0]
    if grouping[1] == 5:
        return [0, 3, 1, 0, 0, 0]
    raise ValueError


def eval_hand_type(cards: list[str], with_joker: bool):
    groups = {}
    for card in cards:
        groups[card] = groups.get(card, 0) + 1

    grouping = [0, 0, 0, 0, 0, 0]
    for k, v in groups.items():
        grouping[v] += 1

    if with_joker:
        grouping = morph_jokers(grouping, cards.count("J"))

    if grouping[5] == 1:
        return 7
    if grouping[4] == 1:
        return 6
    if grouping[3] == 1 and grouping[2] == 1:
        return 5
    if grouping[3] == 1:
        return 4
    if grouping[2] == 2:
        return 3
    if grouping[2] == 1:
        return 2
    if grouping[1] == 5:
        return 1
    raise ValueError


def eval_hand(line: str, with_joker: bool):
    cards_token, bid = line.split()
    cards = [*cards_token]
    hand_type = eval_hand_type(cards, with_joker)
    return int(bid), cards, hand_type


def compare_hands(hand1: tuple[int, list[str], int], hand2: tuple[int, list[int], int], with_joker: bool):
    _, cards1, type1 = hand1
    _, cards2, type2 = hand2

    if type1 != type2:
        return type1 - type2

    ranks1 = [card_rank(x, with_joker) for x in cards1]
    ranks2 = [card_rank(x, with_joker) for x in cards2]

    for r1, r2 in zip(ranks1, ranks2):
        if r1 != r2:
            return r1 - r2
    raise ValueError


lines = open(sys.argv[1]).read().strip().split("\n")
for part in [1, 2]:
    with_joker = part == 2
    hands = [eval_hand(line, with_joker) for line in lines]
    sorted_hands = sorted(hands, key=functools.cmp_to_key(
        lambda a, b: compare_hands(a, b, with_joker)))

    result = 0
    for i, (bid, _, _) in enumerate(sorted_hands):
        result += (i + 1) * bid

    print(f"Part {part}: {result}")

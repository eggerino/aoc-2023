#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#define MAX_NUMBER_IN_WINNING_MAP 100
#define MAX_NUMBER_CARDS 202

size_t find_matches_in_card(const char* card);
size_t get_point_value(size_t number_matches);

size_t find_matches_in_card(const char* card) {
    size_t index = 0;
    while (card[index] != ':') {
        ++index;  // advance to colon
    }

    uint8_t winning_map[MAX_NUMBER_IN_WINNING_MAP] = {0};  // Use number to index into the map
    while (card[index] != '|') {
        while (!isdigit(card[index])) {
            ++index;
        }

        size_t winning_number = atoi(&card[index]);
        assert(winning_number < MAX_NUMBER_IN_WINNING_MAP && "Increase the MAX_NUMBER_IN_WINNING_MAP");
        winning_map[winning_number] = 1;

        while (isdigit(card[index])) {
            ++index;  // advance to first non digit char
        }
        ++index;  // skip one space so the pipe will be at index on end or a digit one the next number
    }

    size_t number_matches = 0;
    while (card[index] != '\n' && card[index] != '\0') {
        while (!isdigit(card[index])) {
            ++index;
        }

        size_t found_number = atoi(&card[index]);
        assert(found_number < MAX_NUMBER_IN_WINNING_MAP && "Increase the MAX_NUMBER_IN_WINNING_MAP");
        if (winning_map[found_number]) {
            ++number_matches;
        }

        while (isdigit(card[index])) {
            ++index;
        }
    }

    return number_matches;
}

size_t get_point_value(size_t number_matches) {
    if (number_matches < 2) {
        return number_matches;
    }

    return 2 * get_point_value(number_matches - 1);
}

int main(int argc, const char** argv) {
    FILE* file = open_input_file_or_panic(argc, argv);

    size_t result_1 = 0;
    int get_copies = 1;
    uint32_t card_count[MAX_NUMBER_CARDS] = {0};

    size_t line_buffer_len = 0;
    char* line = NULL;
    ssize_t chars_read;
    size_t line_index;
    for (line_index = 0; (chars_read = getline(&line, &line_buffer_len, file)) != -1; ++line_index) {
        assert(line_index < MAX_NUMBER_CARDS && "Increase MAX_NUMBER_CARDS");

        card_count[line_index]++;  // Original card

        size_t found_matches = find_matches_in_card(line);
        result_1 += get_point_value(found_matches);

        if (!get_copies) {
            continue;
        }

        assert((line_index + found_matches) < MAX_NUMBER_CARDS && "Increase MAX_NUMBER_CARDS");
        for (size_t copied_card_offset = 1; copied_card_offset <= found_matches; ++copied_card_offset) {
            card_count[line_index + copied_card_offset] += card_count[line_index];
        }
    }

    size_t result_2 = 0;
    for (size_t i = 0; i < line_index; ++i) {
        result_2 += card_count[i];
    }

    if (line != NULL) {
        free(line);
    }
    fclose(file);

    printf("Part 1: %d\n", result_1);
    printf("Part 2: %d\n", result_2);

    exit(EXIT_SUCCESS);
}
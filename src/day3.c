#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"

typedef enum CollectStatus {
    COLLECT_OK = 0,
    COLLECT_NO_NUMBER,
    COLLECT_LIMIT_EXCEEDED
} CollectStatus;

size_t get_row_length(const char* schematic);
size_t get_index_at(size_t row, size_t column, size_t row_length);
void get_position_from(size_t index, size_t row_length, size_t* row, size_t* column);
int is_symbol(char item);
int has_adjacent_symbol(const char* schematic, size_t schematic_size, size_t row_length, size_t index, size_t length);
size_t solve_part1(const char* schematic, size_t schematic_size, size_t row_length);
int32_t get_serial_number_containing(const char* schematic, size_t index);
CollectStatus collect_serial_number(int32_t* serial_numbers, const char* schematic, size_t index);
size_t solve_part2(const char* schematic, size_t schematic_size, size_t row_length);

size_t get_row_length(const char* schematic) {
    size_t index = 0;
    while (schematic[index] != '\0' && schematic[index] != '\n') {
        ++index;
    }
    return index;
}

size_t get_index_at(size_t row, size_t column, size_t row_length) {
    size_t stride = row_length + 1;
    return row * stride + column;
}

void get_position_from(size_t index, size_t row_length, size_t* row, size_t* column) {
    size_t stride = row_length + 1;
    *row = index / stride;
    *column = index % stride;
}

int is_symbol(char item) { return !isdigit(item) && item != '.' && item != '\n' && item != '\0'; }

int has_adjacent_symbol(const char* schematic, size_t schematic_size, size_t row_length, size_t index, size_t length) {
    size_t row, column;
    get_position_from(index, row_length, &row, &column);

    size_t row_start = row == 0 ? 0 : row - 1;
    size_t row_end = row + 1;
    size_t column_start = column == 0 ? 0 : column - 1;
    size_t column_end = column + length;

    for (row = row_start; row <= row_end; ++row) {
        for (column = column_start; column <= column_end; ++column) {
            size_t index = get_index_at(row, column, row_length);

            if (index >= schematic_size) {
                continue;
            }

            if (is_symbol(schematic[index])) {
                return 1;
            }
        }
    }
    return 0;
}

size_t solve_part1(const char* schematic, size_t schematic_size, size_t row_length) {
    size_t result = 0;
    for (size_t i = 0; i < schematic_size; ++i) {
        if (!isdigit(schematic[i])) {  // advance to next digit
            continue;
        }

        int32_t serial_number = atoi(&schematic[i]);

        size_t peek = i;
        while (isdigit(schematic[++peek])) {
        }  // peek to end of serial number

        size_t length_serial_number = peek - i;
        if (has_adjacent_symbol(schematic, schematic_size, row_length, i, length_serial_number)) {
            result += serial_number;
        }

        i = peek;  // Advance to peeked position
    }

    return result;
}

int32_t get_serial_number_containing(const char* schematic, size_t index) {
    if (index == 0) {
        return atoi(schematic);
    }

    if (!isdigit(schematic[index])) {
        return atoi(&schematic[index + 1]);
    }

    return get_serial_number_containing(schematic, index - 1);
}

CollectStatus collect_serial_number(int32_t* serial_numbers, const char* schematic, size_t index) {
    if (!isdigit(schematic[index])) {
        return COLLECT_NO_NUMBER;
    }

    int32_t serial_number = get_serial_number_containing(schematic, index);
    for (size_t i = 0; i < 2; ++i) {
        if (serial_numbers[i] == 0) {
            serial_numbers[i] = serial_number;
            return COLLECT_OK;
        }
    }

    return COLLECT_LIMIT_EXCEEDED;
}

size_t solve_part2(const char* schematic, size_t schematic_size, size_t row_length) {
    size_t result = 0;
    for (size_t i = 0; i < schematic_size; ++i) {
        if (schematic[i] != '*') {  // Advance to next asterix
            continue;
        }

        size_t row, column;
        get_position_from(i, row_length, &row, &column);

        int32_t serial_numbers[2] = {0};

        // Collect row above
        if (row > 0) {
            size_t index_above = get_index_at(row - 1, column, row_length);
            if (collect_serial_number(serial_numbers, schematic, index_above)) {
                // Collect top left
                if (column > 0) {
                    collect_serial_number(serial_numbers, schematic, index_above - 1);
                }
                collect_serial_number(serial_numbers, schematic, index_above + 1);  // top right
            }
        }

        // Collect in row
        if (column > 0) {
            if (collect_serial_number(serial_numbers, schematic, i - 1) == COLLECT_LIMIT_EXCEEDED) {
                continue;
            }
        }
        if (i + 1 < schematic_size) {
            if (collect_serial_number(serial_numbers, schematic, i + 1) == COLLECT_LIMIT_EXCEEDED) {
                continue;
            }
        }

        // Collect row below
        size_t index_below = get_index_at(row + 1, column, row_length);
        if (index_below < schematic_size) {
            switch (collect_serial_number(serial_numbers, schematic, index_below)) {
                case COLLECT_LIMIT_EXCEEDED:
                    continue;
                case COLLECT_NO_NUMBER:
                    if (collect_serial_number(serial_numbers, schematic, index_below - 1) == COLLECT_LIMIT_EXCEEDED) {
                        continue;
                    }
                    if (index_below + 1 < schematic_size) {
                        if (collect_serial_number(serial_numbers, schematic, index_below + 1) == COLLECT_LIMIT_EXCEEDED) {
                            continue;
                        }
                    }
            }
        }

        if (serial_numbers[0] == 0 && serial_numbers[1] == 0) { // Not enough serial numbers found
            continue;
        }

        result += serial_numbers[0] * serial_numbers[1];
    }
    return result;
}

int main(int argc, const char** argv) {
    FILE* file = open_input_file_or_panic(argc, argv);
    size_t schematic_size = 0;
    char* schematic = read_content(file, &schematic_size);
    size_t row_length = get_row_length(schematic);

    size_t result_1 = solve_part1(schematic, schematic_size, row_length);
    printf("Part 1: %d\n", result_1);
    size_t result_2 = solve_part2(schematic, schematic_size, row_length);
    printf("Part 2: %d\n", result_2);

    free(schematic);
    fclose(file);

    exit(EXIT_SUCCESS);
}
#define _GNU_SOURCE
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_RED_CUBES 12
#define NUMBER_GREEN_CUBES 13
#define NUMBER_BLUE_CUBES 14

int32_t parse_game_id(const char* line, size_t* line_index);
int are_draws_left(const char* line, size_t line_index);
void get_draw_amounts(const char* line, size_t* index, int32_t* red, int32_t* green, int32_t* blue);
int32_t max(int32_t value1, int32_t value2);
int is_valid_draw(int32_t red, int32_t green, int32_t blue);
int32_t eval_set_power(int32_t red, int32_t green, int32_t blue);

int32_t parse_game_id(const char* line, size_t* line_index) {
    for (*line_index = 5; line[*line_index] != ':'; ++(*line_index)) {
    }
    return atoi(&line[5]);
}

int are_draws_left(const char* line, size_t line_index) {
    for (; line[line_index] != '\0'; ++line_index) {
        if (isdigit(line[line_index])) {
            return 1;
        }
    }
    return 0;
}

void get_draw_amounts(const char* line, size_t* line_index, int32_t* red, int32_t* green, int32_t* blue) {
    *red = 0;
    *green = 0;
    *blue = 0;

    size_t index = *line_index + 1;  // Move away from draw delimiter
    while (line[index] != ';' && line[index] != '\n' && line[index] != '\0') {
        while (!isdigit(line[++index])) {
        }  // advance to next digit
        int32_t amount = atoi(&line[index]);

        while (!islower(line[++index])) {
        }  // advance to color specifier
        char color = line[index];

        switch (color) {
            case 'r':
                *red = amount;
                break;
            case 'g':
                *green = amount;
                break;
            case 'b':
                *blue = amount;
                break;
            default:
                fprintf(stderr, "Unknown color character %c\n", color);
                exit(EXIT_FAILURE);
        }

        while (islower(line[++index])) {
        }  // advance to the delimiter
    }

    *line_index = index;
}

int32_t max(int32_t value1, int32_t value2) { return value1 > value2 ? value1 : value2; }

int is_valid_draw(int32_t red, int32_t green, int32_t blue) {
    return red <= NUMBER_RED_CUBES && green <= NUMBER_GREEN_CUBES && blue <= NUMBER_BLUE_CUBES;
}

int32_t eval_set_power(int32_t red, int32_t green, int32_t blue) { return red * green * blue; }

int main(int argc, const char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid usage\n%s <DATA FILENAME>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Invalid filename '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int32_t result_1 = 0;
    int32_t result_2 = 0;

    size_t line_buffer_len = 0;
    char* line = NULL;
    ssize_t chars_read;
    while ((chars_read = getline(&line, &line_buffer_len, file)) != -1) {
        size_t line_index = 0;
        int32_t id = parse_game_id(line, &line_index);

        int32_t min_red = 0;
        int32_t min_green = 0;
        int32_t min_blue = 0;

        while (are_draws_left(line, line_index)) {
            int32_t current_red, current_green, current_blue;
            get_draw_amounts(line, &line_index, &current_red, &current_green, &current_blue);

            min_red = max(min_red, current_red);
            min_green = max(min_green, current_green);
            min_blue = max(min_blue, current_blue);
        }

        if (is_valid_draw(min_red, min_green, min_blue)) {
            result_1 += id;
        }

        result_2 += eval_set_power(min_red, min_green, min_blue);
    }

    if (line != NULL) {
        free(line);
    }
    fclose(file);

    printf("Part 1: %d\n", result_1);
    printf("Part 2: %d\n", result_2);

    exit(EXIT_SUCCESS);
}

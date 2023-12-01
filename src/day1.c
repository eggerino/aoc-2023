#define _GNU_SOURCE
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const char const* DIGIT_LITERALS[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
const size_t DIGIT_LITERALS_LENGTH[] = {4, 3, 3, 5, 4, 4, 3, 5, 5, 4};

int find_first_last_digit(const char* str, int32_t* first, int32_t* last, int allow_literals);
int32_t eval_calibration_value(int32_t first, int32_t last);
int try_parse_digit(const char* str, int32_t* value, int allow_literals);
int string_compare(size_t n, const char* str1, const char* str2);

int find_first_last_digit(const char* str, int32_t* first, int32_t* last, int allow_literals) {
    int no_digit_yet_found = -1;

    for (size_t i = 0; str[i] != '\0'; ++i) {
        int32_t current_digit;
        if (!try_parse_digit(&str[i], &current_digit, allow_literals)) {
            continue;
        }

        if (no_digit_yet_found) {
            *first = current_digit;
        }
        *last = current_digit;
        no_digit_yet_found = 0;
    }

    return no_digit_yet_found;
}

int32_t eval_calibration_value(int32_t first, int32_t last) { return first * 10 + last; }

int try_parse_digit(const char* str, int32_t* value, int allow_literals) {
    if (isdigit(str[0])) {
        *value = str[0] - '0';
        return 1;
    }

    if (!allow_literals) {
        return 0;
    }

    for (size_t i = 0; i < 10; ++i) {
        if (string_compare(DIGIT_LITERALS_LENGTH[i], DIGIT_LITERALS[i], str)) {
            *value = i;
            return 1;
        }
    }
    return 0;
}

int string_compare(size_t n, const char* str1, const char* str2) {
    for (size_t i = 0; i < n; ++i) {
        if (str1[i] != str2[i]) {
            return 0;
        }
    }
    return 1;
}

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
    for (size_t line_index = 0; (chars_read = getline(&line, &line_buffer_len, file)) != -1; ++line_index) {
        int32_t first, last;
        if (find_first_last_digit(line, &first, &last, 0)) {
            fprintf(stderr, "No digits found in line %d\n", line_index + 1);
            exit(EXIT_FAILURE);
        }
        result_1 += eval_calibration_value(first, last);

        if (find_first_last_digit(line, &first, &last, 1)) {
            fprintf(stderr, "No digits or literals found in line %d\n", line_index + 1);
            exit(EXIT_FAILURE);
        }
        result_2 += eval_calibration_value(first, last);
    }

    if (line != NULL) {
        free(line);
    }
    fclose(file);

    printf("Part 1: %d\n", result_1);
    printf("Part 2: %d\n", result_2);

    exit(EXIT_SUCCESS);
}

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>

static FILE* open_input_file_or_panic(int argc, const char const**argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid usage\n%s <DATA FILENAME>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Invalid filename '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    return file;
}

#endif

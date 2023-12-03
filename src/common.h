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

static char* read_content(FILE* file, size_t* size) {
    fseek(file, 0, SEEK_END);
    *size = ftell(file);

    char* content = (char*)malloc((*size) * sizeof(char));
    if (content == NULL) {
        fprintf(stderr, "Unable to aquire memory needed to read the content of the file\n.");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_SET);
    fread(content, sizeof(char), *size, file);
    return content;
}

#endif

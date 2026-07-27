#include "file_reader.h"

FILE *fr_open_file(const char *filename, const char *mode) 
{
    FILE *f = fopen(filename, mode);
    if (f == NULL) {
        return NULL;
    }

    return f;
}

char *fr_get_line(FILE *f, int n)
{
    if (f == NULL) {
        return NULL;
    }

    int max_len = 128;
    char *buffer = malloc(sizeof(char) * max_len);
    if (buffer == NULL) {
        return NULL;
    }

    int count = 0;
    while (count <= n) {
        if (fgets(buffer, max_len, f) == NULL) {
            free(buffer);
            return NULL;
        }
        ++count;
    }

    return buffer;
}

int fr_append(const char *filename, char *data)
{
    FILE* f = fopen(filename, "a");
    if (f) {
        fprintf(f, "%s\n", data);
        fclose(f);
        return APP_SUCCESS;
    }
    return APP_FAILURE;
}
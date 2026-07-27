#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <stdlib.h>

#include "error_codes.h"

FILE* fr_open_file(const char *filename, const char *mode);
char *fr_get_line(FILE *f, int n);
int fr_append(const char *filename, char *data);

#endif
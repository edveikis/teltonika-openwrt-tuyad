#ifndef USER_INPUT_H
#define USER_INPUT_H

#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_codes.h"

struct Arguments {
    char dev_id[128];
    char dev_secret[128];
    char product_id[128];
    int daemon_mode;
    int interval;
    int got_user_input;
};

void usr_print_usage(const char *prog_name);
int usr_parse_arguments(int argc, char *argv[], struct Arguments *arguments);

#endif
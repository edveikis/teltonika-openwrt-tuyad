#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "error_codes.h"

typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CpuStats;

double cpu_usage(unsigned int sampleIntervalMs);

#endif
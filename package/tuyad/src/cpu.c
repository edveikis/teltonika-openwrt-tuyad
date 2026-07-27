#include "cpu.h"

static int cpu_read_stats(CpuStats *s) 
{
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        return APP_FAILURE;
    }

    char cpu_label[16];
    int matched = fscanf(fp, "%15s %llu %llu %llu %llu %llu %llu %llu %llu",
        cpu_label, &s->user, &s->nice, &s->system, &s->idle,
        &s->iowait, &s->irq, &s->softirq, &s->steal);

    fclose(fp);

    if (matched != 9) {
        return APP_FAILURE;
    }

    return APP_SUCCESS;
}

double cpu_usage(unsigned int sampleIntervalMs) 
{
    CpuStats prev, curr;

    if (cpu_read_stats(&prev) != 0) {
        return NO_VALUE;
    }

    usleep(sampleIntervalMs * 1000);

    if (cpu_read_stats(&curr) != 0) {
        return NO_VALUE;
    } 
        
    unsigned long long prevIdle = prev.idle + prev.iowait;
    unsigned long long currIdle = curr.idle + curr.iowait;

    unsigned long long prevTotal = prev.user + prev.nice + prev.system +
        prev.idle + prev.iowait + prev.irq + prev.softirq + prev.steal;
    unsigned long long currTotal = curr.user + curr.nice + curr.system +
        curr.idle + curr.iowait + curr.irq + curr.softirq + curr.steal;

    unsigned long long totalDelta = currTotal - prevTotal;
    unsigned long long idleDelta  = currIdle  - prevIdle;

    if (totalDelta == 0) {
        return 0.0;
    } 

    return (double)(totalDelta - idleDelta) / (double)totalDelta * 100.0;
}
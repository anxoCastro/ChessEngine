//
// Created by castro on 30/3/22.
//

#include <sys/time.h>
#include <stddef.h>
#include "time.h"
long get_time_ms() {
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}
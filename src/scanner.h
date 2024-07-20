// Aaron Sprouse 2024
#pragma once

#include <yara.h>

typedef struct {
    char filename[MAX_PATH];
} filedata_t;

int scan_callback(
    YR_SCAN_CONTEXT* context,
    int message,
    void* message_data,
    void* user_data);
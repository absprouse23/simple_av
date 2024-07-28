// Aaron Sprouse 2024
#pragma once

#include <limits.h>
#include <unistd.h>
#include <yara.h>

#define FILEDATA_TYPE_FD 1
#define FILEDATA_TYPE_NAME 2

typedef struct {
    char *filename;
    int fd;
    int type;
} filedata_t;

filedata_t *init_filedata(char *restrict filename, int fd, int type);
void free_filedata(filedata_t *file_data);

int scan_callback(YR_SCAN_CONTEXT *context, int message, void *message_data,
                  void *user_data);
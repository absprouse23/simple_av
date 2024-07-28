// Aaron Sprouse 2024

#include "scanner.h"
#include <time.h>

filedata_t *init_filedata(char *restrict filename, int fd, int type) {
    filedata_t *new_file = malloc(sizeof(filedata_t));
    if (NULL == new_file) {
        return NULL;
    }

    switch (type) {
    case FILEDATA_TYPE_FD:
        new_file->type = type;
        new_file->fd = fd;
        new_file->filename = NULL;
        break;
    case FILEDATA_TYPE_NAME:
        new_file->type = type;
        new_file->fd = -1;
        new_file->filename = malloc(PATH_MAX + 1);
        strncpy(new_file->filename, filename, PATH_MAX);
        break;
    default:
        free(new_file);
        return NULL;
    };

    return new_file;
}

void free_filedata(filedata_t *file_data) {
    if (file_data->filename != NULL) {
        free(file_data->filename);
    }
    free(file_data);
}

static void print_fd_name(int fd) {
    char path[PATH_MAX];
    char resolved_path[PATH_MAX];

    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
    int path_len = readlink(path, resolved_path, sizeof(resolved_path) - 1);
    if (-1 == path_len) {
        perror("readlink");
    }
    resolved_path[path_len] = '\0'; // Ensure null-terminated string
    printf("File: %s\n", resolved_path);
}

int scan_callback([[maybe_unused]] YR_SCAN_CONTEXT *context, int message,
                  void *message_data, void *user_data) {

    switch (message) {
    case CALLBACK_MSG_RULE_MATCHING:
        printf("Rule match for rule: %s\n",
               ((YR_RULE *)message_data)->identifier);

        if (FILEDATA_TYPE_FD == ((filedata_t *)user_data)->type) {
            print_fd_name(((filedata_t *)user_data)->fd);
        } else if (FILEDATA_TYPE_NAME == ((filedata_t *)user_data)->type) {
            printf("File: %s\n", ((filedata_t *)user_data)->filename);
        }

        return CALLBACK_CONTINUE;
    default:
        return CALLBACK_CONTINUE;
    }
}
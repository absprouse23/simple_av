// Aaron Sprouse 2024

#include <linux/limits.h>
#define MODE_SINGLEFILE 0
#define MODE_DIR 1

#define _GNU_SOURCE

#include "compiler.h"
#include "scanner.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/fanotify.h>

int main(int argc, char **argv) {

    int opt;
    int prg_mode = -1;
    char filepath[PATH_MAX];
    char *result = NULL;
    while ((opt = getopt(argc, argv, "f:p:")) != -1) {
        switch (opt) {
        case 'f':
            result = realpath(optarg, filepath);
            prg_mode = MODE_SINGLEFILE;
            break;
        case 'p':
            result = realpath(optarg, filepath);
            prg_mode = MODE_DIR;
            break;
        default:
            fprintf(stderr,
                    "Usage: simple_av [-f <filename> | -p <filename>]\n");
            exit(EINVAL);
        }
    }

    if (NULL == result) {
        perror("realpath");
        return EXIT_FAILURE;
    }

    if (prg_mode == -1) {
        fprintf(stderr, "Usage: simple_av [-f <filename> | -p <filename>]\n");
        exit(EINVAL);
    }

    if (yr_initialize() != ERROR_SUCCESS) {
        fprintf(stderr, "Failed to init yara compiler\n");
        exit(EXIT_FAILURE);
    }

    YR_RULES *rules;

    if (yr_rules_load("rules.dat", &rules) == ERROR_COULD_NOT_OPEN_FILE) {
        if (compiler_build_ruleset(&rules) == EXIT_FAILURE) {
            yr_finalize();
            return EXIT_FAILURE;
        }
        yr_rules_save(rules, "rules.dat");
    }

    if (prg_mode == MODE_SINGLEFILE) {
        filedata_t *file_data = init_filedata(filepath, -1, FILEDATA_TYPE_NAME);

        int scan_status = yr_rules_scan_file(
            rules, filepath,
            SCAN_FLAGS_FAST_MODE | SCAN_FLAGS_REPORT_RULES_MATCHING,
            scan_callback, &file_data, 0);

        switch (scan_status) {
        case ERROR_SUCCESS:
            break;
        case ERROR_COULD_NOT_OPEN_FILE:
            fprintf(stderr, "Could not open file: %s\n", filepath);
            break;
        default:
            fprintf(stderr, "General Error\n");
        }

        free_filedata(file_data);

    } else {
        int fanotify_fd =
            fanotify_init(FAN_CLOEXEC | FAN_NONBLOCK | FAN_CLASS_CONTENT,
                          O_RDONLY | O_LARGEFILE);
        if (fanotify_fd == -1) {
            perror("fanotify_init");
            exit(EXIT_FAILURE);
        }
        // Just interested in FAN_CLOSE_WRITE at the moment for demonstration
        // purposes
        if (fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
                          FAN_CLOSE_WRITE, AT_FDCWD, filepath) == -1) {
            perror("fanotify_mark");
            close(fanotify_fd);
            exit(EXIT_FAILURE);
        }

        // Set up polling
        struct pollfd fds[1];
        fds[0].fd = fanotify_fd;
        fds[0].events = POLLIN;

        printf("Monitoring file access and writes on the root filesystem and "
               "its subdirectories...\n");

        while (1) {
            int poll_num = poll(fds, 1, -1);
            if (poll_num == -1) {
                if (errno == EINTR)
                    continue;
                perror("poll");
                close(fanotify_fd);
                exit(EXIT_FAILURE);
            }

            if (fds[0].revents & POLLIN) {
                char buffer[4096];
                ssize_t len = read(fanotify_fd, buffer, sizeof(buffer));
                if (len == -1 && errno != EAGAIN) {
                    perror("read");
                    close(fanotify_fd);
                    exit(EXIT_FAILURE);
                }

                struct fanotify_event_metadata *metadata;
                for (metadata = (struct fanotify_event_metadata *)buffer;
                     FAN_EVENT_OK(metadata, len);
                     metadata = FAN_EVENT_NEXT(metadata, len)) {
                    if (metadata->mask & FAN_Q_OVERFLOW) {
                        printf("Queue overflow!\n");
                        continue;
                    }

                    if (metadata->mask & FAN_CLOSE_WRITE) {
                        filedata_t *file_data =
                            init_filedata(NULL, metadata->fd, FILEDATA_TYPE_FD);
                        yr_rules_scan_fd(rules, metadata->fd,
                                         SCAN_FLAGS_FAST_MODE |
                                             SCAN_FLAGS_REPORT_RULES_MATCHING,
                                         scan_callback, file_data, 0);
                        free_filedata(file_data);
                        close(metadata->fd);
                    }
                }
            }
        }

        close(fanotify_fd);
    }

    yr_rules_destroy(rules);
    yr_finalize();
}
// Aaron Sprouse 2024

#include "compiler.h"
#include "scanner.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#define MODE_SINGLEFILE 0
#define MODE_DIR 1

int main(int argc, char **argv) {

    int opt;
    int prg_mode = -1;
    filedata_t file_data;
    while ((opt = getopt(argc, argv, "f:p:")) != -1) {
        switch (opt) {
        case 'f':
            realpath(optarg, file_data.filename);
            prg_mode = MODE_SINGLEFILE;
            break;
        case 'p':
            realpath(optarg, file_data.filename);
            prg_mode = MODE_DIR;
            break;
        default:
            fprintf(stderr,
                    "Usage: simple_av [-f <filename> | -p <filename>]\n");
            exit(EINVAL);
        }
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
        int scan_status = yr_rules_scan_file(
            rules, file_data.filename,
            SCAN_FLAGS_FAST_MODE | SCAN_FLAGS_REPORT_RULES_MATCHING,
            scan_callback, &file_data, 0);

        switch (scan_status) {
        case ERROR_SUCCESS:
            break;
        case ERROR_COULD_NOT_OPEN_FILE:
            fprintf(stderr, "Could not open file: %s\n", file_data.filename);
            break;
        default:
            fprintf(stderr, "General Error\n");
        }
    } else {
        printf("Not yet implimented\n");
    }

    yr_rules_destroy(rules);
    yr_finalize();
}
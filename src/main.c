// Aaron Sprouse 2024

#include "compiler.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

    if (yr_initialize() != ERROR_SUCCESS) {
        fprintf(stderr, "Failed to init yara compiler");
        return EXIT_FAILURE;
    }

    YR_RULES* rules;

    if (yr_rules_load("rules.dat", &rules) == ERROR_COULD_NOT_OPEN_FILE) {
        if (compiler_build_ruleset(&rules) == EXIT_FAILURE) {
            yr_finalize();
            return EXIT_FAILURE;
        }
        yr_rules_save(rules, "rules.dat");
    }

    filedata_t user_file;
    realpath(argv[1], user_file.filename);
    
    int scan_status = yr_rules_scan_file(rules, argv[1], SCAN_FLAGS_FAST_MODE | SCAN_FLAGS_REPORT_RULES_MATCHING, scan_callback, &user_file, 0);

    switch (scan_status) {
        case ERROR_SUCCESS:
            break;
        case ERROR_INSUFFICIENT_MEMORY:
            fprintf(stderr, "Insufficient memory for scan\n");
            break;
        case ERROR_COULD_NOT_OPEN_FILE:
            fprintf(stderr, "Could not open file: %s\n", argv[1]);
            break;
        default:
            fprintf(stderr, "General Error\n");
    }

    yr_rules_destroy(rules);
    yr_finalize();
}
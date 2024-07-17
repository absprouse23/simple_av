#include "compiler.h"
#include <yara/error.h>

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

    yr_finalize();
}
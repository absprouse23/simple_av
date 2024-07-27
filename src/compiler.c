// Aaron Sprouse 2024

#include "compiler.h"
#include <dirent.h>
#include <stdio.h>
#include <yara/compiler.h>

void error_callback(int error_level, [[maybe_unused]] const char *file_name,
                    int line_number, [[maybe_unused]] const YR_RULE *rule,
                    const char *message, [[maybe_unused]] void *user_data) {
    if (error_level == YARA_ERROR_LEVEL_ERROR) {
        fprintf(stderr, "Error: ");
    } else {
        fprintf(stderr, "Warning: ");
    }

    fprintf(stderr, "%d ", line_number);
    fprintf(stderr, "%s\n", message);
}

int compiler_build_ruleset(YR_RULES **rules) {

    YR_COMPILER *compiler;

    if (yr_compiler_create(&compiler) != ERROR_SUCCESS) {
        fprintf(stderr, "Failed to create YARA compiler\n");
        return EXIT_FAILURE;
    }

    yr_compiler_set_callback(compiler, error_callback, NULL);

    DIR *dir_stream = opendir("./rules/");
    if (dir_stream == NULL) {
        perror("Cannot open rules directory. Does it exist?");
        yr_compiler_destroy(compiler);
        return EXIT_FAILURE;
    }

    struct dirent *dir;
    while ((dir = readdir(dir_stream)) != NULL) {
        if (dir->d_type != DT_REG) { // TODO: Move into subfolder if it exists
            continue;
        }
        printf("Compiling rule %s\n", dir->d_name);

        chdir("rules");
        FILE *current_file = fopen(dir->d_name, "r");
        int error =
            yr_compiler_add_file(compiler, current_file, NULL, "error.log");

        if (error > 0) {
            fprintf(stderr, "Failed to compile rule %s\n", dir->d_name);
            compiler->errors = 0;
        }
        fclose(current_file);
        chdir("..");
    }

    if (yr_compiler_get_rules(compiler, rules) == ERROR_INSUFFICIENT_MEMORY) {
        yr_compiler_destroy(compiler);
        return EXIT_FAILURE;
    }

    yr_compiler_destroy(compiler);
    return EXIT_SUCCESS;
}
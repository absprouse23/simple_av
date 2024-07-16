#include "compiler.h"

int compiler_build_ruleset(YR_RULES** rules) {

    YR_COMPILER* compiler;

    if (yr_compiler_create(&compiler) != ERROR_SUCCESS) {
        fprintf(stderr, "Failed to create YARA compiler\n");
        return EXIT_FAILURE;
    }

    DIR* dir_stream = opendir("./rules/");
    if (dir_stream == NULL) {
        perror("Cannot open rules directory. Does it exist?");
        yr_compiler_destroy(compiler);
        return EXIT_FAILURE;
    }

    struct dirent *dir;
    while ((dir = readdir(dir_stream)) != NULL) {
        if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0)) {
            continue;
        }
        printf("Compiling rule %s\n", dir->d_name);

        chdir("rules");
        FILE* current_file = fopen(dir->d_name, "r");
        int error = yr_compiler_add_file(compiler, current_file , NULL, NULL);

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
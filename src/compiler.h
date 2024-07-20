// Aaron Sprouse 2024

#pragma once

#include <dirent.h>
#include <unistd.h>
#include <yara.h>

int compiler_build_ruleset(YR_RULES **rules);
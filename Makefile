# C makefile by Samuel B. Foster (github.com/smlbfstr)

# Config
compiler = gcc
debug_compile_flags = -Wall -Wextra -Wfatal-errors -Werror -O0 -g
debug_link_flags = -lyara
release_compile_flags = -Wall -Wextra -Wfatal-errors -Werror -O2
release_link_flags = -lyara
debug = true

build_directory = bin
source_directory = src

# Automatic variables (DO NOT TOUCH ANYTHING BELOW THIS POINT)
target = $(build_directory)/$(notdir $(shell pwd))
sources = $(shell find $(source_directory) -name '*.c')
objects = $(sources:%=$(build_directory)/%.o)
dependencies = $(objects:.o=.d)
include_directories = $(shell find $(source_directory) -type d)
prefix = [\e[33mmake\e[0m]
compile_flags =
link_flags =

ifeq ($(findstring r,$(firstword -$(MAKEFLAGS))),r)
	debug = false
endif

ifeq ($(debug), true)
	compile_flags += $(debug_compile_flags)
	link_flags += $(debug_link_flags)
else
	compile_flags += $(release_compile_flags)
	link_flags += $(release_link_flags)
endif

# Build rules
$(target): $(objects)
	@printf "$(prefix) \e[35mLinking objects\e[0m\n"
	@$(compiler) $(objects) -o $@ $(link_flags)

$(build_directory)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@printf "$(prefix) \e[32mCompiling\e[0m %s\n" $<
	@$(compiler) $(compile_flags) $(addprefix -I,$(include_directories)) -MMD -MP $(CFLAGS) -c $< -o $@
	@chmod go-w $@

-include $(dependencies)

# Subcommands
.PHONY: clean run all

clean:
ifeq ($(wildcard $(build_directory)/.),)
	@printf "$(prefix) \e[31mNothing to clean\e[0m\n"
else
	@printf "$(prefix) \e[31mCleaning binaries\e[0m\n"
	@rm -r $(build_directory)
endif

run:
	@printf "$(prefix) \e[34mRunning\e[0m %s\n" $(notdir $(target))
	@./$(target)

all: clean $(target) run

help:
	@printf "┌─────────────────────────────────────────┐\n"
	@printf "│         \e[1mC Makefile by smlbfstr\e[0m          │\n"
	@printf "├─────────────┬───────────────────────────┤\n"
	@printf "│ \e[32mmake\e[0m        │ Compiles your code        │\n"
	@printf "├─────────────┼───────────────────────────┤\n"
	@printf "│ \e[2;35mmake\e[0;35m -r ...\e[0m │ Enables release mode      │\n"
	@printf "├─────────────┼───────────────────────────┤\n"
	@printf "│ \e[2;31mmake\e[0;31m clean\e[0m  │ Removes build directory   │\n"
	@printf "├─────────────┼───────────────────────────┤\n"
	@printf "│ \e[2;34mmake\e[0;34m run\e[0m    │ Runs the compiled binary  │\n"
	@printf "├─────────────┼───────────────────────────┤\n"
	@printf "│ \e[2;33mmake\e[0;33m all\e[0m    │ Does everything           │\n"
	@printf "└─────────────┴───────────────────────────┘\n"
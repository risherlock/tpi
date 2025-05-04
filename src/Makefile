target = main
build_dir = build

cpp_sources = \
libs/serial.cpp \
main.cpp

c_sources = \

includes = \
-Ilibs

defines =

libs = -lserialport
lib_dir =

c_compiler = gcc
cpp_compiler = g++

linker_flags = $(lib_dir) $(libs) -g
compiler_flags = -g -O3 -Wall -Wextra -Werror -lm $(includes) $(defines)

objects += $(addprefix $(build_dir)/,$(notdir $(c_sources:.c=.o)))
objects += $(addprefix $(build_dir)/,$(notdir $(cpp_sources:.cpp=.o)))

vpath %.c $(sort $(dir $(c_sources)))
vpath %.cpp $(sort $(dir $(cpp_sources)))

dep_dir = $(build_dir)/.deps
deps = $(addprefix $(dep_dir)/,$(notdir $(c_sources:.c=.d)) $(notdir $(cpp_sources:.cpp=.d)))

all: $(build_dir)/$(target)

$(build_dir)/$(target): $(objects) Makefile
	$(cpp_compiler) $(objects) $(linker_flags) -o $@

# Compile C sources with dependency generation
$(build_dir)/%.o: %.c Makefile | $(build_dir) $(dep_dir)
	$(c_compiler) -c $(compiler_flags) -MMD -MP -MF $(dep_dir)/$*.d $< -o $@

# Compile C++ sources with dependency generation
$(build_dir)/%.o: %.cpp Makefile | $(build_dir) $(dep_dir)
	$(cpp_compiler) -c $(compiler_flags) -MMD -MP -MF $(dep_dir)/$*.d $< -o $@

# Create build and dependency directories
$(build_dir) $(dep_dir):
	@mkdir -p $@

# Include auto-generated dependencies
-include $(deps)

run: all
	@$(build_dir)/$(target)

clean:
	rm -rf $(build_dir)

.PHONY: all run clean

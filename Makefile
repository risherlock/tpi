target = main
build_dir = build

cpp_sources = \
main.cpp \

c_sources = \

includes = \

defines =

libs = -lserialport
lib_dir =

c_compiler = gcc
cpp_compiler = g++

linker_flags = $(lib_dir) $(libs) -g
compiler_flags = -g -O3 -Wall -Wextra -Werror -lm $(includes) $(defines)

objects += $(addprefix $(build_dir)/,$(notdir $(c_sources:.c=.o)))
vpath %.c $(sort $(dir $(c_sources)))

objects += $(addprefix $(build_dir)/,$(notdir $(cpp_sources:.cpp=.o)))
vpath %.cpp $(sort $(dir $(cpp_sources)))

all: $(build_dir)/$(target)

$(build_dir)/$(target): $(objects) Makefile
	$(cpp_compiler) $(objects) $(linker_flags) -o $@

$(build_dir)/%.o: %.c Makefile | $(build_dir)
	$(c_compiler) -c $(compiler_flags) $< -o $@

$(build_dir)/%.o: %.cpp Makefile | $(build_dir)
	$(cpp_compiler) -c $(compiler_flags) $< -o $@

$(build_dir):
	@mkdir -p $(build_dir)

run: all
	@$(build_dir)/$(target)

compiler = g++.exe
debug_compiler_flags = -Werror -Wall -fdiagnostics-color=always -g -std=c++23 -O0 -march=native
production_compiler_flags = -Werror -Wall -fdiagnostics-color=always -std=c++23 -O3 -march=native

src = wordle_engine_core

letter = letter.cpp
wordle = wordle.cpp
play_wordle = play_wordle.cpp
bit_finder = bit_finder.cpp

modules = \
	$(src)/wordle.cpp	\
	$(src)/letter.cpp	\

objects = \
	wordle.o	\
	letter.o	\

all: prod_compile_wordle_engine prod_compile_play prod_compile_bit_finder

dev_compile_core: $(modules)
	$(compiler) $(debug_compiler_flags) -c $^

prod_compile_core: $(modules)
	$(compiler) $(production_compiler_flags) -c $^

dev_compile_play: $(objects)
	$(compiler) $(debug_compiler_flags) $(play_wordle) -o ./play.exe $(objects)

prod_compile_play: $(objects)
	$(compiler) $(production_compiler_flags) $(play_wordle) -o ./play.exe $(objects)

dev_compile_bit_finder: $(objects)
	$(compiler) $(debug_compiler_flags) $(bit_finder) -o ./initial_bit_finder.exe $(objects)

prod_compile_bit_finder: $(objects)
	$(compiler) $(production_compiler_flags) $(bit_finder) -o ./initial_bit_finder.exe $(objects)

run_bit_finder: initial_bit_finder.exe
	initial_bit_finder.exe

run: play.exe
	play.exe

test_bit_finder: dev_compile_wordle_engine dev_compile_bit_finder
	initial_bit_finder.exe

test: dev_compile_wordle_engine dev_compile_play
	play.exe

clean:
	del *.exe
	del *.o

dev_test_recompile: dev_compile_core
	$(compiler) $(debug_compiler_flags) test.cpp -o test.exe $(objects)

prod_test_recompile: prod_compile_core
	$(compiler) $(debug_compiler_flags) test.cpp -o test.exe $(objects)

test_time: $(objects)
	$(compiler) $(debug_compiler_flags) test.cpp -o test.exe $(objects)

compiler = g++.exe
debug_compiler_flags = -Werror -Wall -fdiagnostics-color=always -g -std=c++23 -O0
production_compiler_flags = -Werror -Wall -fdiagnostics-color=always -std=c++23 -O3

wordle = wordle.cpp
wordle_object = wordle.o
play_wordle = play_wordle.cpp
bit_finder = bit_finder.cpp

all: prod_compile_wordle_engine prod_compile_play prod_compile_bit_finder

dev_compile_wordle_engine: $(wordle) wordle.h
	$(compiler) $(debug_compiler_flags) -c $(wordle) -o $(wordle_object)

prod_compile_wordle_engine: $(wordle) wordle.h
	$(compiler) $(production_compiler_flags) -c $(wordle) -o $(wordle_object)

dev_compile_play: $(wordle_object)
	$(compiler) $(debug_compiler_flags) $(play_wordle) -o ./play.exe $(wordle_object)

prod_compile_play: $(wordle_object)
	$(compiler) $(production_compiler_flags) $(play_wordle) -o ./play.exe $(wordle_object)

dev_compile_bit_finder: $(wordle_object)
	$(compiler) $(debug_compiler_flags) $(bit_finder) -o ./initial_bit_finder.exe $(wordle_object)

prod_compile_bit_finder: $(wordle_object)
	$(compiler) $(production_compiler_flags) $(bit_finder) -o ./initial_bit_finder.exe $(wordle_object)

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

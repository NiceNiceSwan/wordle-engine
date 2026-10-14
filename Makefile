compiler = g++.exe
compiler_flags = -fdiagnostics-color=always -g -std=c++23

wordle = wordle.cpp
wordle_object = wordle.o
play_wordle = play_wordle.cpp
bit_finder = bit_finder.cpp

all: compile_wordle_prod compile_bit_finder_prod

dev_wordle.o: $(wordle) wordle.h
	$(compiler) $(compiler_flags) -O0 -c $(wordle) -o $(wordle_object)

prod_wordle.o: $(wordle) wordle.h
	$(compiler) $(compiler_flags) -O3 -c $(wordle) -o $(wordle_object)

compile_wordle_prod: $(wordle_object)
	$(compiler) $(compiler_flags) -O3 $(play_wordle) -o ./play.exe $(wordle_object)

compile_wordle_dev: $(wordle_object)
	$(compiler) $(compiler_flags) -O0 $(play_wordle) -o ./play.exe $(wordle_object)

compile_bit_finder_dev: $(wordle_object)
	$(compiler) $(compiler_flags) -O0 $(bit_finder) -o ./initial_bit_finder.exe $(wordle_object)

compile_bit_finder_prod: $(wordle_object)
	$(compiler) $(compiler_flags) -O3 $(bit_finder) -o ./initial_bit_finder.exe $(wordle_object)

run_bit_finder: initial_bit_finder.exe
	initial_bit_finder.exe

run: play.exe
	play.exe

test_bit_finder: dev_wordle.o compile_bit_finder_dev
	initial_bit_finder.exe

test: dev_wordle.o compile_wordle_dev
	play.exe


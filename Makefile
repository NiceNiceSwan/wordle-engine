compiler = g++.exe
debug_compiler_flags = -fdiagnostics-color=always -g -std=c++23 -O0
production_compiler_flags = -fdiagnostics-color=always -std=c++23 -O3

wordle = wordle.cpp
wordle_object = wordle.o
play_wordle = play_wordle.cpp
bit_finder = bit_finder.cpp

all: compile_wordle_prod compile_bit_finder_prod

dev_wordle.o: $(wordle) wordle.h
	$(compiler) $(debug_compiler_flags) -c $(wordle) -o $(wordle_object)

prod_wordle.o: $(wordle) wordle.h
	$(compiler) $(production_compiler_flags) -c $(wordle) -o $(wordle_object)

compile_wordle_dev: $(wordle_object)
	$(compiler) $(debug_compiler_flags) $(play_wordle) -o ./play.exe $(wordle_object)

compile_wordle_prod: $(wordle_object)
	$(compiler) $(production_compiler_flags) $(play_wordle) -o ./play.exe $(wordle_object)

compile_bit_finder_dev: $(wordle_object)
	$(compiler) $(debug_compiler_flags) $(bit_finder) -o ./initial_bit_finder.exe $(wordle_object)

compile_bit_finder_prod: $(wordle_object)
	$(compiler) $(production_compiler_flags) $(bit_finder) -o ./initial_bit_finder.exe $(wordle_object)

run_bit_finder: initial_bit_finder.exe
	initial_bit_finder.exe

run: play.exe
	play.exe

test_bit_finder: dev_wordle.o compile_bit_finder_dev
	initial_bit_finder.exe

test: dev_wordle.o compile_wordle_dev
	play.exe

clean:
	del *.exe
	del *.o

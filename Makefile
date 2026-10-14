all: compile

compile: 
	C:/msys64/ucrt64/bin/g++.exe -fdiagnostics-color=always -O0 -g -std=c++23 ./play_wordle.cpp -o ./play_wordle.exe

run: compile
	./play_wordle.exe

test: compile
	./play_wordle.exe


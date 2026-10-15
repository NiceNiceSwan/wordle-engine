#include <iostream>
#include <chrono>

#include "wordle_engine_core/wordle.h"

using namespace std;
using namespace chrono;

typedef std::chrono::microseconds time_resolution;

int main()
{
	string_vector words = Wordle::get_words_list();
	Wordle::all_words(words);
	
	string word = "wider";
	char_vector word_vector(word.begin(), word.end());
	string pattern = "gbbyb";
	char_vector pattern_vector(pattern.begin(), pattern.end());
	
	Wordle game(words);

	auto start = steady_clock::now();
	game.filter_words(&word_vector, &pattern_vector);
	auto stop = steady_clock::now();
	time_resolution duration = duration_cast<time_resolution>(stop - start);
	cout << "All combinations time: " << duration << endl;
	cout << "Remaining solutions: " << game.words()->size() << endl;


	return 0;
}

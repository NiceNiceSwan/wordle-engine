#include <iostream>
#include <chrono>

#include "wordle_engine_core/wordle.h"

using namespace std;
using namespace chrono;

typedef std::chrono::microseconds time_resolution;

int main()
{
	auto start = steady_clock::now();
	auto stop = steady_clock::now();
	time_resolution duration;

	string_vector words = Wordle::get_words_list();
	Wordle::all_words(words);
	
	Wordle game(words);
	// string_vector* game_state_words;
	// double information;

	string word = "tares";
	char_vector word_vector(word.begin(), word.end());
	string pattern = "bbbyb";
	char_vector pattern_vector(pattern.begin(), pattern.end());

	// game_state_words = game.words();
	// start = steady_clock::now();
	// information = Wordle::simulate_word_probabilities_base(word, game_state_words);
	// stop = steady_clock::now();
	// duration = duration_cast<time_resolution>(stop - start);
	// cout << "Simulating word list for word " << word << " took: " << duration << endl;
	// cout << "Information: " << information << endl;

	start = steady_clock::now();
	game.filter_words(&word_vector, &pattern_vector);
	stop = steady_clock::now();
	duration = duration_cast<time_resolution>(stop - start);
	cout << "Filtering word list for word " << word << ", pattern: " << pattern << " took: " << duration << endl;
	cout << "Remaining solutions: " << game.words()->size() << endl;
/*
	word = "neele";
	word_vector = char_vector(word.begin(), word.end());
	pattern = "bbgbg";
	pattern_vector = char_vector(pattern.begin(), pattern.end());

	game_state_words = game.words();
	information = Wordle::simulate_word_probabilities_base(word, game_state_words);
	cout << "Information: " << information << endl;
	game.filter_words(&word_vector, &pattern_vector);
	cout << "Remaining solutions: " << game.words()->size() << endl;

	word = "peece";
	word_vector = char_vector(word.begin(), word.end());
	pattern = "bbbgg";
	pattern_vector = char_vector(pattern.begin(), pattern.end());

	game.filter_words(&word_vector, &pattern_vector);
	cout << "Remaining solutions: " << game.words()->size() << endl;
*/
	return 0;
}

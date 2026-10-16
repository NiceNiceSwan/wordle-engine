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
	
	Wordle game(words);
	string_vector* game_state_words;
	double information;

	string word = "tares";
	char_vector word_vector(word.begin(), word.end());
	string pattern = "bbbyb";
	char_vector pattern_vector(pattern.begin(), pattern.end());

	game_state_words = game.words();
	information = Wordle::simulate_word_probabilities_base(word, game_state_words);
	cout << "Information: " << information << endl;
	game.filter_words(&word_vector, &pattern_vector);
	cout << "Remaining solutions: " << game.words()->size() << endl;

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

	return 0;
}

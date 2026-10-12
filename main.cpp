#include <iostream>
#include <fstream>

#include "game_state.h"

using namespace std;

string_vector get_words_list();

int main()
{
	string_vector words = get_words_list();
	string word;
	string pattern;
	char_vector word_vector;
	char_vector pattern_vector;

	Game_state game_state(words);

	string_vector* game_state_words;

	for (size_t guess = 0; guess < 6; guess++)
	{
		cout << "Please input word:\n";
		cin >> word;
		word_vector = char_vector(word.begin(), word.end());
		cout << "Please input pattern:\n";
		cin >> pattern;
		pattern_vector = char_vector(pattern.begin(), pattern.end());

		game_state.filter_words(word_vector, pattern_vector);

		game_state_words = game_state.words();

		for (size_t index = 0; index < game_state_words->size(); index++)
		{
			cout << (*game_state_words)[index] << ", ";
		}
		cout << endl;
	}
	

	return 0;
}

string_vector get_words_list()
{
	string_vector words;
	string file_line;
	string word;
	ifstream file("word_list_2.txt");

	getline(file, file_line);

	regex delimiter("\",\"");

	sregex_token_iterator iterator(file_line.begin(), file_line.end(), delimiter, -1);
	sregex_token_iterator end;

	while (iterator != end)
	{
		words.push_back(*iterator);
		++iterator;
	}

	file.close();

	return words;
}

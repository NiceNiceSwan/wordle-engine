#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <cmath>

#define MAX_PATTERN_COMBINATIONS pow(3, 5)

#include "game_state.h"

using namespace std;

string_vector get_words_list(string filename = "word_list_2.txt");

void play_wordle();

void get_initial_bit_count();

double simulate_word_probabilities(string word, promise<double> promise);

int main()
{
	get_initial_bit_count();

	return 0;
}

void play_wordle()
{
	string_vector words = get_words_list();
	string word;
	string pattern;
	char_vector word_vector;
	char_vector pattern_vector;

	Game_state game_state(words);

	/// pointer to the _words variable from inside of game_state
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
}

void get_initial_bit_count()
{
	string_vector words = get_words_list();
	string word;

	Game_state::all_words(words);

	vector<thread> threads;
	vector<promise<double>> promises;
	vector<future<double>> futures;
	threads.reserve(words.size());
	promises.resize(words.size());
	futures.resize(words.size());

	for (size_t word_index = 0; word_index < words.size(); word_index++)
	{
		word = words[word_index];
		futures[word_index] = promises[word_index].get_future();
		threads.push_back(thread(simulate_word_probabilities, word, move(promises[word_index])));
	}
	
	double information;
	for (size_t index = 0; index < threads.size(); index++)
	{
		threads[index].join();
		information = futures[index].get();
		cout << "Word: " << words[index] << ", information: " << information << " bits\n";
	}
	
}

double simulate_word_probabilities(string word, promise<double> promise)
{
	char_vector word_vector(word.begin(), word.end());
	string pattern;
	char_vector pattern_vector;
	double total_bits_amount = 0;
	double probability;
	double information;
	for (unsigned short int combination = 0; combination < MAX_PATTERN_COMBINATIONS; combination++)
	{
		Game_state game_state;
		pattern = Game_state::convert_number_to_combination(combination);
		pattern_vector = char_vector(pattern.begin(), pattern.end());

		game_state.filter_words(word_vector, pattern_vector);
		string_vector* remaining_words = game_state.words();

		probability = (double)remaining_words->size() / Game_state::word_count();
		information = -log2(probability);
		total_bits_amount += probability * information;
	}
	double final_information_amount = total_bits_amount / Game_state::word_count();
	promise.set_value(final_information_amount);
	return final_information_amount;
}

string_vector get_words_list(string filename)
{
	string_vector words;
	string file_line;
	string word;
	ifstream file(filename);

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

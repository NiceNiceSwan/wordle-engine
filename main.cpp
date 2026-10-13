#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <cmath>
#include <chrono>
#include <regex>

#define MAX_PATTERN_COMBINATIONS pow(3, 5)
#define INPUT_FILE_NAME "word_list_2.txt"
#define OUTPUT_FILE_NAME "first_guess_bits.txt"

#include "game_state.h"

using namespace std;
using namespace chrono;

void initialize();

string_vector get_words_list(string filename = INPUT_FILE_NAME);

void play_wordle();

void get_initial_bit_count();

void tests();

double simulate_word_probabilities_base(string word, string_vector* words);

double simulate_word_probabilities(string word, promise<double> promise, string_vector* words);

int main()
{
	initialize();
	get_initial_bit_count();
	// play_wordle();
	// tests();

	return 0;
}

void initialize()
{
	string_vector words = get_words_list();

	Game_state::all_words(words);
}

void play_wordle()
{
	string_vector words = get_words_list();
	string word;
	string pattern;
	char_vector word_vector;
	char_vector pattern_vector;

	Game_state game_state;

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

		game_state.filter_words(&word_vector, &pattern_vector);

		game_state_words = game_state.words();

		vector<thread> threads;
		vector<promise<double>> promises;
		vector<future<double>> futures;
		vector<double> informations;
		threads.resize(words.size());
		promises.resize(words.size());
		futures.resize(words.size());
		informations.resize(words.size());
		for (size_t index = 0; index < words.size(); index++)
		{
			word = words[index];
			futures[index] = promises[index].get_future();
			threads[index] = thread(simulate_word_probabilities, word, move(promises[index]), game_state_words);
			
		}
		vector<double> highest_informations(3, 0);
		vector<int> highest_info_indexes(3, 0);
		double highest_information = 0;
		int highest_info_index = 0;
		for (size_t index = 0; index < words.size(); index++)
		{
			threads[index].join();
			informations[index] = futures[index].get();
			if (informations[index] > highest_informations[0])
			{
				highest_informations[0] = informations[index];
				highest_info_indexes[0] = index;
				continue;
			}
			if (informations[index] > highest_informations[1])
			{
				highest_informations[1] = informations[index];
				highest_info_indexes[1] = index;
				continue;
			}
			if (informations[index] > highest_informations[2])
			{
				highest_informations[2] = informations[index];
				highest_info_indexes[2] = index;
				continue;
			}
			
			// cout << "Word: " << words[index] << ", information: " << informations[index] << " bits\n";
		}
		for (size_t index = 0; index < game_state_words->size(); index++)
		{
			cout << game_state_words->at(index) << ", ";
		}
		cout << endl;
		for (size_t index = 0; index < highest_informations.size(); index++)
		{
			cout << "Highest info word " << index << ": " << words[highest_info_indexes[index]] << ", information: " << highest_informations[index] << endl;
		}
		
		
	}
}

void get_initial_bit_count()
{
	string_vector words = get_words_list();
	string word;

	vector<thread> threads;
	vector<promise<double>> promises;
	vector<future<double>> futures;
	threads.resize(words.size());
	promises.resize(words.size());
	futures.resize(words.size());

	double information;

	cout << "Begginning thread creation\n";

	auto calculation_start = high_resolution_clock::now();

	for (size_t index = 0; index < words.size(); index++)
	{
		word = words[index];
		futures[index] = promises[index].get_future();
		threads[index] = thread(simulate_word_probabilities, word, move(promises[index]), Game_state::all_words());
		if (index % 100 == 0)
		{
			cout << "Finished creating " << index << " (" << (double)index / words.size() << "%) threads\n";
		}
	}

	cout << "Finished creating threads\n";

	ofstream file(OUTPUT_FILE_NAME);

	for (size_t index = 0; index < words.size(); index++)
	{
		threads[index].join();
		information = futures[index].get();
		file << words[index] << "," << information << ",\n";
	}

	file.close();
	
	auto calculation_end = high_resolution_clock::now();
	milliseconds calculation_duration = duration_cast<milliseconds>(calculation_end - calculation_start);
	cout << "Calculating everything took: " << calculation_duration << endl;
}

void tests()
{
	string word = "crane";
	char_vector word_vector(word.begin(), word.end());
	string pattern = "bgbgy";
	char_vector pattern_vector = char_vector(pattern.begin(), pattern.end());

    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration;

	cout << simulate_word_probabilities_base(word, Game_state::all_words()) << endl;
	Game_state game_state;
	// game_state.filter_words(&word_vector, &pattern_vector);
	// cout << game_state.words()->size() << endl;


	stop = std::chrono::high_resolution_clock::now();
	duration = duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Analyzing pattern took: " << duration << std::endl;
}

double simulate_word_probabilities_base(string word, string_vector* words)
{
	char_vector word_vector(word.begin(), word.end());
	string pattern;
	char_vector pattern_vector;
	double total_bits_amount = 0;
	double probability;
	double information;
	for (unsigned short int combination = 0; combination < MAX_PATTERN_COMBINATIONS; combination++)
	{
		Game_state game_state(*words);
		pattern = Game_state::convert_number_to_combination(combination);
		pattern_vector = char_vector(pattern.begin(), pattern.end());

		game_state.filter_words(&word_vector, &pattern_vector);
		string_vector* remaining_words = game_state.words();

		probability = (double)remaining_words->size() / words->size();
		if (probability == 0)
		{
			continue;
		}
		
		information = -log2(probability);
		total_bits_amount += probability * information;
	}
	return total_bits_amount;
}

double simulate_word_probabilities(string word, promise<double> promise, string_vector* words)
{
	double value = simulate_word_probabilities_base(word, words);
	promise.set_value(value);
	return value;
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

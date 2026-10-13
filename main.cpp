#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <cmath>
#include <chrono>
#include <regex>

#define MAX_PATTERN_COMBINATIONS pow(3, 5)
#define MAX_THREADS 100
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

double simulate_word_probabilities_base(string word);

double simulate_word_probabilities(string word, promise<double> promise);

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

		auto start = high_resolution_clock::now();
		auto stop = high_resolution_clock::now();
		start = high_resolution_clock::now();
		game_state.filter_words(&word_vector, &pattern_vector);
		stop = high_resolution_clock::now();

		microseconds duration = duration_cast<microseconds>(stop - start);
		cout << "Calculations took: " << duration << endl;

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

	double information;
	int repeats = ceil((double)words.size() / MAX_THREADS);

	ofstream output_file(OUTPUT_FILE_NAME, ofstream::out | ofstream::trunc);
	output_file.close();

	auto calculation_start = high_resolution_clock::now();

	for (size_t repeat = 0; repeat < repeats; repeat++)
	{
		threads.resize(MAX_THREADS);
		promises.resize(MAX_THREADS);
		futures.resize(MAX_THREADS);
		int starting_index = MAX_THREADS * repeat;
		int end_index = MAX_THREADS * (repeat + 1);
		int threads_to_create = MAX_THREADS;
		if (end_index > words.size())
		{
			threads_to_create = MAX_THREADS - (end_index - words.size());
		}
		
		auto start = high_resolution_clock::now();

		for (size_t index = 0; index < threads_to_create; index++)
		{
			word = words[starting_index + index];
			futures[index] = promises[index].get_future();
			threads[index] = thread(simulate_word_probabilities, word, move(promises[index]));
		}
		
		cout << "Thread batch " << repeat << " creation finished." << endl;

		ofstream file(OUTPUT_FILE_NAME, ios::app);

		for (size_t index = 0; index < threads_to_create; index++)
		{
			threads[index].join();
			information = futures[index].get();
			// cout << "Word: " << words[starting_index + index] << ", information: " << information << " bits\n";
			file << words[starting_index + index] << "," << information << ",\n";
		}

		file.close();
		auto stop = high_resolution_clock::now();
		milliseconds duration = duration_cast<milliseconds>(stop - start);
		cout << "Calculating batch took: " << duration << endl;

		threads.clear();
		futures.clear();
		promises.clear();
	}
	
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

	cout << simulate_word_probabilities_base(word) << endl;
	Game_state game_state;
	// game_state.filter_words(&word_vector, &pattern_vector);
	// cout << game_state.words()->size() << endl;


	stop = std::chrono::high_resolution_clock::now();
	duration = duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Analyzing pattern took: " << duration << std::endl;
}

double simulate_word_probabilities_base(string word)
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

		game_state.filter_words(&word_vector, &pattern_vector);
		string_vector* remaining_words = game_state.words();

		probability = (double)remaining_words->size() / Game_state::word_count();
		if (probability == 0)
		{
			continue;
		}
		
		information = -log2(probability);
		total_bits_amount += probability * information;
	}
	return total_bits_amount;
}

double simulate_word_probabilities(string word, promise<double> promise)
{
	double value = simulate_word_probabilities_base(word);
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

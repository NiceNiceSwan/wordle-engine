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
#define SORTED_OUTPUT_FILE_NAME "first_guess_bits_sorted.txt"

#include "wordle.h"

using namespace std;
using namespace chrono;

void initialize();

string_vector get_words_list(string filename = INPUT_FILE_NAME);

void play_wordle();

void get_initial_bit_count();

void tests();

void sort_bit_list();

int main()
{
	initialize();
	// get_initial_bit_count();
	play_wordle();
	// tests();
	// sort_bit_list();

	return 0;
}

void initialize()
{
	string_vector words = get_words_list();

	Wordle::all_words(words);
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
		threads[index] = thread(simulate_word_probabilities, word, move(promises[index]), Wordle::all_words());
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
		file << words[index] << "," << information << "\n";
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

	cout << simulate_word_probabilities_base(word, Wordle::all_words()) << endl;
	Wordle game_state;
	// game_state.filter_words(&word_vector, &pattern_vector);
	// cout << game_state.words()->size() << endl;


	stop = std::chrono::high_resolution_clock::now();
	duration = duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Analyzing pattern took: " << duration << std::endl;
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

bool comparator(pair<string, double>& pair_1, pair<string, double>& pair_2)
{
	return pair_1.second < pair_2.second;
}

void sort_bit_list()
{
	ifstream input_file(OUTPUT_FILE_NAME);

	string word;
	string info_string;
	double information;

	map<string, double> word_information;
	vector<pair<string, double>> pair_vector;

	while (!input_file.eof())
	{
		getline(input_file, word, ',');
		getline(input_file, info_string);
		information = stod(info_string);
		pair_vector.push_back(pair(word, information));
	}

	input_file.close();
	ofstream output_file(SORTED_OUTPUT_FILE_NAME);
	
	sort(pair_vector.begin(), pair_vector.end(), comparator);

	for (size_t index = 0; index < pair_vector.size(); index++)
	{
		// cout << pair_vector[index].first << ": " << pair_vector[index].second << " bits\n";
		output_file << pair_vector[index].first << ',' << pair_vector[index].second << "\n";
	}
	
	output_file.close();
}

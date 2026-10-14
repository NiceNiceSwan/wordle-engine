#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <chrono>

#include "wordle.h"

#define OUTPUT_FILE_NAME "first_guess_bits.txt"
#define SORTED_OUTPUT_FILE_NAME "first_guess_bits_sorted.txt"

typedef std::chrono::milliseconds time_resolution;

void get_initial_bit_count();
void sort_bit_list();

int main()
{
	get_initial_bit_count();
	sort_bit_list();

	return 0;
}

void get_initial_bit_count()
{
	string_vector words = Wordle::get_words_list();
	std::string word;

	std::vector<std::thread> threads;
	std::vector<std::promise<double>> promises;
	std::vector<std::future<double>> futures;
	threads.resize(words.size());
	promises.resize(words.size());
	futures.resize(words.size());

	double information;

	std::cout << "Begginning thread creation\n";

	auto calculation_start = std::chrono::high_resolution_clock::now();

	for (size_t index = 0; index < words.size(); index++)
	{
		word = words[index];
		futures[index] = promises[index].get_future();
		threads[index] = std::thread(Wordle::simulate_word_probabilities, word, move(promises[index]), Wordle::all_words());
		if (index % 100 == 0)
		{
			std::cout << "Finished creating " << index << " (" << (double)index / words.size() << "%) threads\n";
		}
	}

	std::cout << "Finished creating threads\n";

	std::ofstream file(OUTPUT_FILE_NAME);

	for (size_t index = 0; index < words.size(); index++)
	{
		threads[index].join();
		information = futures[index].get();
		file << words[index] << "," << information << "\n";
	}

	file.close();
	
	auto calculation_end = std::chrono::high_resolution_clock::now();
	time_resolution calculation_duration = duration_cast<time_resolution>(calculation_end - calculation_start);
	std::cout << "Calculating bit counts took: " << calculation_duration << std::endl;
}

bool comparator(std::pair<std::string, double>& pair_1, std::pair<std::string, double>& pair_2)
{
	return pair_1.second < pair_2.second;
}

void sort_bit_list()
{
	std::ifstream input_file(OUTPUT_FILE_NAME);

	std::string word;
	std::string info_string;
	double information;

	std::vector<std::pair<std::string, double>> pair_vector;

	std::cout << "Reading from initial file to begin sorting.\n";
	while (!input_file.eof())
	{
		getline(input_file, word, ',');
		getline(input_file, info_string);
		information = stod(info_string);
		pair_vector.push_back(std::pair(word, information));
	}

	input_file.close();
	std::cout << "Finished reading from file.\nBeginning to sort.\n";
	std::ofstream output_file(SORTED_OUTPUT_FILE_NAME);
	
	sort(pair_vector.begin(), pair_vector.end(), comparator);

	for (size_t index = 0; index < pair_vector.size(); index++)
	{
		// cout << pair_vector[index].first << ": " << pair_vector[index].second << " bits\n";
		output_file << pair_vector[index].first << ',' << pair_vector[index].second << "\n";
	}
	
	output_file.close();
	std::cout << "Finished sorting.\n";
}

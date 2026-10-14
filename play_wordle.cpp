#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <future>

#include "wordle.h"

using namespace std;

void play_wordle();

int main()
{
	play_wordle();

	return 0;
}

void play_wordle()
{
	string_vector words = Wordle::get_words_list();
	Wordle::all_words(words);

	string word;
	string pattern;
	char_vector word_vector;
	char_vector pattern_vector;

	Wordle game_state;

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

		vector<thread> threads(words.size());
		vector<promise<double>> promises(words.size());
		vector<future<double>> futures(words.size());
		vector<double> informations(words.size());

		for (size_t index = 0; index < words.size(); index++)
		{
			word = words[index];
			futures[index] = promises[index].get_future();
			threads[index] = thread(Wordle::simulate_word_probabilities, word, move(promises[index]), game_state_words);
			
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
		}
		if (game_state_words->size() <= 1)
		{
			if (game_state_words->size() == 1)
			{
				cout << "Final word: " << game_state_words->front() << endl;
			}
			return;
		}
		
		for (size_t index = 0; index < game_state_words->size(); index++)
		{
			cout << game_state_words->at(index) << ", ";
		}
		cout << endl;
		for (size_t index = 0; index < highest_informations.size(); index++)
		{
			cout << "Highest info word " << index + 1 << ": " << words[highest_info_indexes[index]] << ", information: " << highest_informations[index] << endl;
		}
		
		
	}
}

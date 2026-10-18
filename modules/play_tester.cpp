#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <future>

#include "../wordle_engine_core/wordle.h"

#define ANSWERS_FILENAME "wordle_answers.txt"
/// have to be <= this many guesses to win
#define MAX_WINNABLE_GUESSES 6
/// guesses allowed before we declare that the game is lost
#define MAX_TOTAL_GUESSES 10

enum game_results
{
	/// @brief outputted when no solution was found
	GAME_LOST = -1,
	WENT_BEYOND_6 = 0,
	GAME_WON = 1
};

typedef std::vector<std::string> string_vector;

using namespace std;
using namespace chrono;

/// @brief plays as many games as there are answers.
void play_all_games();

/// @brief simulates a single wordle game
/// @param answer the correct answer. Must be lowercase.
/// @param initial_guess the first guess. Must be lowercase. Defaults to "tares"
/// @return item from the `game_results` enum
game_results play_game(std::string answer, std::string initial_guess = "tares");

string_vector get_answer_list(std::string filename = ANSWERS_FILENAME);

int main()
{
	string_vector answers = get_answer_list();
	string_vector words = Wordle::get_words_list();
	Wordle::all_words(words);
	/*
	/// games in which the answer was guessed in <=6 guesses
	int won_games = 0;
	/// games in which it needed >6 guesses to win
	int games_over_6 = 0;
	/// games in which the solution was never found
	int lost_games = 0;
	string guess;
	string pattern;

	for (size_t index = 0; index < answers.size(); index++)
	{
		guess = "tares";
		string answer = answers[index];
		Wordle game(words);

		pattern = Wordle::check_answer(guess, answer);
		if (pattern == "ggggg")
		{
			won_games++;
			continue;
		}
		
		char_vector guess_vector(guess.begin(), guess.end());
		char_vector pattern_vector(pattern.begin(), pattern.end());
		game.filter_words(&guess_vector, &pattern_vector);
		string_vector* remaining_words = game.words();
		if (remaining_words->size() == 0)
		{
			lost_games++;
			continue;
		}
		double most_information = 0;
		for (size_t remaining_word_index = 0; remaining_word_index < remaining_words->size(); remaining_word_index++)
		{
			
		}
		
		for (size_t guess_number = 1; guess_number < MAX_TOTAL_GUESSES; guess_number++)
		{
			
		}
		

	}
	*/

	cout << play_game("gaffe");
	return 0;
}


game_results play_game(std::string answer, std::string guess)
{
	if (answer == guess)
	{
		return game_results::GAME_WON;
	}
	
	string pattern = Wordle::check_answer(guess, answer);
	char_vector guess_vector(guess.begin(), guess.end());
	char_vector pattern_vector(pattern.begin(), pattern.end());

	Wordle game(*Wordle::all_words());
	game.filter_words(&guess_vector, &pattern_vector);

	string_vector* remaining_words = game.words();
	if (remaining_words->size() == 0)
	{
		return game_results::GAME_LOST;
	}
	string best_word;

	for (size_t guess_number = 1; guess_number < MAX_TOTAL_GUESSES; guess_number++)
	{
		double most_information = 0;
		cout << "Guess: " << guess_number << endl;
		vector<thread> threads(Wordle::all_words()->size());
		vector<promise<double>> promises(Wordle::all_words()->size());
		vector<future<double>> futures(Wordle::all_words()->size());

		for (size_t index = 0; index < Wordle::all_words()->size(); index++)
		{
			guess = Wordle::all_words()->at(index);
			futures[index] = promises[index].get_future();
			threads[index] = thread(Wordle::simulate_word_probabilities, guess, move(promises[index]), remaining_words);
			
		}

		for (size_t index = 0; index < Wordle::all_words()->size(); index++)
		{
			// cout << "Finished joining " << index << " threads, word: " << words[index] << endl;
			threads[index].join();
			double guess_information = futures[index].get();
			if (guess_information > most_information)
			{
				most_information = guess_information;
				best_word = Wordle::all_words()->at(index);
			}
		}

		guess = best_word;
		if (guess == answer)
		{
			return guess_number < MAX_WINNABLE_GUESSES ? game_results::GAME_WON : game_results::WENT_BEYOND_6;
		}
		
		pattern = Wordle::check_answer(guess, answer);
		guess_vector = char_vector(guess.begin(), guess.end());
		pattern_vector = char_vector(pattern.begin(), pattern.end());

		game.filter_words(&guess_vector, &pattern_vector);
		remaining_words = game.words();
		if (remaining_words->size() == 0)
		{
			return game_results::GAME_LOST;
		}
		if (remaining_words->size() == 1)
		{
			return guess_number+1 < MAX_WINNABLE_GUESSES ? game_results::GAME_WON : game_results::WENT_BEYOND_6;
		}
		
	}
	return game_results::GAME_LOST;
}

string_vector get_answer_list(std::string filename)
{
	string_vector answers;
	string file_line;
	ifstream file(filename);

	getline(file, file_line);
	while (getline(file, file_line))
	{
		answers.push_back(file_line);
	}
	

	file.close();

	return answers;
}

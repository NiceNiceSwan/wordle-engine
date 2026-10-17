#include <chrono>
#include <iostream>
#include <fstream>
#include <regex>
#include <cmath>

#include "wordle.h"

#define MAX_PATTERN_COMBINATIONS pow(3, 5)

string_vector Wordle::_all_words;
unsigned long long int Wordle::_word_count;

typedef std::chrono::microseconds time_resolution;

Wordle::Wordle()
{
	_words = _all_words;
	_exact_match = char_vector(LETTER_COUNT, '.');
	_letters.reserve(LETTERS_IN_ALPHABET);
	for (size_t index = 0; index < LETTERS_IN_ALPHABET; index++)
	{
		_letters.push_back(ALPHABET_INDEX_TO_ASCII(index));
	}
}

Wordle::Wordle(string_vector words)
{
	_words = words;
	_exact_match.reserve(LETTER_COUNT);
	_exact_match = char_vector(LETTER_COUNT, '.');
	_letters.reserve(LETTERS_IN_ALPHABET);
	for (size_t index = 0; index < LETTERS_IN_ALPHABET; index++)
	{
		_letters.push_back(ALPHABET_INDEX_TO_ASCII(index));
	}
}

bool Wordle::_analyze_pattern(const char_vector* word, const char_vector* pattern)
{
	for (unsigned char index = 0; index < LETTER_COUNT; index++)
	{
		char pattern_letter = pattern->at(index);
		char word_letter = word->at(index);
		switch (pattern_letter)
		{
		case 'y':
			if (_letters[ASCII_TO_ALPHABET_INDEX(word_letter)].spot_yellow(index) == IMPOSSIBLE_PATTERN)
			{
				return false;
			}
			_important_letters.insert({word_letter, &_letters[ASCII_TO_ALPHABET_INDEX(word_letter)]});
			break;
		case 'b':
			_letters[ASCII_TO_ALPHABET_INDEX(word_letter)].spot_black(index);
			break;
		case 'g':
			_letters[ASCII_TO_ALPHABET_INDEX(word_letter)].spot_green(index);
			_exact_match[index] = word_letter;
			_important_letters.insert({word_letter, &_letters[ASCII_TO_ALPHABET_INDEX(word_letter)]});
			break;
		default:
			break;
		}
	}
	return true;
}

void Wordle::_filter_word_list()
{
	string_vector filtered_list;
	filtered_list.reserve(_words.size());
	std::string word;

	for (size_t index = 0; index < _words.size(); index++)
	{
		word = _words[index];
		if (_has_forbidden_letter(&word))
		{
			continue;
		}

		filtered_list.push_back(word);
	}
	_words = filtered_list;
}

void Wordle::_reset_letter_pass(const char_vector* word)
{
	for (size_t index = 0; index < LETTER_COUNT; index++)
	{
		_letters[ASCII_TO_ALPHABET_INDEX(word->at(index))].finish_pass();
	}
}

bool Wordle::_has_forbidden_letter(const std::string* word)
{
	bool forbidden = false;
	for (size_t letter_index = 0; letter_index < LETTER_COUNT; letter_index++)
	{
		char letter_char = word->at(letter_index);
		Letter* letter = &_letters[ASCII_TO_ALPHABET_INDEX(letter_char)];
		if (_exact_match[letter_index] != '.' && _exact_match[letter_index] != letter_char)
		{
			forbidden = true;
			break;
		}
		
		if (!letter->has_been_spotted())
		{
			continue;
		}
		
		if (letter->forbidden_spot(letter_index))
		{
			forbidden = true;
			break;
		}
		letter->increment_spot_counter();
		if (letter->pass_spot_counter() > letter->maximum_occurences())
		{
			forbidden = true;
			break;
		}
	}

	for (auto letter: _important_letters)
	{
		if (letter.second->pass_spot_counter() < letter.second->minimum_occurences())
		{
			forbidden = true;
		}
	}
	for (size_t letter_index = 0; letter_index < LETTER_COUNT; letter_index++)
	{
		char letter_char = word->at(letter_index);
		Letter* letter = &_letters[ASCII_TO_ALPHABET_INDEX(letter_char)];
		if (letter->pass_spot_counter() < letter->minimum_occurences())
		{
			forbidden = true;
		}
		letter->finish_pass();
	}
	
	return forbidden;
}

std::string Wordle::convert_number_to_combination(unsigned short int number)
{
	std::string result;
	unsigned short int remainder;
	while (number > 0)
	{
		remainder = number % 3;
		switch (remainder)
		{
		case BLACK:
			result += "b";
			break;
		case YELLOW:
			result += "y";
			break;
		case GREEN:
			result += "g";
			break;
		}
		number /= 3;
	}
	int missing_letters = LETTER_COUNT - result.size();
	if (missing_letters > 0)
	{
		result.insert(result.size(), missing_letters, 'b');
	}

	return result;
}

bool Wordle::filter_words(const char_vector* word, const char_vector* pattern)
{
	// auto start = std::chrono::steady_clock::now();
	// auto stop = std::chrono::steady_clock::now();
	// time_resolution duration;

	if (!_analyze_pattern(word, pattern))
	{
		return false;
	}

	_reset_letter_pass(word);

	// start = std::chrono::steady_clock::now();
	_filter_word_list();
	// stop = std::chrono::steady_clock::now();
	// duration = std::chrono::duration_cast<time_resolution>(stop - start);
	// std::cout << "Filtering word list took: " << duration << std::endl;
	return true;
}

string_vector Wordle::get_words_list(std::string filename)
{
	string_vector words;
	std::string file_line;
	std::string word;
	std::ifstream file(filename);

	std::getline(file, file_line);

	std::regex delimiter("\",\"");

	std::sregex_token_iterator iterator(file_line.begin(), file_line.end(), delimiter, -1);
	std::sregex_token_iterator end;

	while (iterator != end)
	{
		words.push_back(*iterator);
		++iterator;
	}

	file.close();

	return words;
}

double Wordle::simulate_word_probabilities_base(std::string word, string_vector* words)
{
	char_vector word_vector(word.begin(), word.end());
	std::string pattern;
	char_vector pattern_vector;
	double total_bits_amount = 0;
	double probability;
	double total_probability = 0;
	double information;

	for (unsigned short int combination = 0; combination < MAX_PATTERN_COMBINATIONS; combination++)
	{
		Wordle game_state(*words);

		pattern = Wordle::convert_number_to_combination(combination);
		pattern_vector = char_vector(pattern.begin(), pattern.end());

		if (!game_state.filter_words(&word_vector, &pattern_vector))
		{
			continue;	// means the pattern is impossible for the given word
		}

		string_vector* remaining_words = game_state.words();
		int remaining_words_count = remaining_words->size();
		if (remaining_words_count == 0)
		{
			continue;
		}

		probability = (double)remaining_words_count / words->size();
		total_probability += probability;
		information = abs(-log2(probability));
		total_bits_amount += probability * information;
	}

	return total_bits_amount;
}

double Wordle::simulate_word_probabilities(std::string word, std::promise<double> promise, string_vector* words)
{
	double value = simulate_word_probabilities_base(word, words);
	promise.set_value(value);
	return value;
}

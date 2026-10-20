#include <chrono>
#include <iostream>
#include <fstream>
#include <regex>
#include <cmath>
#include <array>

#include "wordle.h"

#define MAX_PATTERN_COMBINATIONS 243

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
			break;
		}
	}

	std::map<char, bool> seen_letters;
	for (size_t letter_index = 0; letter_index < LETTER_COUNT; letter_index++)
	{
		char letter_char = word->at(letter_index);
		Letter* letter = &_letters[ASCII_TO_ALPHABET_INDEX(letter_char)];
		if (letter->pass_spot_counter() < letter->minimum_occurences() && !seen_letters.contains(letter_char))
		{
			forbidden = true;
		}
		seen_letters.insert({letter_char, true});
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

unsigned short int Wordle::convert_combination_to_number(std::string pattern)
{
	unsigned short int result = 0;
	for (size_t index = 0; index < LETTER_COUNT; index++)
	{
		char letter = pattern[index];
		unsigned char letter_value = 0;
		switch (letter)
		{
		case 'b':
			letter_value = 0;
			break;
		case 'y':
			letter_value = 1;
			break;
		case 'g':
			letter_value = 2;
			break;
		default:
			break;
		}
		result += letter_value * pow(3, index);
	}
	return result;
}

std::string Wordle::check_answer(std::string word, std::string answer)
{
	unsigned char filled_letters = 0;
	std::string pattern = "bbbbb";
	for (size_t answer_index = 0; answer_index < LETTER_COUNT; answer_index++)
	{
		if (word[answer_index] == answer[answer_index])
		{
			pattern[answer_index] = 'g';
			filled_letters |= INDEX_TO_BITFLAG(LETTER_COUNT - answer_index);
			continue;
		}
		for (size_t word_index = 0; word_index < LETTER_COUNT; word_index++)
		{
			if (filled_letters & INDEX_TO_BITFLAG(LETTER_COUNT - word_index))
			{
				continue;
			}

			if (answer[answer_index] == word[word_index])
			{
				pattern[word_index] = 'y';
				filled_letters |= INDEX_TO_BITFLAG(LETTER_COUNT - word_index);
				break;
			}
		}
	}

	return pattern;
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
	int pattern_index;
	char_vector pattern_vector;
	double total_bits_amount = 0;
	double probability;
	double total_probability = 0;
	double information;

	std::array<unsigned short int, MAX_PATTERN_COMBINATIONS> matching_words = {0};

	for (size_t index = 0; index < words->size(); index++)
	{
		pattern = Wordle::check_answer(word, words->at(index));
		pattern_index = Wordle::convert_combination_to_number(pattern);
		matching_words[pattern_index] += 1;
	}
	
	for (size_t index = 0; index < MAX_PATTERN_COMBINATIONS; index++)
	{
		if (matching_words[index] == 0)
		{
			continue;
		}
		
		probability = (double)matching_words[index] / words->size();
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

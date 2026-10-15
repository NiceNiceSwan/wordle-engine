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

void Wordle::_analyze_pattern(const char_vector* word, const char_vector* pattern)
{
	for (unsigned char index = 0; index < word->size(); index++)
	{
		char pattern_letter = pattern->at(index);
		char word_letter = word->at(index);
		switch (pattern_letter)
		{
		case 'y':
			// _words = _filter_yellow_letters(word_letter, index);
			_letters[ASCII_TO_ALPHABET_INDEX(word_letter)].spot_yellow(index);
			break;
		case 'b':
			// _black_letters.push_back(word_letter);
			_letters[ASCII_TO_ALPHABET_INDEX(word_letter)].clamp_occurences();
			break;
		case 'g':
			_letters[ASCII_TO_ALPHABET_INDEX(word_letter)].spot();
			_exact_match[index] = word_letter;
			break;
		default:
			break;
		}
	}
}

void Wordle::_filter_word_list()
{
	string_vector filtered_list;
	filtered_list.reserve(_words.size());
	std::string word;

	for (size_t index = 0; index < _words.size(); index++)
	{
		word = _words[index];
		if (!_does_match_exact_pattern(&word))
		{
			continue;
		}

		bool forbidden = false;
		for (size_t letter_index = 0; letter_index < LETTERS_IN_ALPHABET; letter_index++)
		{
			Letter* letter = &_letters[letter_index];
			forbidden = _has_forbidden_letter(letter, &word);
			if (forbidden)
			{
				break;
			}
		}
		if (forbidden)
		{
			continue;
		}

		filtered_list.push_back(word);
	}
	_words = filtered_list;
}

void Wordle::_reset_letter_pass()
{
	for (size_t index = 0; index < LETTERS_IN_ALPHABET; index++)
	{
		_letters[index].finish_pass();
	}
}

bool Wordle::_find_letter_in_word(const char letter, const std::string* word)
{
	for (size_t index = 0; index < word->size(); index++)
	{
		if (word->at(index) == letter)
		{
			return true;
		}
	}
	
	return false;
}

bool Wordle::_has_forbidden_letter(Letter* letter, const std::string* word)
{
	unsigned char spot_counter = 0;
	for (unsigned char index = 0; index < LETTER_COUNT; index++)
	{
		char word_letter = word->at(index);
		if (word_letter != letter->letter())
		{
			continue;
		}
		if (letter->forbidden_spot(index))
		{
			return true;
		}
		spot_counter++;
		if (spot_counter > letter->maximum_occurences())
		{
			return true;
		}
	}

	if (spot_counter < letter->minimum_occurences())
	{
		return true;
	}

	return false;
}

bool Wordle::_does_match_exact_pattern(const std::string* word)
{
	for (size_t index = 0; index < LETTER_COUNT; index++)
	{
		if (_exact_match[index] == '.')
		{
			continue;
		}
		
		if (word->at(index) != _exact_match[index])
		{
			return false;
		}
	}

	return true;
}

string_vector Wordle::_filter_yellow_letters(char letter, unsigned short int index)
{
	string_vector resulting_list;
	resulting_list.reserve(_words.size());
	std::string word;

	for (size_t word_index = 0; word_index < _words.size(); word_index++)
	{
		word = _words[word_index];
		if (!(word[index] == letter) && _find_letter_in_word(letter, &word))
		{
			resulting_list.push_back(word);
		}
	}
	
	return resulting_list;
}

string_vector Wordle::_exclude_black_letters()
{
	string_vector resulting_word_list;
	resulting_word_list.reserve(_words.size());
	bool forbidden = false;
	std::string word;
	char black_letter;

	for (size_t index = 0; index < _words.size(); index++)
	{
		word = _words[index];
		forbidden = false;
		for (size_t black_letter_index = 0; black_letter_index < _black_letters.size(); black_letter_index++)
		{
			black_letter = _black_letters[black_letter_index];
			if (_find_letter_in_word(black_letter, &word))
			{
				forbidden = true;
				break;
			}
		}
		
		if (!forbidden)
		{
			resulting_word_list.push_back(word);
		}
	}
	
	return resulting_word_list;
}

string_vector Wordle::_match_exact_patterns(string_vector words)
{
	std::string word;
	string_vector resulting_word_list;
	resulting_word_list.reserve(_words.size());
	for (size_t index = 0; index < words.size(); index++)
	{
		word = words[index];
		if (_does_match_exact_pattern(&word))
		{
			resulting_word_list.push_back(word);
		}
	}
	
	return resulting_word_list;
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

void Wordle::filter_words(const char_vector* word, const char_vector* pattern)
{
    auto start = std::chrono::steady_clock::now();
    auto stop = std::chrono::steady_clock::now();
    time_resolution duration;

	start = std::chrono::steady_clock::now();
	_analyze_pattern(word, pattern);
	stop = std::chrono::steady_clock::now();
	duration = std::chrono::duration_cast<time_resolution>(stop - start);
	std::cout << "Analyzing pattern took: " << duration << std::endl;

	_reset_letter_pass();

	_filter_word_list();
	// start = std::chrono::steady_clock::now();
	// string_vector swap_list = _exclude_black_letters();
	// stop = std::chrono::steady_clock::now();
	// duration = std::chrono::duration_cast<time_resolution>(stop - start);
	// std::cout << "Excluding black letters took: " << duration << std::endl;

	// start = std::chrono::steady_clock::now();
	// _words = _match_exact_patterns(swap_list);
	// stop = std::chrono::steady_clock::now();
	// duration = std::chrono::duration_cast<time_resolution>(stop - start);
	// std::cout << "matching exact patterns took: " << duration << std::endl;
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
	double information;
	for (unsigned short int combination = 0; combination < MAX_PATTERN_COMBINATIONS; combination++)
	{
		Wordle game_state(*words);
		pattern = Wordle::convert_number_to_combination(combination);
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

double Wordle::simulate_word_probabilities(std::string word, std::promise<double> promise, string_vector* words)
{
	double value = simulate_word_probabilities_base(word, words);
	promise.set_value(value);
	return value;
}

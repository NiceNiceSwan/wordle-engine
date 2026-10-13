#include "game_state.h"
#include <chrono>
#include <iostream>

string_vector Game_state::_all_words;
unsigned long long int Game_state::_word_count;

Game_state::Game_state()
{
	_words = _all_words;
	_exact_match = char_vector(LETTER_COUNT, '.');
}

Game_state::Game_state(string_vector words)
{
	_words = words;
	_exact_match.reserve(LETTER_COUNT);
	_exact_match = char_vector(LETTER_COUNT, '.');
}

void Game_state::_analyze_pattern(const char_vector* word, const char_vector* pattern)
{
	for (unsigned short int index = 0; index < word->size(); index++)
	{
		char pattern_letter = pattern->at(index);
		char word_letter = word->at(index);
		switch (pattern_letter)
		{
		case 'y':
			_words = _filter_yellow_letters(word_letter, index);
			break;
		case 'b':
			_black_letters.push_back(word_letter);
			break;
		case 'g':
			_exact_match[index] = word_letter;
			break;
		default:
			break;
		}
	}
}

bool Game_state::find_letter_in_word(const char letter, const std::string* word)
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

bool Game_state::does_match_exact_pattern(const std::string* word)
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

string_vector Game_state::_filter_yellow_letters(char letter, unsigned short int index)
{
	string_vector resulting_list;
	resulting_list.reserve(_words.size());
	std::string word;

	for (size_t word_index = 0; word_index < _words.size(); word_index++)
	{
		word = _words[word_index];
		if (!(word[index] == letter) && find_letter_in_word(letter, &word))
		{
			resulting_list.push_back(word);
		}
	}
	
	return resulting_list;
}

string_vector Game_state::_exclude_black_letters()
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
			if (find_letter_in_word(black_letter, &word))
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

string_vector Game_state::_match_exact_patterns(string_vector words)
{
	std::string word;
	string_vector resulting_word_list;
	resulting_word_list.reserve(_words.size());
	for (size_t index = 0; index < words.size(); index++)
	{
		word = words[index];
		if (does_match_exact_pattern(&word))
		{
			resulting_word_list.push_back(word);
		}
	}
	
	return resulting_word_list;
}

std::string Game_state::convert_number_to_combination(unsigned short int number)
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
		result.insert(0, missing_letters, 'b');
	}

	return result;
}

void Game_state::filter_words(const char_vector* word, const char_vector* pattern)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration;

	start = std::chrono::high_resolution_clock::now();
	_analyze_pattern(word, pattern);
	stop = std::chrono::high_resolution_clock::now();
	duration = duration_cast<std::chrono::microseconds>(stop - start);
	// std::cout << "Analyzing pattern took: " << duration << std::endl;

	start = std::chrono::high_resolution_clock::now();
	string_vector swap_list = _exclude_black_letters();
	stop = std::chrono::high_resolution_clock::now();
	duration = duration_cast<std::chrono::microseconds>(stop - start);
	// std::cout << "Excluding black letters took: " << duration << std::endl;

	start = std::chrono::high_resolution_clock::now();
	_words = _match_exact_patterns(swap_list);
	stop = std::chrono::high_resolution_clock::now();
	duration = duration_cast<std::chrono::microseconds>(stop - start);
	// std::cout << "matching exact patterns took: " << duration << std::endl;
}

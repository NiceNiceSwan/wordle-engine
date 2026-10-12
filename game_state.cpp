#include "game_state.h"

Game_state::Game_state()
{
	_words = _all_words;
	_exact_match = char_vector(LETTER_COUNT, '.');
}

Game_state::Game_state(string_vector words)
{
	_words = words;
	_exact_match = char_vector(LETTER_COUNT, '.');
}

void Game_state::_analyze_pattern(char_vector word, char_vector pattern)
{
	for (unsigned short int index = 0; index < word.size(); index++)
	{
		char pattern_letter = pattern[index];
		char word_letter = word[index];
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

string_vector Game_state::_filter_yellow_letters(char letter, unsigned short int index)
{
	string_vector resulting_list;
	char_vector pattern_vector(LETTER_COUNT, '.');
	pattern_vector[index] = letter;
	const std::string pattern_string(std::string(pattern_vector.begin(), pattern_vector.end()));
	const std::regex word_pattern(pattern_string);
	const std::string letter_string(1, letter);
	const std::regex letter_pattern(letter_string);
	std::string word;

	for (size_t index = 0; index < _words.size(); index++)
	{
		word = _words[index];
		bool test_1 = std::regex_search(word, letter_pattern);
		bool test_2 = !std::regex_search(word, word_pattern);
		if (test_1 && test_2)
		{
			resulting_list.push_back(word);
		}
	}
	
	return resulting_list;
}

string_vector Game_state::_exclude_black_letters()
{
	string_vector resulting_word_list;
	bool forbidden = false;
	std::string word;
	std::string black_letter;

	for (size_t index = 0; index < _words.size(); index++)
	{
		word = _words[index];
		forbidden = false;
		for (size_t black_letter_index = 0; black_letter_index < _black_letters.size(); black_letter_index++)
		{
			black_letter = _black_letters[black_letter_index];
			std::regex letter_pattern(black_letter);
			if (std::regex_search(word, letter_pattern))
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
	std::string exact_match_string(_exact_match.begin(), _exact_match.end());
	std::string word;
	string_vector resulting_word_list;
	for (size_t index = 0; index < words.size(); index++)
	{
		word = words[index];
		std::regex word_pattern(exact_match_string);
		if (std::regex_search(word, word_pattern))
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
	
	return result;
}

void Game_state::filter_words(char_vector word, char_vector pattern)
{
	_analyze_pattern(word, pattern);

	string_vector swap_list = _exclude_black_letters();

	_words = _match_exact_patterns(swap_list);
}

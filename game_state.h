#include <vector>
#include <string>
#include <regex>

#define LETTER_COUNT 5

typedef std::vector<std::string> string_vector;
typedef std::vector<char> char_vector;

class Game_state
{
private:
	/// @brief holds all the possible guess words
	string_vector _words;
	/// @brief holds all the remaining possible words
	string_vector _possible_words;
	/// @brief holds all the letters that cannot be in the solution
	char_vector _black_letters;
	/// @brief holds all the green letters and their position
	char_vector _exact_match;

	void _analyze_pattern(char_vector word, char_vector pattern);

	string_vector _filter_yellow_letters(char letter, unsigned short int index);

	string_vector _exclude_black_letters();

	string_vector _match_exact_patterns(string_vector words);

public:
	Game_state(string_vector words);

	void filter_words(char_vector word, char_vector pattern);

	string_vector* words() { return &_words; };
};

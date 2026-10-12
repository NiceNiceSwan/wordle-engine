#include <vector>
#include <string>
#include <regex>

#define LETTER_COUNT 5

#define GREEN 2
#define YELLOW 1
#define BLACK 0

typedef std::vector<std::string> string_vector;
typedef std::vector<char> char_vector;

class Game_state
{
private:
	/// @brief holds all words regardless of if they can be the solution or not
	static string_vector _all_words;
	/// @brief quick access for the total amount of words
	static unsigned long long int _word_count;
	/// @brief holds all the possible words that may be the solution
	string_vector _words;
	/// @brief holds all the letters that cannot be in the solution
	char_vector _black_letters;
	/// @brief holds all the green letters and their position
	char_vector _exact_match;

	/** @brief looks at the given word and pattern, and prepares the inner variables for filtering
	/// @param word the inputted word
	/// @param pattern the pattern given upon inputting the word
	*/
	void _analyze_pattern(char_vector word, char_vector pattern);

	/** @brief filters words based on the given yellow letter and its position
	/// @param letter the letter marked in yellow
	/// @param index the position of the letter in the word
	/// @return returns a list with all the words matching from `words()` matching the yellow letter
	*/
	string_vector _filter_yellow_letters(char letter, unsigned short int index);

	/// @brief goes through `_black_letters` and returns a list of all words from `words()` 
	/// which don't have any of the black letters
	/// @return returns a list of letters without any black letters
	string_vector _exclude_black_letters();

	/// @brief for a given list of available words, gives all that match `_exact_match`
	/// @param words list of words to check
	/// @return list of words which match the `_exact_match` pattern
	string_vector _match_exact_patterns(string_vector words);


public:
	Game_state();
	Game_state(string_vector words);

	void filter_words(char_vector word, char_vector pattern);

	// getters
	string_vector* words() { return &_words; };
	static unsigned long long int word_count() { return _word_count; };

	// setters
	static void all_words(string_vector words) {_all_words = words; _word_count = words.size(); };

	static std::string convert_number_to_combination(unsigned short int number);
};

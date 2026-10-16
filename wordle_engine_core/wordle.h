#include <vector>
#include <string>
#include <future>

#include "letter.h"

#define LETTER_COUNT 5

#define GREEN 2
#define YELLOW 1
#define BLACK 0

#define INPUT_FILE_NAME "word_list_2.txt"

typedef std::vector<std::string> string_vector;
typedef std::vector<char> char_vector;

class Wordle
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

	std::vector<Letter> _letters;

	/** @brief looks at the given word and pattern, and prepares the inner variables for filtering
	/// @param word the inputted word
	/// @param pattern the pattern given upon inputting the word */
	/// @return return true if the pattern is possible, false if it isn't
	bool _analyze_pattern(const char_vector* word, const char_vector* pattern);

	void _filter_word_list();

	void _reset_letter_pass();

	bool _find_letter_in_word(const char letter, const std::string* word);

	bool _has_forbidden_letter(Letter* letter, const std::string* word);

	/// @brief checks if word matches the exact pattern. Basically, checks if the word either has the same letter
	/// on the same place as the pattern, or the pattern has `.` on that place. Basically, implementes 
	/// a regex search. Only matches first 5 characters.
	/// @param word pointer to the word
	/// @return true if the first 5 characters are match, false otherwise
	bool _does_match_exact_pattern(const std::string* word);

	/** @brief filters words based on the given yellow letter and its position
	/// @param letter the letter marked in yellow
	/// @param index the position of the letter in the word
	/// @return returns a list with all the words matching from `words()` matching the yellow letter */
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
	Wordle();
	Wordle(string_vector words);

	// getters
	string_vector* words() { return &_words; };
	static string_vector* all_words() { return &_all_words; };
	static unsigned long long int word_count() { return _word_count; };

	// setters
	void words(string_vector words) { _words = words; };
	static void all_words(string_vector words) {_all_words = words; _word_count = words.size(); };

	// functions

	/// @return true if the pattern for the word is possible, false otherwise
	bool filter_words(const char_vector* word, const char_vector* pattern);

	static std::string convert_number_to_combination(unsigned short int number);

	static string_vector get_words_list(std::string filename = INPUT_FILE_NAME);

	static double simulate_word_probabilities_base(std::string word, string_vector* words);

	static double simulate_word_probabilities(std::string word, std::promise<double> promise, string_vector* words);
};

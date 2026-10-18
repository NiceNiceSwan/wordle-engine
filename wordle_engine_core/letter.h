#include <vector>

/// ASCII code for `a`
#define ASCII_a_INDEX 97
/// for a given lowercase char, returns its index in the alphabet
/// for example, putting in `a` here would return `0`
#define ASCII_TO_ALPHABET_INDEX(CHAR) (CHAR - ASCII_a_INDEX)
/// for a given alphabet index, returns the corresponding letter
/// for example, putting in `0` here would return `a`
#define ALPHABET_INDEX_TO_ASCII(CHAR) (CHAR + ASCII_a_INDEX)
#define LETTERS_IN_ALPHABET 26
#define IMPOSSIBLE_PATTERN true

#define INDEX_TO_BITFLAG(index) (1 << (index))

enum spotting_flags
{
	HAS_BEEN_SPOTTED = (1<<0),
	SPOTTED_BLACK_THIS_PASS = (1<<1),
};

/// @brief represents a single letter of the alphabet
class Letter
{
private:
	char _letter;
	unsigned char _minimum_occurences = 0;
	unsigned char _maximum_occurences = 5;

	/// @brief bit flag containing data on when and how the letter was spotted
	unsigned char _spot_flags = 0;
	unsigned char _pass_spot_counter = 0;

	/// @brief spots on which the letter can't be (cuz it was yellow or black). True means the letter can't go there. Is a bitflag.
	unsigned char _forbidden_spots = 0;

public:
	Letter();
	Letter(char init_letter);

	// getters
	inline char letter() const { return _letter; };
	inline unsigned char minimum_occurences() const { return _minimum_occurences; };
	inline unsigned char maximum_occurences() const { return _maximum_occurences; };
	inline bool has_been_spotted() const {return _spot_flags & spotting_flags::HAS_BEEN_SPOTTED; };
	inline unsigned char pass_spot_counter() const { return _pass_spot_counter; };
	inline bool forbidden_spot(unsigned char index) const { return _forbidden_spots & INDEX_TO_BITFLAG(index); };

	// setters

	// simple functions
	inline void increment_spot_counter() { _pass_spot_counter++; };

	// functions
	bool spot_yellow(unsigned char index);
	void spot_black(unsigned char index);
	void spot_green(unsigned char index);
	void finish_pass();
};

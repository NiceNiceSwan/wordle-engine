/// ASCII code for `a`
#define ASCII_a_INDEX 97
/// for a given lowercase char, returns its index in the alphabet
/// for example, putting in `a` here would return `0`
#define ASCII_TO_ALPHABET_INDEX(CHAR) (CHAR - ASCII_a_INDEX)
/// for a given alphabet index, returns the corresponding letter
/// for example, putting in `0` here would return `a`
#define ALPHABET_INDEX_TO_ASCII(CHAR) (CHAR + ASCII_a_INDEX)
#define LETTERS_IN_ALPHABET 26

class Letter
{
private:
	char _letter;
	unsigned char _minimum_occurences = 0;
	unsigned char _maximum_occurences = 5;

	bool spotted_this_pass = false;
	unsigned char pass_spot_counter = 0;

public:
	Letter();
	Letter(char init_letter);

	// getters
	char letter() { return _letter; };
	unsigned char minimum_occurences() { return _minimum_occurences; };
	unsigned char maximum_occurences() { return _maximum_occurences; };

	// setters

	// functions
	void add_occurence();
	void clamp_occurences();
	void spot();
	void finish_pass();
};

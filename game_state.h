#include <vector>
#include <string>

#define LETTER_COUNT 5

typedef std::vector<std::string> string_vector;

class Game_state
{
private:
    /// @brief holds all the possible guess words
    string_vector _words;
    string_vector black_letters;
    string_vector exact_match;

public:
    Game_state(string_vector words);
};

Game_state::Game_state(string_vector words)
{
    _words = words;
    exact_match = string_vector(LETTER_COUNT, ".");
}

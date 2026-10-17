#include "letter.h"

Letter::Letter()
{
}

Letter::Letter(char init_letter)
{
	_letter = init_letter;
}

bool Letter::spot_yellow(unsigned char index)
{
	if (_spotted_black_this_pass)
	{
		return IMPOSSIBLE_PATTERN;
	}
	
	_pass_spot_counter++;
	_has_been_spotted = true;
	if (_pass_spot_counter > _minimum_occurences && _minimum_occurences < _maximum_occurences)
	{
		_minimum_occurences++;
	}
	_forbidden_spots |= INDEX_TO_BITFLAG(index);
	return !IMPOSSIBLE_PATTERN;
}

void Letter::spot_black(unsigned char index)
{
	_spotted_black_this_pass = true;
	_has_been_spotted = true;
	_maximum_occurences = _minimum_occurences;
	_forbidden_spots |= INDEX_TO_BITFLAG(index);
}

void Letter::spot_green(unsigned char index)
{
	_pass_spot_counter++;
	_has_been_spotted = true;
	
	if (_pass_spot_counter > _minimum_occurences)
	{
		if (_maximum_occurences == _minimum_occurences)
		{
			_maximum_occurences++;
		}
		_minimum_occurences++;
	}
	
}

void Letter::finish_pass()
{
	_pass_spot_counter = 0;
	_spotted_black_this_pass = false;
}


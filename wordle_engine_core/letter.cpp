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
	if (_spot_flags & spotting_flags::SPOTTED_BLACK_THIS_PASS)
	{
		return IMPOSSIBLE_PATTERN;
	}
	
	_pass_spot_counter++;
	_spot_flags |= spotting_flags::HAS_BEEN_SPOTTED;
	if (_pass_spot_counter > _minimum_occurences && _minimum_occurences < _maximum_occurences)
	{
		_minimum_occurences++;
	}
	_forbidden_spots |= INDEX_TO_BITFLAG(index);
	return !IMPOSSIBLE_PATTERN;
}

void Letter::spot_black(unsigned char index)
{
	_spot_flags |= spotting_flags::HAS_BEEN_SPOTTED | spotting_flags::SPOTTED_BLACK_THIS_PASS;
	_maximum_occurences = _minimum_occurences;
	_forbidden_spots |= INDEX_TO_BITFLAG(index);
}

void Letter::spot_green(unsigned char index)
{
	_pass_spot_counter++;
	_spot_flags |= spotting_flags::HAS_BEEN_SPOTTED;
	
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
	_spot_flags &= ~spotting_flags::SPOTTED_BLACK_THIS_PASS;
}


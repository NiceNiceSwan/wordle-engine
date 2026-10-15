#include "letter.h"

Letter::Letter()
{
}

Letter::Letter(char init_letter)
{
	_letter = init_letter;
	_forbidden_spots.resize(5, false);
	_green_spots.resize(5, false);
}

void Letter::add_occurence()
{
	if (_minimum_occurences < _maximum_occurences)
	{
		_minimum_occurences++;
	}
}

void Letter::clamp_occurences()
{
	_has_been_spotted = true;
	_maximum_occurences = _minimum_occurences;
}

void Letter::spot()
{
	pass_spot_counter++;
	_has_been_spotted = true;
	if (pass_spot_counter > _minimum_occurences)
	{
		add_occurence();
	}
}

void Letter::spot_yellow(unsigned char index)
{
	spot();
	_forbidden_spots[index] = true;
}


void Letter::finish_pass()
{
	pass_spot_counter = 0;
}


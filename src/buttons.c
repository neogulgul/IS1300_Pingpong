#include "buttons.h"

bool l_button_pressed = false;
bool r_button_pressed = false;

void buttons_reset()
{
	l_button_pressed = false;
	r_button_pressed = false;
}
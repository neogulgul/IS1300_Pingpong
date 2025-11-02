#include "gpio.h"

#include "buttons.h"
#include "pingpong.h"

void sleep(uint32_t ms)
{
	HAL_Delay(ms);
}

void led_set_state(uint8_t led_index, bool state)
{
	bool valid_index = led_index >= 0 && led_index <= 7;
	if (!valid_index) return;

	switch (led_index)
	{
		case 0: { HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, state); break; }
		case 1: { HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, state); break; }
		case 2: { HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, state); break; }
		case 3: { HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, state); break; }
		case 4: { HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, state); break; }
		case 5: { HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, state); break; }
		case 6: { HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, state); break; }
		case 7: { HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, state); break; }
	}
}

void Press_update(Press *this, uint8_t pressed_this_update)
{
	this->pressed_last_update = this->pressed_this_update;
	this->pressed_this_update = pressed_this_update;

	this->just_pressed  =  this->pressed_this_update && !this->pressed_last_update;
	this->just_released = !this->pressed_this_update &&  this->pressed_last_update;
}

void Pingpong_init(Pingpong *this)
{
	this->state = STATE_START;
	this->next_state = this->state;
	this->l_score = 0;
	this->r_score = 0;
}

bool interrupt_occurred_whilst_sleeping(bool *occurred, uint32_t ms)
{
	*occurred = false;
	HAL_Delay(ms);
	return *occurred;
}

void Pingpong_update(Pingpong *this)
{
	this->state = this->next_state;

	bool l_pressed_this_update = HAL_GPIO_ReadPin(L_button_GPIO_Port, L_button_Pin) == 0;
	bool r_pressed_this_update = HAL_GPIO_ReadPin(R_button_GPIO_Port, R_button_Pin) == 0;
	Press_update(&this->l_press, l_pressed_this_update);
	Press_update(&this->r_press, r_pressed_this_update);

	static uint8_t led = 0;

	if (this->l_press.just_pressed) led = (led == 0) ? 7 : led - 1;
	if (this->r_press.just_pressed) led = (led == 7) ? 0 : led + 1;

	// if (interrupt_occurred_whilst_sleeping(&l_button_pressed, 100))
	// {
	// 	led = (led == 7) ? 0 : led + 1;
	// }

	for (uint8_t i = 0; i < 8; i++) led_set_state(i, i == led);

	// switch (this->state)
	// {
	// 	case STATE_START     : { Pingpong_handle_start     (this); break; }
	// 	case STATE_MOVE_LEFT : { Pingpong_handle_move_left (this); break; }
	// 	case STATE_MOVE_RIGHT: { Pingpong_handle_move_right(this); break; }
	// 	case STATE_SHOW_SCORE: { Pingpong_handle_show_score(this); break; }
	// }
}

void Pingpong_handle_start(Pingpong *this)
{
}

void Pingpong_handle_move_left(Pingpong *this)
{
}

void Pingpong_handle_move_right(Pingpong *this)
{
}

void Pingpong_handle_show_score(Pingpong *this)
{
}
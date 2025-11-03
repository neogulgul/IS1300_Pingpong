#include <stdio.h>

#include "gpio.h"

#include "buttons.h"
#include "pingpong.h"

#define BALL_DELAY_MS_BASE      200
#define BALL_DELAY_MS_DECREMENT  20
#define SCORE_DISPLAY_PAUSE_MS 3000

bool interrupt_occurred_whilst_waiting(bool *interrupt, uint32_t ms, bool break_on_interrupt)
{
	uint32_t start = HAL_GetTick();
	*interrupt = false;
	for (;;)
	{
		uint32_t elapsed = HAL_GetTick() - start;
		bool timed_out = elapsed >= ms;
		bool interrupted = *interrupt && break_on_interrupt;
		if (timed_out || interrupted) break;
	}
	return *interrupt;
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

void led_set_state_all(bool state)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		led_set_state(i, state);
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
	this->state      = STATE_UNDEFINED;
	this->next_state = STATE_START;
}

void Pingpong_reset(Pingpong *this)
{
	this->l_score = 0;
	this->r_score = 0;
	Pingpong_ball_speed_reset(this);
}

void Pingpong_ball_speed_reset(Pingpong *this)
{
	this->ball_delay_ms = BALL_DELAY_MS_BASE;
}

void Pingpong_ball_speed_increase(Pingpong *this)
{
	if (this->ball_delay_ms > BALL_DELAY_MS_DECREMENT)
	{
		this->ball_delay_ms -= BALL_DELAY_MS_DECREMENT;
	}
}

void Pingpong_update(Pingpong *this)
{
	bool l_pressed_this_update = HAL_GPIO_ReadPin(L_button_GPIO_Port, L_button_Pin) == 0;
	bool r_pressed_this_update = HAL_GPIO_ReadPin(R_button_GPIO_Port, R_button_Pin) == 0;
	Press_update(&this->l_press, l_pressed_this_update);
	Press_update(&this->r_press, r_pressed_this_update);

	State old_state = this->state;
	this->state = this->next_state;

	bool state_changed = this->state != old_state;

	if (state_changed)
	{
		switch (this->state)
		{
			case STATE_START:
			{
				Pingpong_reset(this);
				printf("Start by pressing on of the buttons.\n");
				break;
			}
			case STATE_MOVE_LEFT:
			{
				printf("Ball now moves left.\n");
				break;
			}
			case STATE_MOVE_RIGHT:
			{
				printf("Ball now moves right.\n");
				break;
			}
			case STATE_SCORE_DISPLAY:
			{
				printf("(%i:%i) Left player has %i points. Right player has %i points.\n", this->l_score, this->r_score, this->l_score, this->r_score);
				break;
			}
		}
	}

	switch (this->state)
	{
		case STATE_START        : { Pingpong_handle_start        (this); break; }
		case STATE_MOVE_LEFT    : { Pingpong_handle_move_left    (this); break; }
		case STATE_MOVE_RIGHT   : { Pingpong_handle_move_right   (this); break; }
		case STATE_BLINKING     : { Pingpong_handle_blinking     (this); break; }
		case STATE_SCORE_DISPLAY: { Pingpong_handle_score_display(this); break; }
	}
}

void Pingpong_serve_from_the_left(Pingpong *this)
{
	this->next_state         = STATE_MOVE_RIGHT;
	this->ball_position      = 0;
	this->left_player_served = true;
}

void Pingpong_serve_from_the_right(Pingpong *this)
{
	this->next_state         = STATE_MOVE_LEFT;
	this->ball_position      = 7;
	this->left_player_served = false;
}

void Pingpong_handle_start(Pingpong *this)
{
	bool both_pressed = this->l_press.just_pressed && this->r_press.just_pressed;
	if (both_pressed) return;

	if (this->l_press.just_pressed)
	{
		Pingpong_serve_from_the_left(this);
	}
	else if (this->r_press.just_pressed)
	{
		Pingpong_serve_from_the_right(this);
	}

	led_set_state_all(0);
}

void Pingpong_scored(Pingpong *this, uint8_t *score)
{
	(*score)++;
	this->next_state = STATE_BLINKING;
}

void Pingpong_handle_move_left(Pingpong *this)
{
	Pingpong_draw_ball(this);
	bool at_edge = this->ball_position == 0;
	if (interrupt_occurred_whilst_waiting(&l_button_pressed, this->ball_delay_ms, true))
	{
		if (at_edge)
		{
			this->next_state = STATE_MOVE_RIGHT;
			this->ball_position = 1;
			Pingpong_ball_speed_increase(this);
			return;
		}
		else
		{
			Pingpong_scored(this, &this->r_score);
		}
	}
	else
	{
		if (at_edge)
		{
			Pingpong_scored(this, &this->r_score);
		}
	}
	this->ball_position--;
}

void Pingpong_handle_move_right(Pingpong *this)
{
	Pingpong_draw_ball(this);
	bool at_edge = this->ball_position == 7;
	if (interrupt_occurred_whilst_waiting(&r_button_pressed, this->ball_delay_ms, true))
	{
		if (at_edge)
		{
			this->next_state = STATE_MOVE_LEFT;
			this->ball_position = 6;
			Pingpong_ball_speed_increase(this);
			return;
		}
		else
		{
			Pingpong_scored(this, &this->l_score);
		}
	}
	else
	{
		if (at_edge)
		{
			Pingpong_scored(this, &this->l_score);
		}
	}
	this->ball_position++;
}

void Pingpong_handle_blinking(Pingpong *this)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		led_set_state_all(1);
		HAL_Delay(50);
		led_set_state_all(0);
		HAL_Delay(50);
	}
	this->next_state = STATE_SCORE_DISPLAY;
}

void Pingpong_handle_score_display(Pingpong *this)
{
	Pingpong_ball_speed_reset(this);

	switch (this->l_score)
	{
		case 0: { led_set_state(0, 0); led_set_state(1, 0); led_set_state(2, 0); led_set_state(3, 0); break; }
		case 1: { led_set_state(0, 1); led_set_state(1, 0); led_set_state(2, 0); led_set_state(3, 0); break; }
		case 2: { led_set_state(0, 1); led_set_state(1, 1); led_set_state(2, 0); led_set_state(3, 0); break; }
		case 3: { led_set_state(0, 1); led_set_state(1, 1); led_set_state(2, 1); led_set_state(3, 0); break; }
		case 4: { led_set_state(0, 1); led_set_state(1, 1); led_set_state(2, 1); led_set_state(3, 1); break; }
	}
	switch (this->r_score)
	{
		case 0: { led_set_state(7, 0); led_set_state(6, 0); led_set_state(5, 0); led_set_state(4, 0); break; }
		case 1: { led_set_state(7, 1); led_set_state(6, 0); led_set_state(5, 0); led_set_state(4, 0); break; }
		case 2: { led_set_state(7, 1); led_set_state(6, 1); led_set_state(5, 0); led_set_state(4, 0); break; }
		case 3: { led_set_state(7, 1); led_set_state(6, 1); led_set_state(5, 1); led_set_state(4, 0); break; }
		case 4: { led_set_state(7, 1); led_set_state(6, 1); led_set_state(5, 1); led_set_state(4, 1); break; }
	}

	HAL_Delay(SCORE_DISPLAY_PAUSE_MS);

	if (Pingpong_has_a_winner(this))
	{
		this->next_state = STATE_START;
		return;
	}

	if (this->left_player_served)
	{
		Pingpong_serve_from_the_right(this);
	}
	else
	{
		Pingpong_serve_from_the_left(this);
	}
}

void Pingpong_draw_ball(Pingpong *this)
{
	led_set_state_all(0);
	led_set_state(this->ball_position, 1);
}

bool Pingpong_has_a_winner(Pingpong *this)
{
	return this->l_score == 4 || this->r_score == 4;
}
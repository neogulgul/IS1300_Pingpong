#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum State
{
	STATE_START,
	STATE_MOVE_LEFT,
	STATE_MOVE_RIGHT,
	STATE_SHOW_SCORE
} State;

typedef struct Press
{
	uint8_t pressed_this_update;
	uint8_t pressed_last_update;
	uint8_t just_pressed;
	uint8_t just_released;
} Press;

typedef struct Pingpong
{
	State state, next_state;
	Press l_press, r_press;
	uint8_t l_score, r_score;
} Pingpong;

void sleep(uint32_t ms);

void led_set_state(uint8_t led_index, bool state);

void Press_update(Press *this, uint8_t pressed_this_update);

void Pingpong_init             (Pingpong *this);
void Pingpong_update           (Pingpong *this);
void Pingpong_handle_start     (Pingpong *this);
void Pingpong_handle_move_left (Pingpong *this);
void Pingpong_handle_move_right(Pingpong *this);
void Pingpong_handle_show_score(Pingpong *this);
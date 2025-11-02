#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum State
{
	STATE_UNDEFINED,
	STATE_START,
	STATE_MOVE_LEFT,
	STATE_MOVE_RIGHT,
	STATE_SCORE_DISPLAY
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
	bool left_player_served;
	uint8_t ball_position;
	uint16_t ball_delay_ms;
} Pingpong;

void sleep(uint32_t ms);

void led_set_state(uint8_t led_index, bool state);
void led_set_state_all(bool state);

void Press_update(Press *this, uint8_t pressed_this_update);

void Pingpong_init                (Pingpong *this);
void Pingpong_reset               (Pingpong *this);
void Pingpong_ball_speed_reset    (Pingpong *this);
void Pingpong_ball_speed_increase (Pingpong *this);
void Pingpong_update              (Pingpong *this);
void Pingpong_serve_from_the_left (Pingpong *this);
void Pingpong_serve_from_the_right(Pingpong *this);
void Pingpong_handle_start        (Pingpong *this);
void Pingpong_handle_move_left    (Pingpong *this);
void Pingpong_handle_move_right   (Pingpong *this);
void Pingpong_handle_score_display(Pingpong *this);
void Pingpong_draw_ball           (Pingpong *this);
bool Pingpong_has_a_winner        (Pingpong *this);
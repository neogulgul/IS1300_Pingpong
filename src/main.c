#include "buttons.h"
#include "pingpong.h"

#include <stdio.h>
#include "main.h"
#include "usart.h"
#include "gpio.h"

void startup();

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	printf("Interrupt caused by GPIO pin 0x%x.\n", GPIO_Pin);

	if (GPIO_Pin == L_button_Pin)
	{
		l_button_pressed = true;
		printf("Left button pressed!\n");
	}
	if (GPIO_Pin == R_button_Pin)
	{
		r_button_pressed = true;
		printf("Right button pressed!\n");
	}
}

int main()
{
	startup();

	Pingpong pingpong;

	Pingpong_init(&pingpong);

	for (;;)
	{
		buttons_reset();
		Pingpong_update(&pingpong);
	}
}
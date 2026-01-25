#include "buzzer.h"
#include "tim.h"

void buzzer_init(){
	__HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 0);
	HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1);
}

void buzzer_SetVolume(uint8_t duty_cycle){
	if (duty_cycle > 99) duty_cycle = 99;
	__HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, duty_cycle);
}

void buzzer_Stop(){
	HAL_TIM_PWM_Stop(&htim13, TIM_CHANNEL_1);
}

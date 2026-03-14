/*
 * software_timer.c
 *
 *  Created on: Nov 4, 2025
 *      Author: Dell
 */

#include "software_timer.h"

#define MAX_TIMER 6

uint16_t timer_counter[MAX_TIMER];
uint8_t  timer_flag[MAX_TIMER];

void timer_init() {
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim13);
}

void setTimer(uint8_t index, uint16_t duration) {
	timer_counter[index] = duration;
	timer_flag[index] = 0;
}

uint8_t isTimerExpired(uint8_t index) {
	if (timer_flag[index] == 1) {
		return 1;
	}
	return 0;
}

void timerRun() {
	for (uint8_t i = 0; i < MAX_TIMER; i++) {
		if (timer_counter[i] > 0) {
			timer_counter[i]--;
			if (timer_counter[i] <= 0) {
				timer_flag[i] = 1;
			}
		}
	}
}

//void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim) {
//	timerRun();
//}

/*
 * software_timer.h
 *
 *  Created on: Nov 4, 2025
 *      Author: Dell
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "main.h"
#include "tim.h"
#include "gpio.h"

void timer_init();
void setTimer(uint8_t index, uint16_t duration);
uint8_t isTimerExpired(uint8_t index);
void timerRun();
//void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim);


#endif /* INC_SOFTWARE_TIMER_H_ */

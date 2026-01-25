#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

void buzzer_init();
void buzzer_SetVolume(uint8_t duty_cycle);
void buzzer_Stop();

#endif /* INC_BUZZER_H_ */

/*
 * ledRGB.h
 * header P4, Pin SIG = PA0
 *  Created on: Dec 11, 2024
 *      Author: TRONG DAT
 */

#ifndef INC_LEDRGB_H_
#define INC_LEDRGB_H_

#include "stm32f1xx_hal.h"
#include "ledRGB.h"
#include <string.h>

/* 	TIM2, CHANNEL1
 * 	F = 8MHz, F_rgb = 8KHz
 * 	PreScaler = 1-1, CounterPeriod = 10-1 ???
 * 	DMA MemtoPre, auto increase address Mem
 * 	Data size Mem = Byte, Pre = HalfWord
 */

extern TIM_HandleTypeDef htim2;

#define RGB_NUM_LED 4
#define RGB_TIM htim2
#define RGB_TIM_CHANNEL TIM_CHANNEL_1

#define RGB_DUTYCYCLE_LO 3 // 375 ns
#define RGB_DUTYCYCLE_HI 6 // 750 ns
#define RGB_RSTCODE 48 // 60 ms

#define RGB_BITS_LENGTH 24 // G7 .. G0 R7 ... R0 B7 ... B0

#define RGB_DMA_BUFF_LENGTH ((RGB_NUM_LED * RGB_BITS_LENGTH) + RGB_RSTCODE)

typedef union{
	struct{
		uint8_t green;
		uint8_t red;
		uint8_t blue;
		uint8_t unused;
	}color;
	uint32_t ledData;
}LED_RGB_DATA;

LED_RGB_DATA ledRGBData[RGB_NUM_LED];
uint8_t DMA_Buffer[RGB_DMA_BUFF_LENGTH];
volatile uint8_t flag_RGB_busy;

void RGB_Init();
void RGB_SetColor(uint8_t rgb_index, uint8_t, uint8_t, uint8_t);
void RGB_TurnOff();
void RGB_Auto(int temperature, int humidity);
void RGB_Update();
void RGB_Callback();
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
#endif /* INC_LEDRGB_H_ */

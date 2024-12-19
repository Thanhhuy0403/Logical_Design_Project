/*
 * ledRGB.c
 *
 *  Created on: Dec 11, 2024
 *      Author: TRONG DAT
 */
#include "ledRGB.h"

void RGB_Init(){
	HAL_TIM_PWM_Init(&RGB_TIM);

	memset(DMA_Buffer, 0, RGB_DMA_BUFF_LENGTH);

	RGB_SetColor(0, 255, 0, 0);
	RGB_SetColor(1, 0, 255, 255);
	RGB_SetColor(2, 0, 255, 0);
	RGB_SetColor(3, 255, 0, 255);

	RGB_Update();

	flag_RGB_busy = 0;
}

void RGB_SetColor(uint8_t rgb_index, uint8_t red, uint8_t green, uint8_t blue){
	if(red > 255) red = 255;
	if(green > 255) green = 255;
	if(blue > 255) blue = 255;

	ledRGBData[rgb_index].color.red = red;
	ledRGBData[rgb_index].color.green = green;
	ledRGBData[rgb_index].color.blue = blue;
}

void RGB_TurnOff(){
	memset(ledRGBData, 0, sizeof(ledRGBData));
	RGB_Update();
}

int auto_map(int x, int in_min, int in_max, int out_min, int out_max){
    if (out_min < out_max){
        return(x - in_min) * (out_max - out_min) / (in_max - in_min);
    }else{
        return(in_max - x) * (out_min - out_max) / (in_max - in_min);
    }
}

void RGB_Auto(int temperature, int humidity){
	// humid
	RGB_SetColor(0, auto_map(humidity, 0, 100, 100, 0), auto_map(humidity, 0, 100, 0, 255), 0);
	// temperature
	if(temperature < 20){
		for(uint8_t i = 1; i < RGB_NUM_LED; i++){
			RGB_SetColor(i, 0, 255, 255);
		}
	}
	else if(temperature < 40){
		for(uint8_t i = 1; i < RGB_NUM_LED; i++){
			RGB_SetColor(i, auto_map(temperature, 20, 40, 0, 150), 0, auto_map(temperature, 20, 40, 255, 0));
		}
	}
	else{
		for(uint8_t i = 1; i < RGB_NUM_LED; i++){
			RGB_SetColor(i, 255, 0, 255);
		}
	}
	RGB_Update();
}

void RGB_Update(){
	if(flag_RGB_busy) return;

	memset(DMA_Buffer, 0, RGB_DMA_BUFF_LENGTH);
	uint8_t bufferIndex = 0;
	for(uint8_t ledIndex = 0; ledIndex < RGB_NUM_LED; ledIndex++){
		for(uint8_t bitIndex = 0; bitIndex < RGB_BITS_LENGTH; bitIndex++){
			if((ledRGBData[ledIndex].ledData >> bitIndex) & 0x01){
				DMA_Buffer[bufferIndex] = RGB_DUTYCYCLE_HI;
			}
			else{
				DMA_Buffer[bufferIndex] = RGB_DUTYCYCLE_LO;
			}

			bufferIndex++;
		}
	}

	HAL_StatusTypeDef HAL_PWM = HAL_TIM_PWM_Start_DMA(&RGB_TIM, RGB_TIM_CHANNEL, (uint32_t*)DMA_Buffer, RGB_DMA_BUFF_LENGTH);
	if(HAL_PWM == HAL_OK) flag_RGB_busy = 1;
}

void RGB_Callback(){
//	HAL_TIM_PWM_Stop_DMA(&RGB_TIM, RGB_TIM_CHANNEL);
//	flag_RGB_busy = 0;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	HAL_TIM_PWM_Stop_DMA(&RGB_TIM, RGB_TIM_CHANNEL);
	flag_RGB_busy = 0;
}

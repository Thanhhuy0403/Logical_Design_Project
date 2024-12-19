/*
 * UART.h
 *
 *  Created on: Dec 12, 2024
 *      Author: TRONG DAT
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32f1xx_hal.h"
#include "ledRGB.h"

#define UART_BUFF_SIZE 30

extern UART_HandleTypeDef huart1;

extern float RH;
extern float Temp;

uint8_t red, green, blue;// put them here cause this is the read buffer for color from UART
extern LED_RGB_DATA ledRGBData[]; // Data for RGB
const char ACK_token[4];

char UARTBuffer[UART_BUFF_SIZE];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_UART_H_ */

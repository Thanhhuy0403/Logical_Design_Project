/*
 * UART.c
 *
 *  Created on: Dec 12, 2024
 *      Author: TRONG DAT
 */

#include "UART.h"
#include <stdio.h>

const char ACK_token[] = "ACK.";

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if(huart->Instance == USART1){
    	// SEND ACK
    	HAL_UART_Transmit(&huart1, (uint8_t*)ACK_token, sizeof(ACK_token), 1000);
    	// READ COMMAND
        if(strncmp(UARTBuffer, "RGB: COLOR:", 11)){
            if(sscanf(UARTBuffer, "RGB: COLOR: %hhu, %hhu, %hhu.", &red, &green, &blue) == 3){// SUCCESS
            	for(uint8_t i = 0; i < RGB_NUM_LED; i++){
            		RGB_SetColor(i, red, green, blue);
            	}
            }else{// FAILED to read all 3 value
            	for(uint8_t i = 0; i < RGB_NUM_LED; i++){
            		RGB_SetColor(i, 0, 0, 255);
            	}
            }
        }
        else if(strncmp(UARTBuffer, "RGB: OFF", 8)){
        	RGB_TurnOff();
//        	RGB_Auto((uint8_t)Temp, (uint8_t)RH);
        }
        else{// read other cmd

        }
        HAL_UART_Receive_IT(&huart1, (uint8_t*)UARTBuffer, UART_BUFF_SIZE);
    }
}

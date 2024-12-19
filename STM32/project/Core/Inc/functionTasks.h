/*
 * functionTasks.h
 *
 *  Created on: Nov 12, 2024
 *      Author: phan thanh huy
 */

#ifndef INC_FUNCTIONTASKS_H_
#define INC_FUNCTIONTASKS_H_

#include "global.h"
#include "stm32f1xx_hal_uart.h"
#include "DHT20.h"
UART_HandleTypeDef huart1;

extern bool flag_HandleDisplay;
extern uint8_t sendData[20];

void handleDisplay();
void clear_task_0();
void display_data();
void sendDataFromSTMToESP();
void ledBlinky();
void RGBAuto();
#endif /* INC_FUNCTIONTASKS_H_ */

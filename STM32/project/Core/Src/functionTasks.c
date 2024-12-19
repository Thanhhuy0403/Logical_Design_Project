/*
 * functionTasks.c
 *
 *  Created on: Nov 12, 2024
 *      Author: phan thanh huy
 */

#include "functionTasks.h"
#include "string.h"

extern float RH;
extern float Temp;

bool flag_HandleDisplay = false;
uint8_t sendData[20] = "12.13-23.67";

/* Hiển thị dòng chữ đầu tiên của chương trình 1 lần duy nhất
 * */
void handleDisplay()
{
	char str[] = "DO AN";
	for (int i = 11; i >= 0; i--) {
		lcd_goto_XY(0, i);
		lcd_send_string(str);
		HAL_Delay(400);
		lcd_clear_display();
		HAL_Delay(50);
	}
	flag_HandleDisplay = true;
}

/* Hiển thị dữ liệu đọc từ cảm biến DHT20 lên màn hình LCD bằng I2C
 * Cứ mỗi 5s sẽ cập nhật dữ liệu gửi 1 lần
 * */
void display_data()
{
	trigger_measurement_data_dht20();
	char buffer[20];
//	char bufferTemp[100];
	sprintf(buffer, "TEMP:  %.2f C", Temp);
//	sprintf(bufferTemp, "%.2f", Temp);


	lcd_goto_XY(0, 0);
	lcd_send_string(buffer);

//	char bufferHumi[20];
	sprintf(buffer, "HUMI:  %.2f %%", RH);
//	sprintf(bufferHumi, "%.2f", RH);

//	strcat(bufferTemp, "-");
//	strcat(bufferTemp, bufferHumi);
//	bufferTemp[strlen(bufferTemp)] = '\0';
	lcd_goto_XY(1, 0);
	lcd_send_string(buffer);

//	int sz = strlen(bufferTemp);
//	for(int i = 0; i < sz; i++){
//		sendData[i] = (uint8_t)bufferTemp[i];
//	}
//	sendData[sz] = '\0';
}

/* Xử lý dữ liêụ và truyền cho esp32
 *
 * */
void sendDataFromSTMToESP(){
	char bufferTemp[100];
	sprintf(bufferTemp, "%.2f", Temp);
	char bufferHumi[20];
	sprintf(bufferHumi, "%.2f", RH);
	strcat(bufferTemp, "-");
	strcat(bufferTemp, bufferHumi);
	bufferTemp[strlen(bufferTemp)] = '\0';
	int sz = strlen(bufferTemp);
	for(int i = 0; i < sz; i++){
		sendData[i] = (uint8_t)bufferTemp[i];
	}
	sendData[sz] = '\0';
	HAL_UART_Transmit(&huart1, sendData, sizeof(sendData), 1000);
}

void ledBlinky(){
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void RGBAuto(){
	RGB_Auto(Temp, RH);
}

/*
 * DHT20.h
 *
 *  Created on: Nov 12, 2024
 *      Author: phan thanh huy
 */

#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include "global.h"

extern float RH;
extern float Temp;

typedef enum
{
  DHT20_OK       			= 0x00U,
  DHT20_ERROR_INIT    = 0x01U,
  DHT20_ERROR_CRC     = 0x02U,
  DHT20_BUSY				  = 0x03U
} Status_Trigger_DHT20;

unsigned char  CheckCrc8(unsigned char *pDat,unsigned char Lenth);
uint8_t read_register(uint8_t register_pointer);
Status_Trigger_DHT20 trigger_measurement_data_dht20();

#endif /* INC_DHT20_H_ */

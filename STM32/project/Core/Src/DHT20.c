/*
 * DHT20.c
 *
 *  Created on: Nov 12, 2024
 *      Author: phan thanh huy
 */

#include "DHT20.h"

extern I2C_HandleTypeDef hi2c1;
#define ADDRESS_SLAVE_DHT20 0X38

float RH = 0;
float Temp = 0;

unsigned char  CheckCrc8(unsigned char *pDat,unsigned char Lenth)
{
	unsigned char crc = 0xff, i, j;

	for (i = 0; i < Lenth ; i++)
	{
			crc = crc ^ *pDat;
			for (j = 0; j < 8; j++)
			{
					if (crc & 0x80) crc = (crc << 1) ^ 0x31;
					else crc <<= 1;
			}
			pDat++;
	}
	return crc;
}

uint8_t read_register(uint8_t register_pointer)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t return_value = 0;

	status = HAL_I2C_Mem_Read(&hi2c1, ADDRESS_SLAVE_DHT20 << 1, (uint16_t)register_pointer, I2C_MEMADD_SIZE_8BIT, &return_value, 1, 100);

	/* Check the communication status */
	if(status != HAL_OK)
	{

	}
	printf("return_value for %X : %X \n",register_pointer, return_value);
	return return_value;
}

//typedef enum
//{
//  DHT20_OK       			= 0x00U,
//  DHT20_ERROR_INIT    = 0x01U,
//  DHT20_ERROR_CRC     = 0x02U,
//  DHT20_BUSY				  = 0x03U
//} Status_Trigger_DHT20;

//Trigger measurement data
Status_Trigger_DHT20 trigger_measurement_data_dht20()
{
	Status_Trigger_DHT20 status = DHT20_OK;

	HAL_Delay(100);
	uint8_t status_init = read_register(0x71);
	if ((status_init & 0x18) == 0x18) // et a byte of status word by sending 0x71. If the status word and 0x18 are not equal to 0x18
	{
		HAL_Delay(10);
		uint8_t data_t[3];
		data_t[0] = 0xAC; //address
		data_t[1] = 0x33; //data1
		data_t[2] = 0x00; //data2
		HAL_I2C_Master_Transmit (&hi2c1, ADDRESS_SLAVE_DHT20 << 1,(uint8_t *) data_t, 3, 100);
		HAL_Delay(80);

		uint8_t buffer[7];
		uint32_t data_read = 0;

    // receive the 2 x 8bit data into the receive buffer
    HAL_I2C_Master_Receive(&hi2c1, ADDRESS_SLAVE_DHT20 << 1, buffer, 7, 100);

		for (int i =0; i<7;i++)
		{
			printf("buffer %d: %X \n",i, buffer[i]);
		}

		// Check Bit[7] at Byte0
		if ((buffer[0] & 0x80) == 0x00)
		{
			// Check CRC
			if(CheckCrc8(&buffer[0],6)==buffer[6])
			{
				printf("CRC OK \n");
				data_read=buffer[1]; 		// 0x90
				data_read=data_read<<8;		// 0x9000
				data_read+=buffer[2]; 		// 0x9000 + 0x28 => 0x9028
				data_read=data_read<<8; 	// 0x902800
				data_read+=buffer[3]; 		// 0x902800 + 0x96 => 0x902896
				data_read=data_read>>4;		// 0x902896 => 0x90289

				RH = (float)data_read*100/1048576;
				printf("RH %0.1f%% \n", RH);

				data_read=buffer[3]&0x0F;
				data_read=data_read<<8;
				data_read+=buffer[4];
				data_read=data_read<<8;
				data_read+=buffer[5];

				Temp = (float)data_read*200/1048576-50;
				printf("Temp %0.1f \n", Temp);
			}
			else
			{
				printf("CRC fail \n");
				status = DHT20_ERROR_CRC;
			}
		}
		else
		{
			printf("Check Bit[7] at Byte0: %x \n",buffer[0]&0x80);
			status = DHT20_BUSY;
		}

	}
	else //  initialize the 0x1B, 0x1C, 0x1E registers
	{
		status = DHT20_ERROR_INIT;
	}
	return status;
}

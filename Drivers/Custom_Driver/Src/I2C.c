/*
 * I2C.c
 *
 *  Created on: Feb 23, 2024
 *      Author: ASUS
 */

#include"main.h"

#define GPIOB_Base_Add	0x40020400
#define I2C1_Base_Add	0x40005400

void I2C_Init()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();

	uint32_t* MODE = (uint32_t*)(GPIOB_Base_Add + 0x00);
	*MODE |= (0b10 << 12) | (0b10 << 18);

	uint32_t* AFRL = (uint32_t*)(GPIOB_Base_Add + 0x20);
	*AFRL &= ~(1 << 24);
	*AFRL |= (0b100 << 24);
	uint32_t* AFRH = (uint32_t*)(GPIOB_Base_Add + 0x24);
	*AFRH &= ~(1 << 4);
	*AFRH |= (0b100 << 4);

	uint32_t* CR1 = (uint32_t*)(I2C1_Base_Add + 0x00);
	uint32_t* CR2 = (uint32_t*)(I2C1_Base_Add + 0x04);
	uint32_t* CCR = (uint32_t*)(I2C1_Base_Add + 0x1C);
	*CR1 &= ~(1 << 0);
	*CR2 |= 16;
	*CCR = 80;
	*CR1 |= (1 << 0);
}

uint8_t I2C_Read(uint8_t sensor_addr, uint8_t sensor_reg)
{
	uint32_t* CR1 = (uint32_t*)(I2C1_Base_Add + 0x00);
	uint32_t* SR1 = (uint32_t*)(I2C1_Base_Add + 0x14);
	uint32_t* SR2 = (uint32_t*)(I2C1_Base_Add + 0x18);
	uint32_t* DR = (uint32_t*)(I2C1_Base_Add + 0x10);
	const uint8_t addr_sensor = sensor_addr << 1;
	while(((*SR2 >> 1)&1) == 1);						//check no communicate on bus
	//Start bit
	*CR1 |= (1 << 8);
	while(((*SR1 >> 0)&1) != 1);						//check Start bit state
	*DR = addr_sensor | 0;								//Send 7bit data + 1bit (R/W)
	while(((*SR1 >> 1)&1) != 1);						//Check Address Sent
	uint32_t temp = *SR2;
	(void)temp;
	while(((*SR1 >> 10)&1) == 1);						//check ACK
	//command
	*DR = sensor_reg;
	while(((*SR1 >> 2)&1) != 1);						//check Byte transfer finished
	while(((*SR1 >> 10)&1) == 1);						//check ACK

	*CR1 |= (1 << 8);									//restart
	while(((*SR1 >> 0)&1) != 1);						//check
	*DR = addr_sensor | 1;
	while(((*SR1 >> 1)&1) != 1);
	temp = *SR2;
	while(((*SR1 >> 6)&1) != 1);

	uint8_t data = *DR;
	*CR1 |= (1 << 9);
	return data;
}

void I2C_Write(uint8_t sensor_add, uint8_t sensor_reg, uint8_t value)
{
	uint32_t* CR1 = (uint32_t*)(I2C1_Base_Add + 0x00);
	uint32_t* SR1 = (uint32_t*)(I2C1_Base_Add + 0x14);
	uint32_t* SR2 = (uint32_t*)(I2C1_Base_Add + 0x18);
	uint32_t* DR = (uint32_t*)(I2C1_Base_Add + 0x10);

	const uint8_t addr_sensor = sensor_add << 1;
	while(((*SR2 >> 1)&1) == 1);

	*CR1 |= (1 << 8);
	while(((*SR1 >> 0)&1) != 1);
	*DR = addr_sensor | 0;
	while(((*SR1 >> 1)&1) != 1);
	uint8_t temp = *SR2;
	(void)temp;
	while(((*SR1 >> 10)&1) == 1);
	*DR = sensor_reg;
	while(((*SR1 >> 2)&1) != 1);
	while(((*SR1 >> 10)&1) == 1);
	*DR = value;
	while(((*SR1 >> 2)&1) != 1);
	while(((*SR1 >> 10)&1) == 1);
	*CR1 |= (1 << 9);
}

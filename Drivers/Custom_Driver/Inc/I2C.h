/*
 * I2C.h
 *
 *  Created on: Feb 23, 2024
 *      Author: ASUS
 */

#ifndef CUSTOM_DRIVER_INC_I2C_H_
#define CUSTOM_DRIVER_INC_I2C_H_
#include<stdint.h>

void I2C_Init();
uint8_t I2C_Read(uint8_t sensor_addr, uint8_t sensor_reg);
void I2C_Write(uint8_t sensor_addr, uint8_t sensor_reg,uint8_t value);

#endif /* CUSTOM_DRIVER_INC_I2C_H_ */

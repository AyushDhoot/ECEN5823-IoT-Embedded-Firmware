/*
 * i2c.h
 *
 *  Created on: 22-Sep-2018
 *      Author: Ayush
 */

#include "src/main.h"
#include "src/gpio.h"
#include "em_i2c.h"
#include "sleep.h"
#include "letimer.h"

#ifndef I2C_H_
#define I2C_H_

int temp;

#endif /* I2C_H_ */

void i2c_init(void);
void i2c_disable (void);
void i2c_driver (void);
void I2C0_IRQHandler (void);
int temperature;
void read_byte(uint8_t slave_address, uint8_t reg_address, uint8_t* data_sixty_four);
uint8_t read_status(uint8_t slave_address, uint8_t reg_address);
void write_byte(uint8_t slave_address, uint8_t reg_address, uint8_t data_byte);

float mag_sensor(void);

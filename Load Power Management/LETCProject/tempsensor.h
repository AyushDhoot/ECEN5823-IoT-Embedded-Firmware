/*
 * tempsensor.h
 *
 *  Created on: 22-Sep-2018
 *      Author: Ayush
 */

#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_


#define threshold_temp 15 		// Set Threshold Temperature


#endif /* TEMPSENSOR_H_ */

#include "em_i2c.h"
#include "i2cspm.h"
#include "tempsens.h"
#include "src/gpio.h"
#include "i2c.h"


void temp_sensor (void);

/*
 * tempsensor.c
 *
 *  Created on: 22-Sep-2018
 *      Author: Ayush
 */


#include "tempsensor.h"
#include "src/gpio.h"
#include "i2c.h"

float temp_sensor()
{
	tempF = ((175.72 * temperature)/65536)-46.85	;

			if ( tempF <= threshold_temp )
				{
					GPIO_PinOutSet( LED1_port , LED1_pin );
				}

	return tempF;
}


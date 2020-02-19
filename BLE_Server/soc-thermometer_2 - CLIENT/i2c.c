/*
 * i2c.c
 *
 *  Created on: 22-Sep-2018
 *      Author: Ayush
 */

#include "i2c.h"
#include "src/gpio.h"
#include "sleep.h"
#include "src/sleep.h"
#include "timer.h"

void i2c_init(void)
{
		//blockSleepmode(sleepEM2);

		GPIO_PinModeSet(scl_port,scl_pin,gpioModeWiredAnd,1 ); //SCL Line

		GPIO_PinModeSet(sda_port,sda_pin,gpioModeWiredAnd,1 ); //SDA Line

		I2C0->ROUTEPEN |= I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN; //Setting SCL and SDA

		I2C0->ROUTELOC0 |= (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SCLLOC_MASK))| I2C_ROUTELOC0_SCLLOC_LOC14;

		I2C0->ROUTELOC0 |= (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SDALOC_MASK))| I2C_ROUTELOC0_SDALOC_LOC16;

	const I2C_Init_TypeDef i2c_reg = I2C_INIT_DEFAULT;

	I2C_Init(I2C0,&i2c_reg);
	I2C_Enable(I2C0,true);
	GPIO_PinOutSet(sensor_port,sensor_pin);

/*	int i=0;
	while (i<400000)
	{
		i++;
	}*/
	Timer0_IRQHandler();
	//Reset I2C Slave if Required
	for (int i=0; i<9 ; i++)
	{
		GPIO_PinOutClear(scl_port,scl_pin);
		GPIO_PinOutSet(sda_port,sda_pin);
	}

		if (I2C0->STATE & I2C_STATE_BUSY)
			{
				I2C0->CMD = I2C_CMD_ABORT;
			}

	//Clear all Interrupts
	//I2C0->IFC= 0x00000000;
}


void i2c_disable (void)
{
	//Disable Interrupts
	I2C0->ROUTEPEN &= ~(I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN);
	I2C_Enable(I2C0,false);

	GPIO_PinOutClear(sda_port,sda_pin);
	GPIO_PinOutClear(scl_port,scl_pin);
	GPIO_PinOutClear(sensor_port,sensor_pin);

//	unblockSleepmode(2);
}

void i2c_driver (void)
{
	I2C0->TXDATA = (0x40<<1)|(0x00); 	//Initialize Tx Buffer

	I2C0->CMD= I2C_CMD_START; 	//Send Start

	while ((I2C0->IF & I2C_IF_ACK)==0){};

	I2C0->IFC=I2C_IFC_ACK; //Clear interrupt

	I2C0->TXDATA = 0xE3; //Initialize the TX Buffer

	while ((I2C0->IF & I2C_IF_ACK)==0){};  //Check ack

	I2C0->IFC=I2C_IFC_ACK; //Clear Interrupt

	I2C0->CMD= I2C_CMD_START; 	//Send Start

	I2C0->TXDATA =(0x40<<1)|(0x01);  //TX Buffer initialization Read

	while ((I2C0->IF & I2C_IF_ACK)==0){};  //Check ack

	I2C0->IFC =I2C_IFC_ACK;  //Clear Interrupt

	while ((I2C0->IF & I2C_IF_RXDATAV)==0); //Receive data from sensor

	temperature = I2C0-> RXDATA;	//Store as temperature

	temperature= temperature<<8;	//Only the MSB

	I2C0->CMD = I2C_CMD_ACK;

	while ((I2C0->IF & I2C_IF_RXDATAV)==0);

	temperature |= I2C0-> RXDATA;	//Only the LSB

	I2C0->CMD = I2C_CMD_NACK; //Send NACK

	I2C0->CMD = I2C_CMD_STOP; //Send Stop Command

}



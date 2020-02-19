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
#include "stdio.h"
#include "magsens.h"


int16_t data_receive=0;
int16_t Xaxis=0;
int16_t Yaxis=0;
int16_t Zaxis=0;


void i2c_init(void)
{
		//blockSleepmode(sleepEM2);

	GPIO_PinModeSet(scl_port,scl_pin,gpioModeWiredAnd,1 ); //SCL Line

	GPIO_PinModeSet(sda_port,sda_pin,gpioModeWiredAnd,1 ); //SDA Line

	I2C0->ROUTEPEN |= I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN; //Setting SCL and SDA

	I2C0->ROUTELOC0 |= (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SCLLOC_MASK))| I2C_ROUTELOC0_SCLLOC_LOC14;

	I2C0->ROUTELOC0 |= (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SDALOC_MASK))| I2C_ROUTELOC0_SDALOC_LOC16;

	//const I2C_Init_TypeDef i2c_reg = I2C_INIT_DEFAULT;
	/*I2C_Init_TypeDef * I2c_init = malloc(sizeof(I2C_Init_TypeDef)); //Initialize a structure pointer to I2C initialization structure
	I2c_init->enable = false; //I2C will be enabled only when it is required
	I2c_init->master = true; // Set to master
	I2c_init->refFreq = 0; //Currently configured reference clock will be used
	I2c_init->freq = I2C_FREQ_FAST_MAX    ; //Not sure about what to keep
	I2c_init->clhr = i2cClockHLRAsymetric  ; //Set to fast mode of 6:3


	//I2C_Init(I2C0,&i2c_reg);
	I2C_Init(I2C0,I2c_init);
*/

	const I2C_Init_TypeDef i2c_reg = I2C_INIT_SEN;
	I2C_Init(I2C0,&i2c_reg);

	I2C_Enable(I2C0,true);
	GPIO_PinOutSet(sensor_port,sensor_pin);

	int i=0;
	while (i<400000)
	{
		i++;
	}

	//Reset I2C Slave if Required
	for (int i=0; i<9 ; i++)
	{
		GPIO_PinOutClear(scl_port,scl_pin);
		GPIO_PinOutSet(scl_port,scl_pin);
		//GPIO_PinOutSet(sda_port,sda_pin);
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


/*------------------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Write byte for I2C.

  Writing to particular slave address and its corresponding register.

 @param:slave_address: Address of slave device(7 bit addressing)
 @param: reg_address: Address of register (8 bit address of Register)
 @param: data_byte: Data that is to be written

 @return: None
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void write_byte(uint8_t slave_address, uint8_t reg_address, uint8_t data_byte){
	I2C0->TXDATA =((slave_address << 1) & (0xFE)); //Set the slave address and write bit which is zero
	I2C0->CMD = I2C_CMD_START; //Start condition

	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for acknowledge signal
	I2C0->IFC = (I2C_IFC_ACK );		//Clear the Acknowledge flag

	I2C0->TXDATA = reg_address;	//Send register address
	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for Acknowledgement signal after sending data
	I2C0->IFC = (I2C_IFC_ACK );		//Clear the Acknowledge flag

	I2C0->TXDATA = data_byte;	//Send data
	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for Acknowledgement signal after sending data
	I2C0->IFC = (I2C_IFC_ACK );		//Clear the Acknowledge flag

	I2C0->CMD = I2C_CMD_STOP;	//Send stop command

}

/*------------------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Read pixel byte for I2C.

  Writing to particular slave address and its corresponding register.

 @param:slave_address: Address of slave device(7 bit addressing)
 @param: reg_address: Address of register (8 bit address of Register)
 @param: data_sixty_four: Pointer to address which will contain sixty four bytes received from I2C sensor

 @return: NONE
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
void read_byte(uint8_t slave_address, uint8_t reg_address, uint8_t* data_sixty_four){
	uint8_t i=0;	//Count to track how many bytes are read

	I2C0->CMD = I2C_CMD_START; //Start condition
	while(I2C0->CMD & I2C_STATUS_PSTART);

	I2C0->TXDATA = ((slave_address << 1) & (0xFE)); //Set the slave address and write bit which is zero

	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for acknowledge signal
	I2C0->IFC = (I2C_IFC_ACK );		//Clear the Acknowledge flag

	I2C0->TXDATA = reg_address;	//Send register address


	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for Acknowledgement signal after sending data
	I2C0->IFC = (I2C_IFC_ACK );		//Clear the Acknowledge flag

	I2C0->CMD = I2C_CMD_START; //Repeated Start condition
	while(I2C0->CMD & I2C_STATUS_PSTART);

	I2C0->TXDATA = ((slave_address << 1) | (0x01)); //Set the slave address and read bit which is one

	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for acknowledge signal
	I2C0->IFC = (I2C_IFC_ACK );		//Clear the Acknowledge flag

	for(i=0;i<127;i++){
		while((I2C0->IF & I2C_IF_RXDATAV) ==0);	//Wait until Receive buffer contains some value
		*(data_sixty_four + i) = I2C0->RXDATA;	//The RXDATAV interrupt flag is cleared when we read data from receive Buffer
		//I2C0->IFC = I2C_IFC_ACK;				//Clear the acknowledgement flag
		I2C0->CMD = I2C_CMD_ACK;				//Transmit acknowledgement from master to slave to request next data
		while(I2C0->STATUS & I2C_STATUS_PACK);

	}

	I2C0->IFC |= I2C_IFC_ACK;				//Clear the acknowledgement flag
	while((I2C0->IF & I2C_IF_RXDATAV) ==0);	//Wait until Receive buffer contains some value
	*(data_sixty_four + i) = I2C0->RXDATA;	//The RXDATAV interrupt flag is cleared when we read data from receive Buffer
	I2C0->CMD |= I2C_CMD_NACK;				//Transmit acknowledgement from master to slave to request next data
	while(I2C0->CMD & I2C_STATUS_PNACK);
	I2C0->CMD = I2C_CMD_STOP;					//Send stop bit
	while(I2C0->CMD & I2C_STATUS_PSTOP);


}


/*------------------------------------------------------------------------------------------------------------------------------------*/
/*
  @brief: Read status byte for I2C.

  Writing to particular slave address and its corresponding register.

 @param:slave_address: Address of slave device(7 bit addressing)
 @param: reg_address: Address of register (8 bit address of Register)

 @return: Data from sensor
 */
/*-----------------------------------------------------------------------------------------------------------------------------*/
uint8_t read_status(uint8_t slave_address, uint8_t reg_address){
		I2C0->IFC |= 0XFF;	//Clear all interrupts

	uint8_t data_byte=0;	//status to be read
	uint8_t i=0;
	I2C0->TXDATA = ((slave_address << 1)); //Set the slave address and write bit which is zero
	I2C0->CMD = I2C_CMD_START; //Start condition
	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for acknowledge signal
	I2C0->IFC |= (I2C_IFC_ACK );		//Clear the Acknowledge flag

	I2C0->TXDATA = reg_address;	//Send register address

	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for Acknowledgement signal after sending data
	I2C0->IFC |= (I2C_IFC_ACK );		//Clear the Acknowledge flag

	I2C0->CMD |= I2C_CMD_START; //Start condition
	I2C0->TXDATA = ((slave_address << 1) | (0x01)); //Set the slave address and read bit which is one
	while((I2C0->IF & I2C_IF_ACK)==0); //Wait for acknowledge signal
	I2C0->IFC |= (I2C_IFC_ACK );		//Clear the Acknowledge flag

		while((I2C0->STATUS & I2C_STATUS_RXDATAV)==0);	//Wait until Receive buffer contains some value
		data_byte = I2C0->RXDATA;	//The RXDATAV interrupt flag is cleared when we read data from receive Buffer
		I2C0->IFC |= I2C_IFC_ACK;				//Clear the acknowledgement flag
		I2C0->CMD |= I2C_CMD_NACK;				//Transmit acknowledgement from master to slave to request next data


	I2C0->CMD |= I2C_CMD_STOP;					//Send stop bit
	for(i=0;i<253;i++);		//Insert some delay
	return data_byte;
}

float mag_sensor(void)
{
	  /*data_receive=read_status(0x1E, 0x27);
	  printf("\n\r Status: %d ",data_receive);
*/
	  data_receive=read_status(0x1E, 0x29);
	 // printf("\tX high: %d ",data_receive);

	  Xaxis=data_receive;
	  Xaxis=(Xaxis << 8);

	  data_receive=read_status(0x1E, 0x28);
	 // printf("\n\rX low: %d ",data_receive);

	  Xaxis = Xaxis | data_receive;
	  double XaxisF = Xaxis * SENSITIVITY;
	  printf("\n\r Xaxis: %1.2f", XaxisF);

	  data_receive=read_status(0x1E, 0x2B);
	 // printf("\tY high: %d ",data_receive);

	  Yaxis=data_receive;
	  Yaxis=(Yaxis << 8);

	  data_receive=read_status(0x1E, 0x2A);
	 // printf("\n\rY low: %d ",data_receive);

	  Yaxis = Yaxis | data_receive;
	  //Yaxis = Yaxis * SENSITIVITY;
	  double YaxisF = Yaxis * SENSITIVITY;
	  //printf("\n\r Yaxis: %d", Yaxis);
	  printf("\n\r Yaxis: %1.2f", YaxisF);

	  data_receive=read_status(0x1E, 0x2D);
	 // printf("\tZ high: %d ",data_receive);

	  Zaxis=data_receive;
	  Zaxis=(Zaxis << 8);

	  data_receive=read_status(0x1E, 0x2C);
	 // printf("\n\rZ low: %d ",data_receive);

	  Zaxis = Zaxis | data_receive;
	  //Zaxis = Zaxis * SENSITIVITY;
	  double ZaxisF = Zaxis * SENSITIVITY;
	  //printf("\n\r Zaxis: %d", Zaxis);
	  printf("\n\r Zaxis: %1.2f\r\n", ZaxisF);

	 /* data_receive=read_status(0x1E, 0x2E);
	  printf("\n\rTemp low: %d ",data_receive);
	  data_receive=read_status(0x1E, 0x2F);
	  printf("\tTemp high: %d ",data_receive);
	  data_receive=read_status(0x1E, 0x30);*/
	 // printf("\n\r Int_cfg: %x ",data_receive);
	  //data_receive=read_status(0x1E, 0x31);
	  //printf("\tInt_src: %d ",data_receive);
	  return XaxisF;

}

//***********************************************************************************
// Include files
//***********************************************************************************
#include "src/gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************
/*void gpio_init(void){

	// Set LED ports to be standard output drive with default off (cleared)
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	//GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	//GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

}*/

void gpio_init(void){

	GPIO_PinModeSet( scl_port , scl_pin , gpioModeWiredAnd , 1 ); //SCL Line
	GPIO_PinModeSet( sda_port , sda_pin , gpioModeWiredAnd , 1 ); //SDA Line
	GPIO_PinModeSet( sensor_port , sensor_pin , gpioModePushPull , 1 ); //enable Sensor Pin For Si7021

	//Reset I2C Slave
	for (int i=0; i<9 ; i++)
	{
		GPIO_PinOutClear( scl_port , scl_pin );
		GPIO_PinOutSet( sda_port , sda_pin );
	}

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	GPIO_PinModeSet( gpioPortF , 6 , gpioModeInputPull, 1 ); //enable Push Button
//	GPIO_PinModeSet( sensor_port , sensor_pin , gpioModePushPull , 1 ); //enable Sensor Pin For Si7021
	//Clear all the Gpio interrupts

//	GPIO->IFC = 0x00000000;

}

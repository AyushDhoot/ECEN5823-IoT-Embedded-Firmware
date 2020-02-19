//***********************************************************************************
// Include files
//***********************************************************************************
#include "main.h"
#include "em_gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED0 pin is
#define	LED0_port gpioPortF
#define LED0_pin 4
#define LED0_default	false 	// off

 //LED1 pin is
#define LED1_port gpioPortF
#define LED1_pin 5
#define LED1_default	false	// off

//Defining SDA and SCL lines
#define sda_port gpioPortC
#define sda_pin 11
#define scl_port gpioPortC
#define scl_pin 10
#define sensor_port gpioPortD
#define sensor_pin 15

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_init(void);


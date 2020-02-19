
/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

#include "retargetserial.h"
#include "stdio.h"

/* Device initialization header */
#include "hal-config.h"
#include "retargetserialhalconfig.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
//#define EM 3

#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

// Gecko configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
};

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;
//***********************************************************************************
// Include files
//***********************************************************************************

#include "src/main.h"
#include "src/gpio.h"
#include "src/sleep.h"
#include "src/cmu.h"
#include "letimer.h"
#include "sleep.h"
#include "i2c.h"
#include "tempsensor.h"



//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************

uint16_t data_receive=0;
//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************


//***********************************************************************************
// main
//***********************************************************************************

/**
 * @brief  Main function
 */
int main(void)
{
   initMcu();   // Initialize device

  initBoard(); // Initialize board

  gpio_init(); /* Initialize GPIO */

  cmu_init(); // Initialize clocks

  gecko_init(&config);  // Initialize stack

  LETimer0Setup();

  RETARGET_SerialInit();
  RETARGET_SerialCrLf(true);		//this is used for proper display of the UART on TERA TERM. If TERA TERM settings are changed, this line can be avoided

  //gecko_init(&config);  // Initialize stack

  event_scheduler=0;
  i2c_init();



  write_byte(0x1E, 0x20, 0x90);
  write_byte(0x1E, 0x22, 0x00);
  write_byte(0x1E, 0x30, 0x08);
  data_receive=read_status(0x1E, 0x20);
  printf("REG1 = %d\r\n",data_receive);
  data_receive=read_status(0x1E, 0x21);
  printf("REG2 = %d\r\n",data_receive);
  data_receive=read_status(0x1E, 0x22);
  printf("REG3 = %d\r\n",data_receive);
  data_receive=read_status(0x1E, 0x23);
  printf("REG4 = %d\r\n",data_receive);
  data_receive=read_status(0x1E, 0x24);
  printf("REG5 = %d\r\n",data_receive);
  data_receive=read_status(0x1E, 0x27);
  printf("STATUS REG = %d\r\n",data_receive);
  uint8_t i = 0;
  while(1)
  {
	  if (event_scheduler==0)
		  sleep();
	  if (event_scheduler & event_1)
	  {
		  i2c_init();
		  	  data_receive=read_status(0x1E, 0x27);
		  	  printf("\n\r Status: %d ",data_receive);
		  	  data_receive=read_status(0x1E, 0x28);
		  	  printf("\n\rX low: %d ",data_receive);
		  	  data_receive=read_status(0x1E, 0x29);
		  	  printf("\tX high: %d ",data_receive);
   		  	  data_receive=read_status(0x1E, 0x2A);
			  printf("\n\rY low: %d ",data_receive);
			  data_receive=read_status(0x1E, 0x2B);
			  printf("\tY high: %d ",data_receive);
			  data_receive=read_status(0x1E, 0x2C);
			  printf("\n\rZ low: %d ",data_receive);
			  data_receive=read_status(0x1E, 0x2D);
			  printf("\tZ high: %d ",data_receive);
			  data_receive=read_status(0x1E, 0x2E);
			  		  	  printf("\n\rTemp low: %d ",data_receive);
			  		  	  data_receive=read_status(0x1E, 0x2F);
			  		  	  printf("\tTemp high: %d ",data_receive);
			  		  	data_receive=read_status(0x1E, 0x30);
			  		  			  	  printf("\n\r Int_cfg: %d ",data_receive);
			  		  			  	  data_receive=read_status(0x1E, 0x31);
			  		  			  	  printf("\tInt_src: %d ",data_receive);

		  	  for(i=0;i<255;i++);
		  	  for(i=0;i<255;i++);
		  	  for(i=0;i<255;i++);
		  	  //for(i=0;i<255;i++);
		  //i2c_driver();
		  //temp_sensor();
		  //i2c_disable();
		  //unblockSleepmode(sleepEM2);
		  event_scheduler &= ~event_1;
	  }
	  }
}

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */

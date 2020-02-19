/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Thermometer Example Application
 *
 * This Thermometer and OTA example allows the user to measure temperature
 * using the temperature sensor on the WSTK. The values can be read with the
 * Health Thermometer reader on the Blue Gecko smartphone app.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board Headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "infrastructure.h"
#include <stdio.h>

/* GATT database */
#include "gatt_db.h"

/* EM library (EMlib) */
#include "em_system.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

#include "retargetserial.h"

/* Device initialization header */
#include "hal-config.h"

#ifdef FEATURE_BOARD_DETECTED
#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#else
#error This sample app only works with a Silicon Labs Board
#endif

#include "src/main.h"
#include "src/gpio.h"
#include "src/sleep.h"
#include "src/cmu.h"
#include "letimer.h"
#include "sleep.h"
#include "i2c.h"
#include "timer.h"
#include "tempsensor.h"
#include "infrastructure.h"
#include "lcdGraphics/lcd_driver.h"
#include "inttypes.h"
//#include "lcd_driver.h"


/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

/* Gecko configuration parameters (see gecko_configuration.h) */
#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

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

/* Flag for indicating DFU Reset must be performed */
uint8_t boot_to_dfu = 0;

/**
 * @brief Function for taking a single temperature measurement with the WSTK Relative Humidity and Temperature (RHT) sensor.
 */
uint32_t tempe;

float temperatureMeasure()
{
	i2c_init();
	i2c_driver();
	float temper=temp_sensor();
	tempe=FLT_TO_UINT32(temper*1000, -3);
//uint32_t x=temper;
uint8_t htmTempBuffer[5]; /* Stores the temperature data in the Health Thermometer (HTM) format. */
uint8_t flags = 0x00;   /* HTM flags set as 0 for Celsius, no time stamp and no temperature type. */
/* Stores the temperature data read from the sensor in the correct format */
uint8_t *p = htmTempBuffer; /* Pointer to HTM temperature buffer needed for converting values to bitstream. */
UINT8_TO_BITSTREAM(p, flags);
/* Convert temperature to bitstream and place it in the HTM temperature data buffer (htmTempBuffer) */
UINT32_TO_BITSTREAM(p, tempe);
/* Send indication of the temperature in htmTempBuffer to all "listening" clients.
* This enables the Health Thermometer in the Blue Gecko app to display the temperature.
*  0xFF as connection ID will send indications to all connections. */
gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_temperature_measurement, 5, htmTempBuffer);

//i2c_disable();
return temper;
}

/**
 * @brief  Main function
 */
int connect;
int main(void)
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();
  cmu_init();
  // Initialize stack
  gecko_init(&config);
  gpio_init();
  // Initialize the Temperature Sensor
  LETimer0Setup();

  Timer0_setup();

  LCD_init("BLE Server");//OR LCD_init("BLE Client");

  RETARGET_SerialInit();
  RETARGET_SerialCrLf(true);

  struct gecko_msg_system_get_bt_address_rsp_t *addr;
  char address[40] = {0};
  char temp[8]={0};
  bool enter=false;

  while (1) {
    /* Event pointer for handling events */
    struct gecko_cmd_packet* evt;

    /* Check for stack event. */
    evt = gecko_wait_event();

    /* Handle events */
    switch (BGLIB_MSG_ID(evt->header)) {
      /* This boot event is generated when the system boots up after reset.
       * Do not call any stack commands before receiving the boot event.
       * Here the system is set to start advertising immediately after boot procedure. */
     case gecko_evt_system_boot_id:
   	   printf("Boot\r\n");
   	  gecko_cmd_sm_delete_bondings();
   	   printf("Delete all bondings\n");
   	   gecko_cmd_sm_configure(0x0F,sm_io_capability_displayyesno);
   	   gecko_cmd_sm_set_bondable_mode(1);
   	   //gecko_cmd_le_gap_set_adv_parameters(160,160,7);
   	   add = gecko_cmd_system_get_bt_address();
   	   LCD_write("ADDRESS", LCD_ROW_BTADDR1);
   	   snprintf(address, 40, "%02x:%02x:%02x:%02x:%02x:%02x",
   	  				  addr->address.addr[5],
   	  				  addr->address.addr[4],
   	  				  addr->address.addr[3],
   	  				  addr->address.addr[2],
   	  				  addr->address.addr[1],
   	  				  addr->address.addr[0]
   	  	  	  );
   	  LCD_write(address, LCD_ROW_BTADDR2);

        /* Set advertising parameters. 100ms advertisement interval.
         * The first two parameters are minimum and maximum advertising interval, both in
         * units of (milliseconds * 1.6). */
        gecko_cmd_le_gap_set_advertise_timing(0, 400, 400, 0, 0);
        gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);

        break;


      case gecko_evt_le_connection_opened_id:
    	  printf("Awaiting Connection\r\n");
    	  gecko_cmd_le_connection_set_parameters(evt->data.evt_le_connection_opened.connection,60,60,3,300);
              		//connect=evt->data.evt_le_connection_opened.connection;
          uint32_t activeConnectionId = evt->data.evt_le_connection_opened.connection;
          gecko_cmd_sm_increase_security(activeConnectionId);
          LCD_write("Press PB0", LCD_ROW_ACTION);
      			break;

      		// Set TX power adaptively based on rssi values. Function sets TX power in units of 0.1 dbm.
      case gecko_evt_le_connection_rssi_id:
      				      gecko_cmd_system_halt(1); // halt radio before setting new TX power value
      				  	  if (evt->data.evt_le_connection_rssi.rssi > -35 ){
      				  	      gecko_cmd_system_set_tx_power(-260);	//setting TX power to min (-26 dbm) when rssi more than -35 dB
      				  	  }else if (evt->data.evt_le_connection_rssi.rssi > -45 && evt->data.evt_le_connection_rssi.rssi <-35){
      				  	      gecko_cmd_system_set_tx_power(-200);
      				  	  }else if(evt->data.evt_le_connection_rssi.rssi > -55 && evt->data.evt_le_connection_rssi.rssi <-45){
      				  	      gecko_cmd_system_set_tx_power(-150);
      				  	  }else if (evt->data.evt_le_connection_rssi.rssi >-65 && evt->data.evt_le_connection_rssi.rssi< -55){
      				  	      gecko_cmd_system_set_tx_power(-50);
      				  	  }else if (evt->data.evt_le_connection_rssi.rssi >-75 && evt->data.evt_le_connection_rssi.rssi <-65){
      				  	      gecko_cmd_system_set_tx_power(0);
      				  	  }else if (evt->data.evt_le_connection_rssi.rssi >-85 && evt->data.evt_le_connection_rssi.rssi < -75){
      				  	      gecko_cmd_system_set_tx_power(50);
      				  	  }else{
      				  	      gecko_cmd_system_set_tx_power(80); //setting TX power to max (8 dbm) when rssi less than -85 dB
      				  	  }
      				  	  gecko_cmd_system_halt(0); // resume radio operation after setting TX power
      				  break;

      /* This event is generated when a connected client has either
       * 1) changed a Characteristic Client Configuration, meaning that they have enabled
       * or disabled Notifications or Indications, or
       * 2) sent a confirmation upon a successful reception of the indication. */
      case gecko_evt_gatt_server_characteristic_status_id:
        /* Check that the characteristic in question is temperature - its ID is defined
         * in gatt.xml as "temperature_measurement". Also check that status_flags = 1, meaning that
         * the characteristic client configuration was changed (notifications or indications
         * enabled or disabled). */

        if (evt-> data.evt_gatt_server_characteristic_status.status_flags == gatt_server_confirmation)
            	     	    	  {
            	     	    		  gecko_cmd_le_connection_get_rssi(evt-> data.evt_gatt_server_characteristic_status.connection);
            	     	    	  }
            	        break;


      case gecko_evt_sm_confirm_passkey_id:
    	  	    printf("Confirm Password\r\n");
                char password[20]={0};
                printf("Are you able to see the same password on the tablet: %lu (y/n)?\r\n", evt->data.evt_sm_confirm_passkey.passkey);
                snprintf(password,sizeof(password),"%lu",evt->data.evt_sm_confirm_passkey.passkey);
                LCD_write(password,LCD_ROW_PASSKEY);
                //int gp = GPIO_PinInGet(gpioPortF,6);
                while(GPIO_PinInGet(gpioPortF,6)!=0);
               // while(gp!=0);
                printf("Password Confirmed\r\n");
                LCD_write("Password Confirmed", LCD_ROW_ACTION);
                LCD_write("",LCD_ROW_PASSKEY);
                gecko_cmd_sm_passkey_confirm(evt->data.evt_sm_confirm_bonding.connection, 1);

                break;

      case gecko_evt_sm_bonded_id:

          	  printf("Bonded");
          	  LCD_write("Bonded", LCD_ROW_ACTION);
          	  enter = true;
          	  break;

      /* This event is generated when the software timer has ticked. In this example the temperature
       * is read after every 1 second and then the indication of that is sent to the listening client. */
      case gecko_evt_system_external_signal_id:
     	         /* Measure the temperature as defined in the function temperatureMeasure() */
    	  LCD_write("Connected???", LCD_ROW_CONNECTION);
     	       if (enter==true)
     	         {
     	    	  LCD_write("Connected!!!!", LCD_ROW_CONNECTION);
     	    	  float t=temperatureMeasure();//temperatureMeasure();
     	    	   LCD_write("Connected", LCD_ROW_CONNECTION);
     	    	   snprintf(temp, 8 , "%.2f" , t);
     	           LCD_write(temp, LCD_ROW_TEMPVALUE);
  	  	  	  	  }
     	       	 break;



      case gecko_evt_le_connection_closed_id:
        /* Check if need to boot to dfu mode */
        if (boot_to_dfu) {
          /* Enter to DFU OTA mode */
          gecko_cmd_system_reset(2);
        } else {
          /* Stop timer in case client disconnected before indications were turned off */
          gecko_cmd_hardware_set_soft_timer(0, 0, 0);
          /* Restart advertising after client has disconnected */
          gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
        }
        break;

      case gecko_evt_sm_bonding_failed_id:
    	  LCD_write("Bonding Failed", LCD_ROW_ACTION);
    	  break;



      /* Events related to OTA upgrading
         ----------------------------------------------------------------------------- */

      /* Checks if the user-type OTA Control Characteristic was written.
       * If written, boots the device into Device Firmware Upgrade (DFU) mode. */
      case gecko_evt_gatt_server_user_write_request_id:
        if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
          /* Set flag to enter to OTA mode */
          boot_to_dfu = 1;
          /* Send response to Write Request */
          gecko_cmd_gatt_server_send_user_write_response(
            evt->data.evt_gatt_server_user_write_request.connection,
            gattdb_ota_control,
            bg_err_success);

          /* Close connection to enter to DFU OTA mode */
          gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
        }
        break;

      default:
        break;
    }
  }
}

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */

/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs BT Mesh Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko BT Mesh C application.
 * The application starts unprovisioned Beaconing after boot
 ***************************************************************************************************
 * <b> (C) Copyright 2017 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>
#include "mesh_generic_model_capi_types.h"
#include "mesh_lighting_model_capi_types.h"
#include "mesh_lib.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>
#include <em_rtcc.h>
#include <gpiointerrupt.h>

/* Device initialization header */
#include "hal-config.h"

/* Display Interface header */
#include "display_interface.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif


/*User defined includes*/
#include "lcd_driver.h"
#include "retargetserial.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "magsens.h"
#include "i2c.h"
#include "letimer.h"
#include "src/cmu.h"


/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

// bluetooth stack heap
#define MAX_CONNECTIONS 2

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

const gecko_configuration_t config =
{
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .max_timers = 16,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,						//for enabling the deep sleep for the node
};
/*
OOB parameters
*/
#define P_K 	(0x00)
#define A_M 	(0x03)
#define O_A 	(0x03)
#define O_S 	(0x04)
#define I_A 	(0x00)
#define I_S 	(0x04)
#define O_L 	(0x01)
#define BITMASK (0x01)


//
////OOB parameters for 2 digit
//#define P_K 	(0x00)
//#define A_M 	(0x08)
//#define O_A 	(0x08)
//#define O_S 	(0x02)
//#define I_A 	(0x00)
//#define I_S 	(0x00)
//#define O_L 	(0x00)
//#define BITMASK (0x01)


/*BUTTON*/
#define BUTTON_PORT	gpioPortF
#define PB0 	(6)
#define PB1 	(7)


/*LED STATE DEFINITIONS*/
#define LED_STATE_OFF 				(0)			//both LEDs OFF
#define LED_STATE_ON  				(1)			//both LEDs ON
#define LED_STATE_PROV   			(3)         //provisioning (LEDs blinking)
#define LED_STATE_ALL_OFF			(4)			//All LEDs OFF
#define LED_STATE_TRAFFIC_ON		(5)			//LED1 ON
#define LED_STATE_TRAFFIC_OFF		(6)			//LED1 oFF
#define LED_STATE_STREET_LIGHT_ON	(7)			//LED0 ON
#define LED_STATE_STREET_LIGHT_OFF	(8)			//LED0 ON


/*LED*/
#define TURN_LED_OFF   		GPIO_PinOutClear
#define TURN_LED_ON    		GPIO_PinOutSet
#define LED_PORT 			gpioPortF
#define LED1_PIN			5
#define LED0_PIN 			4
#define LED_DEFAULT_STATE  	0




/** Timer Frequency */
#define TIMER_CLK_FREQ ((uint32)32768)
/** Convert msec to timer ticks. */
#define TIMER_MS_2_TIMERTICK(ms) ((TIMER_CLK_FREQ * ms) / 1000)

/*SOFT TIMER ID*/

#define TIMER_ID_FACTORY_RESET  		(77)
#define TIMER_ID_RETRANS    			(10)
#define TIMER_ID_FRIEND_FIND 			(20)
#define TIMER_ID_RESTART    			(78)
#define TIMER_ID_SAVE_STATE	   			(60)
#define TIMER_ID_SIGNAL 				(40)
#define TIMER_ID_PROVISIONING   		(66)
#define TIMER_ID_DELAYED_PRI_LEVEL      (45)
#define TIMER_ID_PRI_LEVEL_TRANSITION   (44)


/*variables*/
static uint16 _elem_index=0xFFFF;
static uint16 _primary_elem_index = 0xffff; /* For indexing elements of the node */
uint8_t num_connections;
uint8_t conn_handle=0xFF;
uint8 request_count;

/*functions*/
void node_init(void);
void lpn_init(void);

/*PS data*/
static int persistent_load(void);
static int persistent_save(void);
static void persistent_state_change(void);

/*Persistent Data Store related structure and Function*/
static PACKSTRUCT(struct lightbulb_state {
  // On/Off Server state
  uint8_t mag_thres;
  uint8_t power_up;

  // Primary Generic Level
  int16_t pri_level_current;
  int16_t pri_level_target;
}) lightbulb_state;


uint32_t delayed_pri_level_trans = 0;

static void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
void mesh_native_bgapi_init(void);
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

static errorcode_t pri_level_update(uint16_t element_index);
static errorcode_t pri_level_response(uint16_t element_index,uint16_t client_addr,uint16_t appkey_index);
static errorcode_t pri_level_update_and_publish(uint16_t element_index);


/**
 * button initialization. Configure pushbuttons PB0,PB1
 * as inputs.
 */
static void button_init()
{
  // configure pushbutton PB0 and PB1 as inputs, with pull-up enabled
  GPIO_PinModeSet(BUTTON_PORT, PB0, gpioModeInputPull, 1);
  GPIO_PinModeSet(BUTTON_PORT, PB1, gpioModeInputPull, 1);
}

/**
 * LED initialization. Configure LED pins as outputs
 */
static void led_init()
{
  // configure LED0 and LED1 as outputs
  GPIO_PinModeSet(LED_PORT, LED0_PIN, gpioModePushPull, LED_DEFAULT_STATE);
  GPIO_PinModeSet(LED_PORT, LED1_PIN, gpioModePushPull, LED_DEFAULT_STATE);
}

/*CHANGE LED STATE FUNCTION*/
static void LED_set_state(int state)
{
  switch (state) {
    case LED_STATE_OFF:
      TURN_LED_OFF(LED_PORT, LED0_PIN);
      TURN_LED_OFF(LED_PORT, LED1_PIN);
      break;
    case LED_STATE_ON:
      TURN_LED_ON(LED_PORT, LED0_PIN);
      TURN_LED_ON(LED_PORT, LED1_PIN);
      break;
    case LED_STATE_PROV:
	  GPIO_PinOutToggle(LED_PORT, LED0_PIN);
	  GPIO_PinOutToggle(LED_PORT, LED1_PIN);
	  break;
	case LED_STATE_ALL_OFF:
	  TURN_LED_OFF(LED_PORT, LED0_PIN);
	  TURN_LED_OFF(LED_PORT, LED1_PIN);
	  break;
	case LED_STATE_TRAFFIC_ON:
	  TURN_LED_ON(LED_PORT, LED0_PIN);
	  break;
	case LED_STATE_TRAFFIC_OFF:
	  TURN_LED_OFF(LED_PORT, LED0_PIN);
	  break;
	case LED_STATE_STREET_LIGHT_ON:
	  TURN_LED_ON(LED_PORT, LED1_PIN);
	  break;
	case LED_STATE_STREET_LIGHT_OFF:
	  TURN_LED_OFF(LED_PORT, LED1_PIN);
	  break;

    default:
      break;
  }
}

int persistent_save(void)
{
struct gecko_msg_flash_ps_save_rsp_t* pSave;

  pSave = gecko_cmd_flash_ps_save(0x4004, sizeof(struct lightbulb_state), (const uint8*)&lightbulb_state);

  if (pSave->result) {
    printf("lightbulb_state_store(): PS save failed, code %x\r\n", pSave->result);
    return(-1);
  }
  printf("Persistent Store successful\r\n");
  return 0;
}

static void persistent_state_change(void)
{
  gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(5000), TIMER_ID_SAVE_STATE, 1);
}

int persistent_load(void)
{
	struct gecko_msg_flash_ps_load_rsp_t* pLoad;

	  pLoad = gecko_cmd_flash_ps_load(0x4004);

	  if (pLoad->result) {
	    memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));
	    lightbulb_state.pri_level_current = 0x0000;
	    lightbulb_state.pri_level_target = 0x0000;
	    lightbulb_state.mag_thres= 0.5;
	    return -1;
	  }

	  memcpy(&lightbulb_state, pLoad->value.data, pLoad->value.len);
	  return 0;

}
void initiate_factory_reset(void)
{
	printf("factory reset\r\n");
	LCD_write("\n***\nFACTORY RESET\n***", LCD_ROW_ACTION);

  /* if connection is open then close it before rebooting */


  /* perform a factory reset by erasing PS storage. This removes all the keys and other settings
     that have been configured for this node */
	gecko_cmd_flash_ps_erase_all();
  // reboot after a small delay
	gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_FACTORY_RESET, 1);
}

void set_device_name(bd_addr *pAddr)
{
  char name[20];
  uint16 res;

  // create unique device name using the last two bytes of the Bluetooth address
  sprintf(name, "LPN node %x:%x", pAddr->addr[1], pAddr->addr[0]);

  printf("Device name: '%s'\r\n", name);

  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name)->result;
  if (res) {
    printf("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\r\n", res);
  }

  // show device name on the LCD
  LCD_write(name, LCD_ROW_BTADDR1);
}


/**
 * this function is called each time the lightbulb state in RAM is changed. It sets up a soft timer
 * that will save the state in flash after small delay. The purpose is to reduce amount of unnecessary
 * flash writes.
 */
static void lightbulb_state_changed(void)
{
  gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(5000), TIMER_ID_SAVE_STATE, 1);
}

void lpn_publish(void)
{
	request_count--;
	lightbulb_state.pri_level_current = 0x01;
	lightbulb_state.pri_level_target = 0xFF;
	pri_level_update_and_publish(_elem_index);
	gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(50), TIMER_ID_RETRANS, 0);
}

static void pri_level_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  // for simplicity, this demo assumes that all level requests use set level.
  // other type of requests are ignored

 // uint16_t lightness;

  if (request->kind != mesh_generic_request_level) {
    return;
  }

  printf("pri_level_request: level=%d, transition=%lu, delay=%u\r\n",
         request->level, transition_ms, delay_ms);

  int16 light_level = request->level;
  printf("Light level = %d\r\n", light_level);

  int8 street_light = light_level;
  printf("Street Light level = %d\r\n", street_light);

  int8 traffic_light = (light_level>>8);
  printf("Traffic Light level = %d\r\n", traffic_light);

  if(street_light == 0x0F)
  	  {
	  printf("Street lights ON\r\n");
	  LED_set_state(7);
  	  }
  else
  	  {
	  printf("Street lights OFF\r\n");
	  LED_set_state(8);
  	  }

  if(traffic_light == 0x0F)
  	  {
	  printf("Traffic lights ON\r\n");
	  LED_set_state(5);
  	  }
  else
  	  {
	  printf("Traffic lights OFF\r\n");
	  LED_set_state(6);
  	  }

  if (lightbulb_state.pri_level_current == request->level) {
    printf("Request for current state received; no op\n");
  } else {
    printf("Setting pri_level to <%d>\r\n", request->level);
    lightbulb_state.pri_level_current = request->level;
      // lightbulb current state will be updated when transition is complete
    }
    lightbulb_state_changed();

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    pri_level_response(element_index, client_addr, appkey_index);
  } else {
    pri_level_update(element_index);
  }
}

static void pri_level_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  if (lightbulb_state.pri_level_current != current->level.level) {
    printf("pri_level_change: from %d to %d\r\n", lightbulb_state.pri_level_current, current->level.level);
    lightbulb_state.pri_level_current = current->level.level;
    lightbulb_state_changed();
  } else {
    printf("pri_level update -same value (%d)\r\n", lightbulb_state.pri_level_current);
  }
}

void pri_level_transition_complete()
{
  // transition done -> set state, update and publish
  lightbulb_state.pri_level_current = lightbulb_state.pri_level_target;

  printf("transition complete. New pri_level is %d\r\n", lightbulb_state.pri_level_current);

  lightbulb_state_changed();
  pri_level_update_and_publish(_primary_elem_index);
}


static void init_models(void)
{
  /*mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                           0,
                                           onoff_request,
                                           onoff_change);*/
  mesh_lib_generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                           0,
                                           pri_level_request,
                                           pri_level_change);
}

static errorcode_t pri_level_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.pri_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.pri_level_target;

  return mesh_lib_generic_server_response(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          &target,
                                          0,
                                          0x00);
}

static errorcode_t pri_level_update(uint16_t element_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.pri_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.pri_level_target;

  return mesh_lib_generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        &target,
                                        0);
}

static errorcode_t pri_level_update_and_publish(uint16_t element_index)
{
  errorcode_t e;

  e = pri_level_update(element_index);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_generic_state_level);
  }

  return e;
}

static void server_state_changed(struct gecko_msg_mesh_generic_server_state_changed_evt_t *pEvt)
{
  int i;

  printf("state changed: ");
  printf("model ID %4.4x, type %2.2x ", pEvt->model_id, pEvt->type);
  for (i = 0; i < pEvt->parameters.len; i++) {
    printf("%2.2x ", pEvt->parameters.data[i]);
  }
  printf("\r\n");
}

void node_init(void)
{
  mesh_lib_init(malloc, free, 8);
  printf("Initializing node\r\n");
  lpn_init();

}

/*
 * Initialize LPN functionality with configuration and friendship establishment.
 */
void lpn_init(void)
{
  uint16 res;
  // Initialize LPN functionality.
  res = gecko_cmd_mesh_lpn_init()->result;
  if (res) {
    printf("LPN init failed (0x%x)\r\n", res);
    return;
  }

  res = gecko_cmd_mesh_lpn_configure(2, 5 * 1000)->result;
  if (res) {
    printf("LPN conf failed (0x%x)\r\n", res);
    return;
  }

  printf("trying to find friend.....\r\n");
  res = gecko_cmd_mesh_lpn_establish_friendship(0)->result;

  if (res != 0) {
    printf("ret.code %x\r\n", res);
  }

  memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));
    if (persistent_load() != 0) {
      printf("persistent_load() failed, using defaults\r\n");
      goto publish;
    }
    else
    {
    	lightbulb_state.power_up=0x02;
    	//persistent_state_change();
    }

    switch (lightbulb_state.power_up) {
        case MESH_GENERIC_ON_POWER_UP_STATE_OFF:
          printf("On power up state is OFF\r\n");
          lightbulb_state.pri_level_current = MESH_GENERIC_ON_OFF_STATE_OFF;
          lightbulb_state.pri_level_target = MESH_GENERIC_ON_OFF_STATE_OFF;

          //LEDS_SetState(LED_STATE_OFF);
          //LEDS_SetTemperature(lightbulb_state.temperature_default, lightbulb_state.deltauv_default, 0);
          break;
        case MESH_GENERIC_ON_POWER_UP_STATE_ON:
          printf("On power up state is ON\r\n");
          lightbulb_state.pri_level_current = MESH_GENERIC_ON_OFF_STATE_ON;
          lightbulb_state.pri_level_target = MESH_GENERIC_ON_OFF_STATE_ON;
          //LEDS_SetState(LED_STATE_ON);
          //LEDS_SetTemperature(lightbulb_state.temperature_default, lightbulb_state.deltauv_default, 0);
          break;
        case MESH_GENERIC_ON_POWER_UP_STATE_RESTORE:
          printf("On power up state is RESTORE\r\n");
          int16 light_level = lightbulb_state.pri_level_current;
            printf("Light level = %d\r\n", light_level);

            int8 street_light = light_level;
            printf("Street Light level = %d\r\n", street_light);

            int8 traffic_light = (light_level>>8);
            printf("Traffic Light level = %d\r\n", traffic_light);

            if(street_light == 0x0F)
            	  {
          	  printf("Street lights ON\r\n");
          	  LED_set_state(7);
            	  }
            else
            	  {
          	  printf("Street lights OFF\r\n");
          	  LED_set_state(8);
            	  }

            if(traffic_light == 0x0F)
            	  {
          	  printf("Traffic lights ON\r\n");
          	  LED_set_state(5);
            	  }
            else
            	  {
          	  printf("Traffic lights OFF\r\n");
          	  LED_set_state(6);
            	  }
          break;
      }

      publish:
      lightbulb_state_changed();
      persistent_state_change();
      init_models();
      pri_level_update(_elem_index);
    }



int main()
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  gecko_stack_init(&config);
  gecko_bgapi_class_dfu_init();
  gecko_bgapi_class_system_init();
  gecko_bgapi_class_le_gap_init();
  gecko_bgapi_class_le_connection_init();
  gecko_bgapi_class_gatt_init();
  gecko_bgapi_class_gatt_server_init();
  gecko_bgapi_class_endpoint_init();
  gecko_bgapi_class_hardware_init();
  gecko_bgapi_class_flash_init();
//  gecko_bgapi_class_test_init();
  //gecko_bgapi_class_sm_init();
  mesh_native_bgapi_init();
  gecko_initCoexHAL();


  gecko_bgapi_class_mesh_node_init();
//  gecko_bgapi_class_mesh_prov_init();
  gecko_bgapi_class_mesh_proxy_init();
  gecko_bgapi_class_mesh_proxy_server_init();
  //gecko_bgapi_class_mesh_proxy_client_init();
  //gecko_bgapi_class_mesh_generic_client_init();
  gecko_bgapi_class_mesh_generic_server_init();
  //gecko_bgapi_class_mesh_vendor_model_init();
  //gecko_bgapi_class_mesh_health_client_init();
  //gecko_bgapi_class_mesh_health_server_init();
  //gecko_bgapi_class_mesh_test_init();
  gecko_bgapi_class_mesh_lpn_init();
  //gecko_bgapi_class_mesh_friend_init();

  RETARGET_SerialInit();
  RETARGET_SerialCrLf(true);

  cmu_init();

  LCD_init("BT LPN");

  button_init();
  led_init();
  i2c_init();

  //Initializing Magnetometer
  write_byte(0x1E, 0x20, 0xF0);
  write_byte(0x1E, 0x21, 0x00);
  write_byte(0x1E, 0x22, 0x00);
  write_byte(0x1E, 0x30, 0x08);

  gpio_init();

  DI_Init();
  LCD_init("BT LPN");


  int p= persistent_load();
  if(p==0)
  {
	  uint8_t mag_read= lightbulb_state.mag_thres;
	  printf("Persistent Load Successful\r\n");
	  printf("\r\n Data= %x\r\n", mag_read);
  }
  else
	  printf("PS Load failed\r\n");

  while (1) {
    struct gecko_cmd_packet *evt = gecko_wait_event();
    bool pass = mesh_bgapi_listener(evt);
    if (pass) {
      handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
    }
  }
}

static void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
  uint16_t result;

   struct gecko_msg_mesh_node_provisioning_failed_evt_t  *prov_fail_evt;

   if (NULL == evt) {
	return;
	}

  switch (evt_id) {
/*    case gecko_evt_dfu_boot_id:
      //gecko_cmd_le_gap_set_advertising_timing(0, 1000*adv_interval_ms/625, 1000*adv_interval_ms/625, 0, 0);
      gecko_cmd_le_gap_set_mode(2, 2);
      break;*/
    case gecko_evt_system_boot_id:
      // Initialize Mesh stack in Node operation mode, wait for initialized event

      printf("BOOT ID\r\n");
      if (GPIO_PinInGet(BUTTON_PORT, PB0) == 0 || GPIO_PinInGet(BUTTON_PORT, PB1) == 0)
      	  {
          	  initiate_factory_reset();
          } else{
        //gecko_cmd_le_gap_set_advertising_timing(0, 1000*adv_interval_ms/625, 1000*adv_interval_ms/625, 0, 0);
      //gecko_cmd_le_gap_set_mode(2, 2);
      struct gecko_msg_system_get_bt_address_rsp_t *pAddr = gecko_cmd_system_get_bt_address();
        	set_device_name(&pAddr->address);
       }
      result = gecko_cmd_mesh_node_init_oob(P_K, A_M, O_A, O_S, I_A, I_S, O_L) -> result;
      if(result)
      {
    	  printf("OOB Failed\r\n");
    	  LCD_write("OOB failed", LCD_ROW_BTADDR1);
      }
      break;

    case gecko_evt_le_connection_closed_id:
      /* Check if need to boot to dfu mode */
      if (boot_to_dfu) {
        /* Enter to DFU OTA mode */
        gecko_cmd_system_reset(2);
      }
      printf("evt:conn closed, reason 0x%x\r\n", evt->data.evt_le_connection_closed.reason);
      conn_handle = 0xFF;
      if (num_connections > 0) {
        if (--num_connections == 0) {
         LCD_write("", LCD_ROW_CONNECTION);
        }
      }

      break;

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

    case gecko_evt_mesh_node_display_output_oob_id:
        {
        printf("Output OOB ID\r\n");
		struct gecko_msg_mesh_node_display_output_oob_evt_t *pOOB = (struct gecko_msg_mesh_node_display_output_oob_evt_t *)&(evt->data);
		printf("gecko_msg_mesh_node_display_output_oob_evt_t: action %d, size %d\r\n", pOOB->output_action, pOOB->output_size);

		for(uint8_t i=0;i<pOOB->data.len;i++)
		{
			printf("%d ", pOOB->data.data[i]);
		}
		printf("\r\n");
        }
		break;

    case gecko_evt_mesh_node_initialized_id:
	    printf("node initialized\r\n");

	    gecko_cmd_mesh_generic_server_init();
	    struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);
	    if (pData->provisioned) {
		   printf("node is provisioned. address:%x, ivi:%ld\r\n", pData->address, pData->ivi);
		   LCD_write("Provisioned",LCD_ROW_BTADDR1);
		   _elem_index = 0;
		   node_init();
		   init_models();
		   LETimer0Setup();
		   }
		   else{
		   printf("starting unprovisioned beaconing...\r\n");
		   LCD_write("Unprovisioned",LCD_ROW_BTADDR1);
		   //gecko_cmd_mesh_node_start_unprov_beaconing(0x3);   // enable ADV and GATT provisioning bearer
		   gecko_cmd_mesh_node_start_unprov_beaconing(0x2);   // enable GATT provisioning bearer
		   }
	   break;

    case gecko_evt_mesh_node_provisioning_started_id:
          printf("Started provisioning\r\n");

          // start timer for blinking LEDs to indicate which node is being provisioned
           break;

    case gecko_evt_mesh_node_provisioned_id:
    	printf("Node Successfully provisioned\r\n");
    	LCD_write("Provisioned",LCD_ROW_BTADDR1);
    	node_init();
    	LETimer0Setup();
    	_elem_index = 0;
    	break;

    case gecko_evt_mesh_node_provisioning_failed_id:
          prov_fail_evt = (struct gecko_msg_mesh_node_provisioning_failed_evt_t  *)&(evt->data);
          printf("provisioning failed, code %x\r\n", prov_fail_evt->result);
          //DI_Print("prov failed", DI_ROW_STATUS);
          /* start a one-shot timer that will trigger soft reset after small delay */
         // gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, 1);
          break;

    case gecko_evt_mesh_node_key_added_id:
          printf("got new %s key with index %x\r\n", evt->data.evt_mesh_node_key_added.type == 0 ? "network" : "application",
                 evt->data.evt_mesh_node_key_added.index);
          break;

    case gecko_evt_mesh_node_model_config_changed_id:
      printf("model config changed\r\n");
      break;

    case gecko_evt_le_connection_opened_id:
		 num_connections++;
		 gecko_cmd_mesh_lpn_deinit(); //Turn off LPN Feature
		 conn_handle=evt->data.evt_le_connection_opened.connection;
		 LCD_write("Connected", LCD_ROW_ACTION);
		 gecko_cmd_mesh_lpn_deinit();
		 LCD_write("LPN OFF", LCD_ROW_ACTION);
		 break;

    case gecko_evt_mesh_node_reset_id:
      printf("evt gecko_evt_mesh_node_reset_id\r\n");
      initiate_factory_reset();
      break;

    case gecko_evt_hardware_soft_timer_id:
		switch (evt->data.evt_hardware_soft_timer.handle) {

		case TIMER_ID_FACTORY_RESET:
			gecko_cmd_system_reset(0);
			break;

		case TIMER_ID_RESTART:
			  gecko_cmd_system_reset(0);
			  break;
		case TIMER_ID_PROVISIONING:
			  LED_set_state(LED_STATE_PROV);
			  break;

		case TIMER_ID_RETRANS:
			printf("\r\n In Timer ID Retrans\r\n");
			lpn_publish();
    		// stop retransmission timer if it was the last attempt
			if (request_count == 0) {
				gecko_cmd_hardware_set_soft_timer(0, TIMER_ID_RETRANS, 0);
			}
			break;

		case TIMER_ID_FRIEND_FIND:
		{
			printf("trying to find friend...\r\n");
			result = gecko_cmd_mesh_lpn_establish_friendship(0)->result;
			if (result != 0) {
				printf("ret.code %x\r\n", result);
			}
		}
			break;

		case TIMER_ID_SAVE_STATE:
			persistent_save();
			break;

		  }
		break;

    case gecko_evt_mesh_lpn_friendship_established_id:
      printf("friendship established\r\n");
      LCD_write("LPN", LCD_ROW_CLIENTADDR);
      break;

    case gecko_evt_mesh_lpn_friendship_failed_id:
      printf("friendship failed\r\n");
      LCD_write("no friend", LCD_ROW_CLIENTADDR);
      // try again in 2 seconds
      result  = gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(2000), TIMER_ID_FRIEND_FIND, 1)->result;
      if (result) {
        printf("timer failure?!  %x\r\n", result);
      }
      break;

    case gecko_evt_mesh_lpn_friendship_terminated_id:
      printf("friendship terminated, code0x%x\r\n", evt->data.evt_mesh_lpn_friendship_terminated.reason);
      LCD_write("friend lost",LCD_ROW_CLIENTADDR);

      if (num_connections == 0) {
        // try again in 2 seconds
        result  = gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(2000), TIMER_ID_FRIEND_FIND, 1)->result;
        if (result) {
          printf("timer failure?!  %x\r\n", result);
        }
      }
      break;

    case gecko_evt_system_external_signal_id:
    	printf("Entering External Signal ID\r\n");
    	request_count=3;
    	lpn_publish();
    	break;

    case gecko_evt_mesh_generic_server_client_request_id:
    	printf("Entering the server client request id \r\n");
    	mesh_lib_generic_server_event_handler(evt);
    	break;

    case gecko_evt_mesh_generic_server_state_changed_id:

	  // get debug prints for each server state changed event
        server_state_changed(&(evt->data.evt_mesh_generic_server_state_changed));

	  // pass the server state changed event to mesh lib handler that will invoke
	  // the callback functions registered by application
	    mesh_lib_generic_server_event_handler(evt);
	    break;

    default:
      break;
  }
}

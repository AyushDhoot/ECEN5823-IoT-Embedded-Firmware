/*
 * letimer.c
 *
 *  Created on: 15-Sep-2018
 *      Author: Ayush
 */
#include "em_letimer.h"
#include "letimer.h"
#include "src/gpio.h"
#include "em_core.h"
#include "src/main.h"
#include "src/cmu.h"
#include "math.h"
#include "src/main.h"
#include "sleep.h"
#include "src/sleep.h"
#include "tempsensor.h"
#include "i2c.h"

int k;
int comp_1;
int freq=32768;

/* Set configurations for LETIMER 0 */
void LETimer0Setup()
{
	blockSleepmode(sleepEM4);
	LETIMER_IntClear(LETIMER0,LETIMER_IFC_COMP0);
 const LETIMER_Init_TypeDef letimerInit =
 {
  .enable         = false,                   /* Start counting when init completed. */
  .debugRun       = false,                  /* Counter shall not keep running during debug halt. */
  //.rtcComp0Enable = false,                  /* Don't start counting on RTC COMP0 match. */
  //.rtcComp1Enable = false,                  /* Don't start counting on RTC COMP1 match. */
  .comp0Top       = true,                   /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
  .bufTop         = false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
  .out0Pol        = 0,                      /* Idle value for output 0. */
  .out1Pol        = 0,                      /* Idle value for output 1. */
  .ufoa0          = letimerUFOANone,         /* no PWM output on output 0 */
  .ufoa1          = letimerUFOANone,       /* no Pulse output on output 1*/
  .repMode        = letimerRepeatFree       /* Count until stopped */
  };
  /* Initialize LETIMER */
  LETIMER_Init(LETIMER0, &letimerInit);
  while(LETIMER0->SYNCBUSY!=0);
  LETIMER_IntClear(LETIMER0,LETIMER_IFC_COMP0);
  //LETIMER0->IFC&=0x000000000;

  //CMU->LFAPRESC0=CMU_LFAPRESC0_
  int comp_0;
  freq = 1000;
  comp_0=freq*period;

/*  if (EnergyMode<=3)
  {
	  if (period>2)
	   {
	   int tick=freq*period;
	   int k=tick/65536;
	   k = pow(2,k);
	   CMU_ClockDivSet(cmuClock_LETIMER0,k);
	   comp_0=tick/k;
	 //  comp_1= freq*(ledON/k);
	   }
	   else if (period<=2)*/
	  // {

	  	//  int comp_1= 16384*ledON;
	   //}

	  // comp_1=2867;
  	  //}
  //else if (EnergyMode>3)
  	  	//  {
  		  // freq = 1000;
  		/* if (period>2)
  		 	   {
  		 	   int tick=freq*period;
  		 	   int k=tick/65536;
  		 	   k = pow(2,k);
  		 	   CMU_ClockDivSet(cmuClock_LETIMER0,k);
  		 	   comp_0=tick/k;
  		 	//   comp_1= (comp_0-(freq*ledON));
  		 	   }
  		 	   else if (period<=2)
  		 	   {
  		*/ 	// 	  comp_0=freq*period;
  		 	  	//  int comp_1= 16384*ledON;
  		 	   //}
  		   //comp_1=175;

  	  	  //}



  LETIMER_CompareSet(LETIMER0, 0, comp_0);
//  LETIMER_CompareSet(LETIMER0, 1, comp_1);

  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP0);//BlockSleepMode
  NVIC_EnableIRQ(LETIMER0_IRQn);

  LETIMER_Enable(LETIMER0, true);
  LETIMER0 -> CMD = LETIMER_CMD_START;

 // blockSleepmode(EnergyMode);
}

void LETIMER0_IRQHandler(void)
	{

		CORE_ATOMIC_IRQ_DISABLE();

		blockSleepmode(sleepEM2);

			int j;
			j=LETIMER_IntGet(LETIMER0);
			if (j & LETIMER_IF_COMP0)
			{
				event_scheduler|=LETIMER_IF_COMP0;
			}
			/*i2c_init();
			i2c_driver();
			temp_sensor();
			i2c_disable();
			unblockSleepmode(sleepEM2);*/
			//}

			LETIMER_IntClear(LETIMER0,j);
/*
			int j;
			j=LETIMER_IntGet(LETIMER0);

			if (j & LETIMER_IF_COMP0)
			{
				GPIO_PinOutClear(LED0_port, LED0_pin);
				LETIMER_IntClear(LETIMER0,LETIMER_IFC_COMP0 | LETIMER_IF_UF);

				 //LETIMER_CompareSet(LETIMER0, 1, comp_1);
				 //LETIMER_IntClear(LETIMER0,LETIMER_IFC_COMP1);
				 LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP1);
			}
			if (LETIMER_IF_COMP1 & j)
				{
				GPIO_PinOutSet(LED0_port, LED0_pin);
				LETIMER_IntClear(LETIMER0,LETIMER_IFC_COMP1);
				LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP0);
				}

*/


	//	for (i = 0; i < 1750000; i++)
		//{
			//	GPIO_PinOutSet(LED0_port, LED0_pin);
				//GPIO_PinOutSet(LED1_port, LED1_pin);
	//	}
		//GPIO_PinOutToggle(LED0_port, LED0_pin);
		//GPIO_PinOutClear(LED0_port, LED0_pin);

			CORE_ATOMIC_IRQ_ENABLE();

	}



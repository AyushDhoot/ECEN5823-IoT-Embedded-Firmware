//***********************************************************************************
// Include files
//***********************************************************************************
#include "src/cmu.h"
#include "em_cmu.h"
#include "em_letimer.h"
#include "main.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

//volatile int EnergyMode;
void cmu_init(void){
	CMU_ClockEnable(cmuClock_GPIO, true);  // Peripheral clocks enabled

//	if (EnergyMode<=3)
	//{
	//CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
	//CMU_LFXOInit(&lfxoInit);
	CMU_OscillatorEnable(cmuOsc_LFXO,true,true);
	CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_LFXO);
	CMU_ClockEnable(cmuClock_LFA,true);
	//}
	//if (EnergyMode>3)
	//{
	CMU_OscillatorEnable(cmuOsc_ULFRCO,true,true);
	CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_ULFRCO);
	CMU_ClockEnable(cmuClock_LFA,true);
	//}

	CMU_ClockEnable(cmuClock_HFPER,true);
	CMU_ClockEnable(cmuClock_I2C0,true);
	CMU_ClockEnable(cmuClock_CORELE,true);
	CMU_ClockEnable(cmuClock_LETIMER0,true);



}


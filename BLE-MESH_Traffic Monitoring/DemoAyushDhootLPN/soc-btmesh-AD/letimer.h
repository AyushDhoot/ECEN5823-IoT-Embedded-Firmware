/*
 * letimer.h
 *
 *  Created on: 07-Dec-2018
 *      Author: Ayush
 */

#ifndef LETIMER_H_
#define LETIMER_H_

#include "em_letimer.h"
#include "native_gecko.h"

void LETIMER0_IRQHandler(void);
void LETimer0Setup();
//void LETIMER_Enable();
#define period 8

#endif /* LETIMER_H_ */

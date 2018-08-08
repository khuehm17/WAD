/******************************************************
 *      Delay using System Tick Timer Header File
 *
 *****************************************************/

#ifndef _SYS_DELAY_H_
#define _SYS_DELAY_H_

/* Include Header Files */
#include "board.h"

/* FUNCTION */
void Systick_Delay_Init();
void SysTick_Handler (void);
void ms_delay(uint32_t dlyTicks);

#endif /* _SYS_DELAY_H_ */

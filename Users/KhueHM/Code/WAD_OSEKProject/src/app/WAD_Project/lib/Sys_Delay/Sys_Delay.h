/******************************************************
 *      Delay using System Tick Timer Header File
 *
 *****************************************************/

/* Include Header Files */
#include "board.h"

/* FUNCTION */
void Systick_Delay_Init();
void SysTick_Handler (void);
void ms_delay(uint32_t dlyTicks);

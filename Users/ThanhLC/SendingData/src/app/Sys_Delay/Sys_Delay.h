/******************************************************
 *      Delay using Systick Timer
 *
 *****************************************************/
#include "board.h"


volatile uint32_t msTicks;

static void Systick_Delay_Init()
{
    /* Activate SysTick */
   SysTick_Config(SystemCoreClock/1000);

   /* Update priority set by SysTick_Configuration */
   NVIC_SetPriority(SysTick_IRQn, (1<<2) - 1);

   /* Enable system tick interrupt */
   NVIC_EnableIRQ(SysTick_IRQn);
}


void SysTick_Handler (void)
{
    msTicks ++;
}

void ms_delay(uint32_t dlyTicks)
{
    uint32_t curTicks;
    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks);
}

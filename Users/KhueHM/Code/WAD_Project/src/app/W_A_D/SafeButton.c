/*******************************************************
 *
 *      Safe Button Feature Function
 *
 ******************************************************/

/*
 * Include Header Files
 */
#include <stdio.h>
#include "board.h"
#include "gpio_pins.h"
#include "gpio_imx.h"
#include "debug_console_imx.h"
#include "Sys_Delay.h"
#include "Lora.h"

/*
 * Declare Global Variables
 */
// Define a small delay time (ms) to avoid button bounce
#define buttonDEBOUNCE 70

/*
 * MAIN Function

int main(void)
{
    hardware_init();

    Systick_Delay_Init();

    debug_printf("Start...!\r\n");

    bool IntStatus;

    while(1)
    {
        IntStatus = GPIO_IsIntPending(GPIO4_BASE_PTR, 8);
        //debug_printf("%d", IntStatus);

        if (IntStatus == 1)
        {
            debug_printf("ON\r\n");
            GPIO_WritePinOutput(GPIO6_BASE_PTR, 14, gpioPinClear);
        }
        else
        {
            GPIO_WritePinOutput(GPIO6_BASE_PTR, 14, gpioPinSet);
        }

        ms_delay(buttonDEBOUNCE);
    }
}
*/

/*
 * Press Button to send safe message to Host Server
 */
void SafeButton()
{
    bool ButtonStt;
    ButtonStt = GPIO_IsIntPending(GPIO4_BASE_PTR, 8);

    if (ButtonStt == 1)
    {
        // Send message
        Lora_Send("WC1_OK\r\n");
        GPIO_WritePinOutput(GPIO6_BASE_PTR, 14, gpioPinSet);
    }
    else
    {
        ;
    }
    // Button Debounce
    ms_delay(buttonDEBOUNCE);
}

/*
 * GPIO Button Handler
 */
void GPIO4_Combined_0_15_Handler()
{
}

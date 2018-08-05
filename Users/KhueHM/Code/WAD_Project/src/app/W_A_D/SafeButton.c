/*******************************************************
 *
 *
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
#include "rdc_semaphore.h"
#include "Sys_Delay.h"

/*
 * Declare Global Variables
 */
// Define a small delay time (ms) to avoid button bounce
#define buttonDEBOUNCE 70

/*
 * Button Initialization
 */
void GPIO_Ctrl_InitButton()
{
    gpio_init_config_t ButtonInitConfig = {
        .pin = 8,
        .direction = gpioDigitalInput,
        .interruptMode = gpioIntHighLevel
        };
    GPIO_Init(GPIO4_BASE_PTR, &ButtonInitConfig);
    /* Enable GPIO pin interrupt */
    GPIO_SetPinIntMode(GPIO4_BASE_PTR, 8, true);
    /* Clear the interrupt state */
    GPIO_ClearStatusFlag(GPIO4_BASE_PTR, 8);
}

/*
 * Buzzer Initialization
 */
void GPIO_Ctrl_InitBuzzer()
{
    gpio_init_config_t buzzerInitConfig = {
        .pin = 14,
        .direction = gpioDigitalOutput,
        .interruptMode = gpioNoIntmode
    };
    GPIO_Init(GPIO6_BASE_PTR, &buzzerInitConfig);
}


/*
 * MAIN Function
 */
int main(void)
{
    hardware_init();

    Systick_Delay_Init();

    GPIO_Ctrl_InitBuzzer();
    GPIO_Ctrl_InitButton();

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

/*
 * GPIO Button Handler
 */
void GPIO4_Combined_0_15_Handler()
{
}

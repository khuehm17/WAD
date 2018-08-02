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

/*
 * Declare Global Variables
 */
static volatile uint8_t button_pressed_flag;

/*
 * LED Initialization
 */
gpio_init_config_t ledInit= {
        .pin=6,
        .direction=gpioDigitalOutput,
        .interruptMode=gpioNoIntmode
        };

static void GPIO_Ctrl_InitLedPin()
{

    RDC_SEMAPHORE_Lock(rdcPdapGpio4);
    GPIO_Init(GPIO4,&ledInit);
    RDC_SEMAPHORE_Unlock(rdcPdapGpio4);
}

/*
 * Button Initialization
 */
void GPIO_Ctrl_InitButton()
{
    gpio_init_config_t ButtonInitConfig = {
        .pin = 8,
        .direction = gpioDigitalInput,
        .interruptMode = gpioIntHighLevel,
        };
    GPIO_Init(GPIO4_BASE_PTR, &ButtonInitConfig);
    NVIC_EnableIRQ(GPIO4_INT15_0_IRQn);
    /* Clear the interrupt state, this operation is necessary, because the GPIO module maybe confuse
    the first rising edge as interrupt*/
    GPIO_ClearStatusFlag(GPIO4_BASE_PTR, 8);
    /* Enable GPIO pin interrupt */
    GPIO_SetPinIntMode(GPIO4_BASE_PTR, 8, true);
}

/*
 * GPIO Wait Button Pressed
 */
void WaitButtonPressed()
{
    while (button_pressed_flag == 0);
    button_pressed_flag = 0;
}

/*
 * MAIN Function
 */
int main(void)
{
    hardware_init();
    GPIO_Ctrl_InitLedPin();
    GPIO_Ctrl_InitButton();
    while(1)
    {
        WaitButtonPressed();
        if (button_pressed_flag == 0)
        {
            RDC_SEMAPHORE_Lock(rdcPdapGpio4);
            GPIO_WritePinOutput(GPIO4,ledInit.pin,gpioPinClear);
            RDC_SEMAPHORE_Unlock(rdcPdapGpio4);
        }
    }
}

/*
 * GPIO Button Handler
 */
void GPIO4_Combined_0_15_Handler()
{
    button_pressed_flag = 1;
    RDC_SEMAPHORE_Lock(rdcPdapGpio4);
    GPIO_WritePinOutput(GPIO4,ledInit.pin,gpioPinSet);

    RDC_SEMAPHORE_Unlock(rdcPdapGpio4);
    /* clear the interrupt status */
    GPIO_ClearStatusFlag(GPIO4_BASE_PTR, 8);
}











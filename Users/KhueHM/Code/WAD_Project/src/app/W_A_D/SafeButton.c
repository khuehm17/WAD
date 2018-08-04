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
static bool on = false;
static uint8_t keyLastState;
static uint8_t keyState;
static volatile uint8_t button_pressed_flag;

/*
 * Button Initialization
 */
void GPIO_Ctrl_InitButton()
{
    gpio_init_config_t ButtonInitConfig = {
        .pin = 8,
        .direction = gpioDigitalInput,
        .interruptMode = gpioNoIntmode
        };
    GPIO_Init(GPIO4_BASE_PTR, &ButtonInitConfig);
    //NVIC_EnableIRQ(GPIO4_INT15_0_IRQn);
    /* Clear the interrupt state, this operation is necessary, because the GPIO module maybe confuse
    the first rising edge as interrupt*/
    //GPIO_ClearStatusFlag(GPIO4_BASE_PTR, 8);
    /* Enable GPIO pin interrupt */
    //GPIO_SetPinIntMode(GPIO4_BASE_PTR, 8, true);
}

/*
 * GPIO Wait Button Pressed
 */
 /*
void WaitButtonPressed()
{
    while (button_pressed_flag == 0);
    button_pressed_flag = 0;
}
*/

/*
 * GPIO Wait Button Pressed
 */
void WaitButtonPressed()
{
    while( keyState == keyLastState )
    {
        keyState = GPIO_ReadPinInput(GPIO4_BASE_PTR, 8);
    }
    keyLastState = keyState;
}

/*
 * MAIN Function
 */
int main(void)
{
    hardware_init();

    debug_printf("Init Button...!\r\n");

    GPIO_Ctrl_InitButton();

    debug_printf("Start...!\r\n");


    while(1)
    {
        WaitButtonPressed();
        PRINTF("%c ", on ? 'ON' : 'OFF');
        on = !on;
    }
}

/*
 * GPIO Button Handler
 */
/*
void GPIO4_Combined_0_15_Handler()
{
    button_pressed_flag = 1;

    debug_printf("ON\r\n");

    // clear the interrupt status
    GPIO_ClearStatusFlag(GPIO4_BASE_PTR, 8);
}
*/

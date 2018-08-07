/***********************************************************************
 *
 *      W-A-D
 *      Wheelchair Accident Detector Project Source File
 *      Demo Version - 08 / 2018
 *
 **********************************************************************/

/*
 * Include Header Files
 */
#include <stdio.h>
#include "FXOS8700CQ.h"
#include "FXAS21002C.h"
#include "Lora.h"
#include "I2C_RW_Register.h"
#include "Sys_Delay.h"
#include "debug_console_imx.h"
#include "gpio_pins.h"
#include "gpio_imx.h"

/*
 * Global variables and constants Declaration
 */

/*
 * MAIN Function
 */
int main (void)
{
    hardware_init();
    SystemInit();

    I2C_FXAS21002C_Init();
    I2C_FXOS8700CQ_Init();
    FXAS21002C_Init();
    FXOS8700CQ_Init();

    GPIO_WritePinOutput(GPIO6_BASE_PTR, 14, gpioPinSet);

    Systick_Delay_Init();
    //FXOS8700CQ_Accel_Calib();

    /* Start...! */
    /* Check FXOS8700CQ Connection */
    while (FXOS8700CQ_WhoAmI_Check() == 0)
    {
        ;
    }
    /* Check FXOS8700CQ Connection */
    while (FXAS21002C_WhoAmI_Check() == 0)
    {
        ;
    }

    while(1)
    {
        EventHandling();
        ms_delay(300);
    }
}

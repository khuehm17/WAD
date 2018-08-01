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
#include "AccidentAlert.c"

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

    UART_Lora_Init();
    Systick_Delay_Init();

    I2C_FXOS8700CQ_Init();
    I2C_FXAS21002C_Init();

    FXOS8700CQ_Init();
    FXAS21002C_Init();

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
        FXAS21002C_GetGyro();
        FXOS8700CQ_GetAccel();
        AccidentDetect();
        ms_delay(300);
    }
}





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
        AccidentDetect();
        ms_delay(300);
    }
}





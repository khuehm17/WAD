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
#include "FXOS8700CQ.h"
#include "FXAS21002C.h"
#include "Lora.h"
#include "I2C_RW_Register.h"
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

    UART_Lora_Init();

    I2C_FXOS8700CQ_Init();
    I2C_FXAS21002C_Init();

    FXOS8700CQ_Init();
    FXAS21002C_Init();

    //FXOS8700CQ_Accel_Calib();

    debug_printf("Start...!\r\n");
    debug_printf("Check WHO_AM_I Register of Sensor\r\n");
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

    debug_printf("Start Read Data From Sensor\r\n");

    while(1)
    {
        //Lora_Send("Hello World!\r\n");
        FXOS8700CQ_GetAccel();
        FXAS21002C_GetGyro();
        //ms_delay(200);
    }
}





/***********************************************************************
 *
 *      W-A-D
 *      Wheelchair-Accident-Detector Project MAIN File
 *      OSEK FreeRTOS Demo Version - 08 / 2018
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
#include "debug_console_imx.h"
#include "os.h"

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

    /* Start the OSEK */
    debug_printf("Starting OSEK-OS in AppMode1\r\n");

    StartOS(AppMode1);
    while(TRUE);

}

void ErrorHook(void)
{
    debug_printf("Kernel Panic\r\n");
	ShutdownOS(0);
}
/*
 * Sensor Initialization Task
 * FXOS8700CQ and FXAS21002C
 */
TASK (TaskInit)
{
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

    /* END Initialization Task */
    TerminateTask();
}

/*
 * Getting Value from Sensor Task
 */
TASK (TaskGetValue)
{
    while(1)
    {
        WaitEvent(evGetVal);
        ClearEvent(evGetVal);
        FXOS8700CQ_GetAccel();
        FXAS21002C_GetGyro();
    }
    /* END Get Sensor Value Task */
    TerminateTask();
}

/*
 * This is a periodic task.
 */
TASK(TaskPeriodic)
{
	SetEvent(TaskGetValue, evGetVal);
	/* END Periodic Task */
	TerminateTask();
}

TASK (TaskIDLE)
{
    while(1)
    {

    }
}

/***********************************************************************
 *   EOF
 **********************************************************************/

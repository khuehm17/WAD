/***********************************************************************
 *
 *      Wheelchair Accident Alert Function
 *
 **********************************************************************/

/*
 * Include Header Files
 */
#include "FXAS21002C.h"
#include "FXOS8700CQ.h"
#include "Lora.h"

/*
 * Define Wheelchair ID to Detect
 *  Wheelchair ID Form: ID-UserName-Room-Block
 */
const char wheelchairID[] = "CwtVinh";

/*
 * Accident Detect Function
 */
void AccidentDetect()
{
    struct Accel accel_Values;
    struct Gyro gyro_Values;

    gyro_Values = FXAS21002C_GetGyro();
    accel_Values = FXOS8700CQ_GetAccel();

    if ( (gyro_Values.g_X >= 2500) || (gyro_Values.g_Y >= 2500) || (accel_Values.a_X >= 2500) || (accel_Values.a_Y >= 2500))
    {
        Lora_Send(wheelchairID);
    }
}

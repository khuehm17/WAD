/***********************************************************************
 *
 *      FXAS21002C Source File
 *
 **********************************************************************/

/*
 * Include Header Files
 */
#include "FXAS21002C.h"



/*
 * Global variables and constants Declaration
 */
uint8_t GyroData[6];
int16_t Xout_Gyro_16_bit, Yout_Gyro_16_bit, Zout_Gyro_16_bit;
float Roll, Pitch, Yaw;
char Temp;



/*
 * FXAS21002C I2C Protocol Initialization
 */
void I2C_FXAS21002C_Init()
{
    /* Setup I2C initialization structure. */
    i2c_init_config_t i2cInitConfig = {
        .baudRate     = 400000u,
        .slaveAddress = FXAS21002C_I2C_ADDRESS
    };

    /* Initialize board specified hardware. */
    hardware_init();

    /* Get current module clock frequency. */
    i2cInitConfig.clockRate = get_i2c_clock_freq(I2C4_BASE_PTR);

    I2C_Init(I2C4_BASE_PTR, &i2cInitConfig);

    /* Finally, enable the I2C module */
    I2C_Enable(I2C4_BASE_PTR);
}

/*
 * Turn on FXAS21002C Standby Mode
 * For modifying Register
 */
void FXAS21002C_Standby()
{
    char temp = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1);
    I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1, temp &~ (0x03));
}

/*
 * Turn on FXAS21002C Ready Mode
 * To be in this mode to output data
 */
void FXAS21002C_Ready()
{
    char temp = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1);
    I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1, temp &~ (0x03));
    I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1, temp | (0x01));

}

/*
 * Turn on FXAS21002C Active Mode
 * To be in this mode to output data
 */
void FXAS21002C_Active()
{
    char temp = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1);
    I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1, temp &~ (0x03));
    I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1, temp | (0x02));

}

/*
 * FXAS21002C I2C WHO AM I Check
 */
int FXAS21002C_WhoAmI_Check()
{
    unsigned char Check_Val = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_WHO_AM_I_REG);
    if (FXAS21002C_WHO_AM_I_VAL == Check_Val)
    {
        PRINTF("FXAS21002C Ready to GO...!\n\r");
        PRINTF("FXAS21002C ID is %d\n\r", Check_Val);
        return 1;
    }
    else
    {
        PRINTF("FXAS21002C ID is INCORRECT...!\n\r");
        return 0;
    }
}

/*
 * FXAS21002C Initialization Function
 */
void FXAS21002C_Init()
{
    I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1, 0x40);	// Reset all registers to POR values
	//ms_delay(1);		// ~1ms delay
	I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG0, 0x1F);	// High-pass filter enabled, HPF cutoff frequency = 0.031Hz, +/-250 dps range -> 7.8125 mdps/LSB = 128 LSB/dps
	I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG2, 0x0C);	// Enable DRDY interrupt, mapped to INT1 - PTA5, push-pull, active low interrupt
	I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_CTRL_REG1, 0x16);	// ODR = 25Hz, Active mode
}


/*
 * Get FXAS21002C Accelerometer Value
 */
struct Gyro FXAS21002C_GetGyro()
{
    struct Gyro gyro_Values;

    GyroData[0] = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_OUT_X_MSB_REG);		// Read FXAS21002C data output registers 0x01-0x06
    GyroData[1] = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_OUT_X_LSB_REG);		// Read FXAS21002C data output registers 0x01-0x06
    GyroData[2] = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_OUT_Y_MSB_REG);		// Read FXAS21002C data output registers 0x01-0x06
    GyroData[3] = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_OUT_Y_LSB_REG);		// Read FXAS21002C data output registers 0x01-0x06
    GyroData[4] = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_OUT_Z_MSB_REG);		// Read FXAS21002C data output registers 0x01-0x06
    GyroData[5] = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_OUT_Z_LSB_REG);		// Read FXAS21002C data output registers 0x01-0x06
    // 16-bit gyro data
    Xout_Gyro_16_bit = (int16_t) (GyroData[0]<<8 | GyroData[1]);		// Compute 16-bit X-axis output value
    Yout_Gyro_16_bit = (int16_t) (GyroData[2]<<8 | GyroData[3]);		// Compute 16-bit Y-axis output value
    Zout_Gyro_16_bit = (int16_t) (GyroData[4]<<8 | GyroData[5]);		// Compute 16-bit Z-axis output value

    gyro_Values.g_X = Xout_Gyro_16_bit;
    gyro_Values.g_Y = Yout_Gyro_16_bit;
    gyro_Values.g_Z = Zout_Gyro_16_bit;

    // Gyro data converted to dps
    Roll = (float) (Xout_Gyro_16_bit) / SENSITIVITY_250;		// Compute X-axis output value in dps
    Pitch = (float) (Yout_Gyro_16_bit) / SENSITIVITY_250;		// Compute Y-axis output value in dps
    Yaw = (float) (Zout_Gyro_16_bit) / SENSITIVITY_250;			// Compute Z-axis output value in dps

    /* Print Gyro Values for Testing
    debug_printf("Gyro X %d\r\n", Xout_Gyro_16_bit);
    debug_printf("Gyro Y %d\r\n", Yout_Gyro_16_bit);
    debug_printf("Gyro Z %d\r\n", Zout_Gyro_16_bit);
    */
    return gyro_Values;
    //debug_printf("Gyro: R: %f  P: %f  Y: %f\n\r", Roll, Pitch, Yaw);
    // Temperature data
    //Temp = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, FXAS21002C_TEMP_REG);
}

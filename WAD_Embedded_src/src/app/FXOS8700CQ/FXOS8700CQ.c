/***********************************************************************
 *
 *      FXOS8700CQ Source File
 *
 **********************************************************************/

/*
 * Include Header Files
 */
#include "FXOS8700CQ.h"


/*
 * Global variables and constants Declaration
 */
/* Accelerometer variables */
uint8_t AccelData[6];
//int16_t Xout_g, Yout_g, Zout_g;

unsigned char MagData[6];
//float Xout_uT, Yout_uT, Zout_uT;

#define PI					  3.14159

/*
 * FXOS8700CQ I2C Protocol Initialization
 */
void I2C_FXOS8700CQ_Init()
{
    /* Setup I2C initialization structure. */
    i2c_init_config_t i2cInitConfig = {
        .baudRate     = 400000u,
        .slaveAddress = FXOS8700CQ_I2C_ADDRESS
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
 * Turn on FXOS8700CQ Standby Mode
 * For modifying Register
 */
void FXOS8700CQ_Standby()
{
    char temp = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG1);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG1, temp &~ (0x01));
}

/*
 * Turn on FXOS8700CQ Active Mode
 * To be in this mode to output data
 */
void FXOS8700CQ_Active()
{
    char temp = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG1);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG1, temp | (0x01));
}

/*
 * Reset FXOS8700CQ
 */
void FXOS8700CQ_Reset()
{
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG2, 0x40);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_M_CTRL_REG2, 0x40);
}

/*
 * FXOS8700CQ I2C WHO AM I Check
 */
int FXOS8700CQ_WhoAmI_Check()
{
    unsigned char Check_Val = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_WHO_AM_I_REG);
    if (FXOS8700CQ_WHO_AM_I_VAL == Check_Val)
    {
        PRINTF("FXOS8700CQ Ready to GO...!\n\r");
        PRINTF("FXOS8700CQ ID is %d\n\r", Check_Val);
        return 1;
    }
    else
    {
        PRINTF("FXOS8700CQ ID is INCORRECT...!\n\r");
        return 0;
    }
}

/*
 * FXOS8700CQ Initialization Function
 */
void FXOS8700CQ_Init()
{
    /*
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG2, 0x40);
    FXOS8700CQ_Standby();
    //ms_delay(1);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_XYZ_DATA_CFG_REG, 0x00);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_M_CTRL_REG1, 0x1F);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_M_CTRL_REG2, 0x20);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG2, 0x02);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG3, 0x00);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG4, 0x01);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG5, 0x01);
    //I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG1, 0x35);
    FXOS8700CQ_Active();
    */
    FXOS8700CQ_Reset();
    ms_delay(1);
    FXOS8700CQ_Standby();            // Standby Mode
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_F_SETUP_REG, 0x01);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_M_CTRL_REG1, 0x9F);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_M_CTRL_REG2, 0x20);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_XYZ_DATA_CFG_REG, 0x00);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG1, 0x1D);

    FXOS8700CQ_Active();            // Active Mode

}

/*
 * FXOS8700CQ Accelerometer Value Calibration
 */
void FXOS8700CQ_Accel_Calib()
{
    struct Accel accel_Values;
    char X_Accel_offset, Y_Accel_offset, Z_Accel_offset;

    FXOS8700CQ_Active();

    AccelData[0] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_X_MSB_REG);
    AccelData[1] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_X_LSB_REG);
    AccelData[2] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_Y_MSB_REG);
    AccelData[3] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_Y_LSB_REG);
    AccelData[4] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_Z_MSB_REG);
    AccelData[5] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_Z_LSB_REG);

    accel_Values.a_X = ((int16_t) (AccelData[0]<<8 | AccelData[1])) >> 2;		// Compute 14-bit X-axis acceleration output value
    accel_Values.a_Y = ((int16_t) (AccelData[2]<<8 | AccelData[3])) >> 2;		// Compute 14-bit Y-axis acceleration output value
    accel_Values.a_Z = ((int16_t) (AccelData[4]<<8 | AccelData[5])) >> 2;		// Compute 14-bit Z-axis acceleration output value

    X_Accel_offset = accel_Values.a_X / 8 * (-1);		// Compute X-axis offset correction value
    Y_Accel_offset = accel_Values.a_Y / 8 * (-1);		// Compute Y-axis offset correction value
    Z_Accel_offset = (accel_Values.a_Z - SENSITIVITY_2G) / 8 * (-1);		// Compute Z-axis offset correction value

    FXOS8700CQ_Standby();
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OFF_X_REG, X_Accel_offset);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OFF_Y_REG, Y_Accel_offset);
    I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OFF_Z_REG, Z_Accel_offset);
    FXOS8700CQ_Active();
}

/*
 * FXOS8700CQ Magnetometer Value Calibration
 */
void FXOS8700CQ_Magnet_Calib()
{
    struct Mag mag_Values;
    short Xout_Mag_16_bit_avg, Yout_Mag_16_bit_avg, Zout_Mag_16_bit_avg;
	short Xout_Mag_16_bit_max, Yout_Mag_16_bit_max, Zout_Mag_16_bit_max;
	short Xout_Mag_16_bit_min, Yout_Mag_16_bit_min, Zout_Mag_16_bit_min;
	char i = 0;

	while (i < 150)		// This will take ~30s (94 samples * 1/3.125)
	{
        MagData[0] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_X_MSB_REG);		// Read data output registers 0x33 - 0x38
        MagData[1] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_X_LSB_REG);		// Read data output registers 0x33 - 0x38
        MagData[2] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_Y_MSB_REG);		// Read data output registers 0x33 - 0x38
        MagData[3] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_Y_LSB_REG);		// Read data output registers 0x33 - 0x38
        MagData[4] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_Z_MSB_REG);		// Read data output registers 0x33 - 0x38
        MagData[5] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_Z_LSB_REG);		// Read data output registers 0x33 - 0x38

        mag_Values.m_X = (short) (MagData[0]<<8 | MagData[1]);		// Compute 16-bit X-axis magnetic output value
        mag_Values.m_Y = (short) (MagData[2]<<8 | MagData[3]);		// Compute 16-bit Y-axis magnetic output value
        mag_Values.m_Z = (short) (MagData[4]<<8 | MagData[5]);		// Compute 16-bit Z-axis magnetic output value

        if (i == 0)
            {
                Xout_Mag_16_bit_max = mag_Values.m_X;
                Xout_Mag_16_bit_min = mag_Values.m_X;

                Yout_Mag_16_bit_max = mag_Values.m_Y;
                Yout_Mag_16_bit_min = mag_Values.m_Y;

                Zout_Mag_16_bit_max = mag_Values.m_Z;
                Zout_Mag_16_bit_min = mag_Values.m_Z;
            }

        // Check to see if current sample is the maximum or minimum X-axis value
        if (mag_Values.m_X > Xout_Mag_16_bit_max)	{Xout_Mag_16_bit_max = mag_Values.m_X;}
        if (mag_Values.m_X < Xout_Mag_16_bit_min)	{Xout_Mag_16_bit_min = mag_Values.m_X;}

        // Check to see if current sample is the maximum or minimum Y-axis value
        if (mag_Values.m_Y > Yout_Mag_16_bit_max)	{Yout_Mag_16_bit_max = mag_Values.m_Y;}
        if (mag_Values.m_Y < Yout_Mag_16_bit_min)	{Yout_Mag_16_bit_min = mag_Values.m_Y;}

        // Check to see if current sample is the maximum or minimum Z-axis value
        if (mag_Values.m_Z > Zout_Mag_16_bit_max)	{Zout_Mag_16_bit_max = mag_Values.m_Z;}
        if (mag_Values.m_Z < Zout_Mag_16_bit_min)	{Zout_Mag_16_bit_min = mag_Values.m_Z;}

        i++;

	}

	Xout_Mag_16_bit_avg = (Xout_Mag_16_bit_max + Xout_Mag_16_bit_min) / 2;		// X-axis hard-iron offset
	Yout_Mag_16_bit_avg = (Yout_Mag_16_bit_max + Yout_Mag_16_bit_min) / 2;		// Y-axis hard-iron offset
	Zout_Mag_16_bit_avg = (Zout_Mag_16_bit_max + Zout_Mag_16_bit_min) / 2;		// Z-axis hard-iron offset

	// Left-shift by one as magnetometer offset registers are 15-bit only, left justified
	Xout_Mag_16_bit_avg <<= 1;
	Yout_Mag_16_bit_avg <<= 1;
	Zout_Mag_16_bit_avg <<= 1;

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG1, 0x00);		// Standby mode to allow writing to the offset registers

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOFF_X_LSB_REG, (char) (Xout_Mag_16_bit_avg & 0xFF));
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOFF_X_MSB_REG, (char) ((Xout_Mag_16_bit_avg >> 8) & 0xFF));
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOFF_Y_LSB_REG, (char) (Yout_Mag_16_bit_avg & 0xFF));
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOFF_Y_MSB_REG, (char) ((Yout_Mag_16_bit_avg >> 8) & 0xFF));
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOFF_Z_LSB_REG, (char) (Zout_Mag_16_bit_avg & 0xFF));
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOFF_Z_MSB_REG, (char) ((Zout_Mag_16_bit_avg >> 8) & 0xFF));

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_CTRL_REG1, 0x35);		// Active mode again
}


/*
 * FXOS8700CQ Get Accelerometer Data
 */
struct Accel FXOS8700CQ_GetAccel()
{
    struct Accel accel_Values;

    AccelData[0] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_X_MSB_REG);		// Read data output registers 0x01-0x06
    AccelData[1] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_X_LSB_REG);		// Read data output registers 0x01-0x06
    AccelData[2] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_Y_MSB_REG);		// Read data output registers 0x01-0x06
    AccelData[3] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_Y_LSB_REG);		// Read data output registers 0x01-0x06
    AccelData[4] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_Z_MSB_REG);		// Read data output registers 0x01-0x06
    AccelData[5] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_OUT_Z_LSB_REG);		// Read data output registers 0x01-0x06

    // 14-bit accelerometer data
    accel_Values.a_X = ((int16_t) (AccelData[0]<<8 | AccelData[1])) >> 2;		// Compute 14-bit X-axis acceleration output value
    accel_Values.a_Y = ((int16_t) (AccelData[2]<<8 | AccelData[3])) >> 2;		// Compute 14-bit Y-axis acceleration output value
    accel_Values.a_Z = ((int16_t) (AccelData[4]<<8 | AccelData[5])) >> 2;		// Compute 14-bit Z-axis acceleration output value

    /* Accelerometer data converted to g's
    Xout_g =  (Xout_Accel_14_bit / SENSITIVITY_2G);		// Compute X-axis output value in g's
    Yout_g =  (Yout_Accel_14_bit / SENSITIVITY_2G);		// Compute Y-axis output value in g's
    Zout_g =  (Zout_Accel_14_bit / SENSITIVITY_2G);		// Compute Z-axis output value in g's
    */
    return accel_Values;

    //debug_printf("Accel X %d\r\n", Xout_Accel_14_bit);
    //debug_printf("Accel Y %d\r\n", Yout_Accel_14_bit);
    //debug_printf("Accel Z %d\r\n", Zout_Accel_14_bit);
    //debug_printf("%d  %d  %d      ", Xout_Accel_14_bit, Yout_Accel_14_bit, Zout_Accel_14_bit);
}

/*
 * FXOS8700CQ Get Magnetometer Data
 */
struct Mag FXOS8700CQ_GetMag()
{
    struct Mag mag_Values;
    //float Heading;
    MagData[0] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_X_MSB_REG);		// Read data output registers 0x33 - 0x38
    MagData[1] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_X_LSB_REG);		// Read data output registers 0x33 - 0x38
    MagData[2] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_Y_MSB_REG);		// Read data output registers 0x33 - 0x38
    MagData[3] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_Y_LSB_REG);		// Read data output registers 0x33 - 0x38
    MagData[4] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_Z_MSB_REG);		// Read data output registers 0x33 - 0x38
    MagData[5] = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, FXOS8700CQ_MOUT_Z_LSB_REG);		// Read data output registers 0x33 - 0x38

    // 16-bit magnetometer data
    mag_Values.m_X = (short) (MagData[0]<<8 | MagData[1]);		// Compute 16-bit X-axis magnetic output value
    mag_Values.m_Y = (short) (MagData[2]<<8 | MagData[3]);		// Compute 16-bit Y-axis magnetic output value
    mag_Values.m_Z = (short) (MagData[4]<<8 | MagData[5]);		// Compute 16-bit Z-axis magnetic output value

    return mag_Values;
    /* Magnetometer data converted to microteslas
    Xout_uT = (float) (Xout_Mag_16_bit) / SENSITIVITY_MAG;		// Compute X-axis output magnetic value in uT
    Yout_uT = (float) (Yout_Mag_16_bit) / SENSITIVITY_MAG;		// Compute Y-axis output magnetic value in uT
    Zout_uT = (float) (Zout_Mag_16_bit) / SENSITIVITY_MAG;		// Compute Z-axis output magnetic value in uT
    */
    //Heading = atan2 (Yout_uT, Xout_uT) * 180 / PI;		// Compute Yaw angle
}

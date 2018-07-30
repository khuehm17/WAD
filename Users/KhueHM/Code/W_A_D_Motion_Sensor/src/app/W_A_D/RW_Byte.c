#include "RW_Byte_Init.h"
#include "Sys_Delay.h"

/* Declare Global Variable */
static uint8_t txBuffer[5];
uint8_t rxBuffer[13];
static uint8_t cmdBuffer[5];


/* I2C Communication Initialization */
void Init_I2C_Sensor()
{
    /* Setup I2C initialization structure. */
    i2c_init_config_t i2cInitConfig = {
        .baudRate     = 400000u,
        .slaveAddress = BOARD_I2C_FXOS8700CQ_ADDR
    };

    /* Initialize board specified hardware. */
    hardware_init();

    /* Get current module clock frequency. */
    i2cInitConfig.clockRate = get_i2c_clock_freq(I2C4_BASE);

    I2C_Init(I2C4_BASE, &i2cInitConfig);

    /* Finally, enable the I2C module */
    I2C_Enable(I2C4_BASE);
}

/* Initialize FXOS8700CQ */
int Init_FXOS8700CQ()
{
    // Set FXOS8700CQ to Standby Mode
    Write_BYTE(0x00, FXOS8700CQ_CTRL_REG1);
    // Set Magnetometer Control Register 1
    Write_BYTE(0x1F, FXOS8700CQ_M_CTRL_REG1);
    // Set Magnetometer Control Register 2
    Write_BYTE(0x20, FXOS8700CQ_M_CTRL_REG2);
    // Set Data configuration Register
    Write_BYTE(0x01, FXOS8700CQ_XYZ_DATA_CFG);
    // Set Accelerometer Control Register 1
    Write_BYTE(0x0D, FXOS8700CQ_CTRL_REG1);
    // WHO_AM_I Check
    Read_BYTE(FXOS8700CQ_WHO_AM_I, 1);
    if (FXOS8700CQ_WHO_AM_I_VAL == rxBuffer[0])
    {
        PRINTF("Ready to GO...!\n\r");
        PRINTF("FXOS8700CQ ID is %d\n\r", rxBuffer[0]);
        return 1;
    }

    else
    {
        PRINTF("ERROR CONNECTION...!\n\r");
        return 0;
    }

}

/* Send Data through I2C Function */
static bool I2C_MasterSendDataPolling(I2C_Type *base,
                                      const uint8_t *cmdBuff,
                                      uint32_t cmdSize,
                                      const uint8_t *txBuff,
                                      uint32_t txSize)
{
    if (I2C_GetStatusFlag(base, i2cStatusBusBusy))
        return false;

    /* Set I2C work under Tx mode */
    I2C_SetDirMode(base, i2cDirectionTransmit);

    /* Switch to Master Mode and Send Start Signal. */
    I2C_SetWorkMode(base, i2cModeMaster);

    /* Send first byte */
    if (0 != cmdSize)
    {
        I2C_WriteByte(base, *cmdBuff++);
        cmdSize--;
    }
    else
    {
        I2C_WriteByte(base, *txBuff++);
        txSize--;
    }

    while (1)
    {
        /* Wait I2C transmission status flag assert.  */
        while (!I2C_GetStatusFlag(base, i2cStatusInterrupt));

        /* Clear I2C transmission status flag. */
        I2C_ClearStatusFlag(base, i2cStatusInterrupt);

        /* Transmit complete. */
        if ((I2C_GetStatusFlag(base, i2cStatusReceivedAck)) ||
            ((0 == txSize) && (0 == cmdSize)))
        {
            /* Switch to Slave mode and Generate a Stop Signal. */
            I2C_SetWorkMode(base, i2cModeSlave);

            /* Switch back to Rx direction. */
            I2C_SetDirMode(base, i2cDirectionReceive);
            return true;
        }
        else
        {
            if (0 != cmdSize)
            {
                I2C_WriteByte(base, *cmdBuff++);
                cmdSize--;
            }
            else
            {
                I2C_WriteByte(base, *txBuff++);
                txSize--;
            }
        }
    }
}


/* Receive Data through I2C Function */
static bool I2C_MasterReceiveDataPolling(I2C_Type *base,
                                         const uint8_t *cmdBuff,
                                         uint32_t cmdSize,
                                         uint8_t *rxBuff,
                                         uint32_t rxSize)
{
    uint32_t currentDir = i2cDirectionReceive;

    /* Clear I2C interrupt flag to avoid spurious interrupt */
    I2C_ClearStatusFlag(base, i2cStatusInterrupt);

    if (I2C_GetStatusFlag(base, i2cStatusBusBusy))
    {
        return false;
    }

    /* Set I2C work under Tx mode */
    I2C_SetDirMode(base, i2cDirectionTransmit);

    /* Switch to Master Mode and Send Start Signal. */
    I2C_SetWorkMode(base, i2cModeMaster);

    if (0 != cmdSize)
    {
        currentDir = i2cDirectionTransmit;
        if (1 == cmdSize)
            I2C_SendRepeatStart(base);
        I2C_WriteByte(base, *cmdBuff++);
        cmdSize--;
    }
    else
    {
        /* Change to receive state. */
        I2C_SetDirMode(base, i2cDirectionReceive);

        if (1 == rxSize)
            /* Send Nack */
            I2C_SetAckBit(base, false);
        else
            /* Send Ack */
            I2C_SetAckBit(base, true);
        /* dummy read to clock in 1st byte */
        *rxBuff = I2C_ReadByte(base);
    }

    while (1)
    {
        /* Wait I2C transmission status flag assert.  */
        while (!I2C_GetStatusFlag(base, i2cStatusInterrupt));

        /* Clear I2C transmission status flag. */
        I2C_ClearStatusFlag(base, i2cStatusInterrupt);

        if (i2cDirectionTransmit == currentDir)
        {
            if (0 < cmdSize)
            {
                if (I2C_GetStatusFlag(base, i2cStatusReceivedAck))
                {
                    /* Switch to Slave mode and Generate a Stop Signal. */
                    I2C_SetWorkMode(base, i2cModeSlave);

                    /* Switch back to Rx direction. */
                    I2C_SetDirMode(base, i2cDirectionReceive);
                    return false;
                }
                else
                {
                    if (1 == cmdSize)
                        I2C_SendRepeatStart(base);
                    I2C_WriteByte(base, *cmdBuff++);
                    cmdSize--;
                }
            }
            else
            {
                /* Change to receive state. */
                I2C_SetDirMode(base, i2cDirectionReceive);
                currentDir = i2cDirectionReceive;

                if (1 == rxSize)
                    /* Send Nack */
                    I2C_SetAckBit(base, false);
                else
                    /* Send Ack */
                    I2C_SetAckBit(base, true);
                /* dummy read to clock in 1st byte */
                *rxBuff = I2C_ReadByte(base);
            }
        }
        else
        {
            /* Normal read operation. */
            if (2 == rxSize)
                /* Send Nack */
                I2C_SetAckBit(base, false);
            else
                /* Send Nack */
                I2C_SetAckBit(base, true);

            if (1 == rxSize)
                /* Switch back to Tx direction to avoid additional I2C bus read. */
                I2C_SetDirMode(base, i2cDirectionTransmit);
            *rxBuff = I2C_ReadByte(base);
            rxBuff++;
            rxSize--;

            /* receive finished. */
            if (0 == rxSize)
            {
                /* Switch to Slave mode and Generate a Stop Signal. */
                I2C_SetWorkMode(base, i2cModeSlave);

                /* Switch back to Rx direction. */
                I2C_SetDirMode(base, i2cDirectionReceive);

                return true;
            }
        }
    }
}


/* Write a Byte to a Register */
void Write_BYTE(char byte, char reg)
{
    cmdBuffer[0] = BOARD_I2C_FXOS8700CQ_ADDR << 1;
    cmdBuffer[1] = reg;
    txBuffer[0]  = byte;
    I2C_MasterSendDataPolling(I2C4_BASE, cmdBuffer, 2, txBuffer, 1);
}

/* Read a Byte to a Register */
void Read_BYTE(char reg, uint32_t dataLength)
{
    cmdBuffer[0] = BOARD_I2C_FXOS8700CQ_ADDR << 1;
    cmdBuffer[1] = reg;
    cmdBuffer[2] = (BOARD_I2C_FXOS8700CQ_ADDR << 1) + 1;
    I2C_MasterReceiveDataPolling(I2C4_BASE, cmdBuffer, 3, rxBuffer, dataLength);
}

/* Get value from FXOS8700CQ Sensor */
void FXOS8700CQ_GetValues(void)
{
    I2C_Enable(I2C4_BASE);
    Read_BYTE(FXOS8700CQ_STATUS, FXOS8700CQ_READ_LEN);
    //I2C_MasterReceiveDataPolling(BOARD_I2C_BASEADDR, cmdBuffer, 3, rxBuffer, 6);
    // copy the 14 bit accelerometer byte data into 16 bit words
    int acc_x = (int16_t)(((rxBuffer[1] << 8) | rxBuffer[2]))>> 2;
    int acc_y = (int16_t)(((rxBuffer[3] << 8) | rxBuffer[4]))>> 2;
    int acc_z = (int16_t)(((rxBuffer[5] << 8) | rxBuffer[6]))>> 2;

    // copy the magnetometer byte data into 16 bit words
    //int mag_x = (int16_t) (rxBuffer[7] << 8) | rxBuffer[8];
    //int mag_y = (int16_t) (rxBuffer[9] << 8) | rxBuffer[10];
    //int mag_z = (int16_t) (rxBuffer[11] << 8) | rxBuffer[12];

    //struct Data values;  // structure to hold values

    //debug_printf("2G MODE: X=%f Y=%f Z=%f\n\r",acc_x, acc_y, acc_z);
    PRINTF("ACC Value: X=%d Y=%d Z=%d\n\r",acc_x, acc_y, acc_z);
    // 0.488 mg/LSB in 4 g mode (8.1 data sheet)
    //values.ax = 0.488e-3*acc_x;
    //values.ay = 0.488e-3*acc_y;
    //values.az = 0.488e-3*acc_z;

    // the magnetometer sensitivity is fixed at 0.1 μT/LSB
    //values.mx = 0.1e-6*mag_x;
    //values.my = 0.1e-6*mag_y;
    //values.mz = 0.1e-6*mag_z;

    //PRINTF("X=%6.3fg Y=%6.3fg Z=%6.3fg\n\r",values.ax, values.ay, values.az);
    //return values;
    I2C_Disable(I2C4_BASE);
}


int main(void)
{
    // INIT
    SystemInit();
    Systick_Delay_Init();
    Init_I2C_Sensor();
    debug_printf("Start...!\n\r");
    debug_printf("Check WHO_AM_I Register of Sensor\n\r");
    //Init_FXOS8700CQ();
    // I2C will start reading Data from FXOS8700CQ until I2C connection is OK!

    while(Init_FXOS8700CQ() == 0)
    {
        ;
    }
    //struct Data values;

    debug_printf("Start Read Data From Sensor\n\r");


    //debug_printf("Finish Demo!");

    while(1)
    {
        FXOS8700CQ_GetValues();
        ms_delay(500);

        // print each struct member over serial
        /*debug_printf("ax = %f ay = %f az = %f | mx = %f my = %f mz = %f\n"
              ,values.ax, values.ay, values.az
              ,values.mx, values.my, values.mz);  */

    }


}



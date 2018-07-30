/***********************************************************************
 *
 *      I2C Protocol Read & Write Register Source File
 *          * Base I2C 4 *
 *
 **********************************************************************/

/*
 * Include Header Files
 */
#include "I2C_RW_Register.h"

/*
 * Declare Global Variable
 */
static uint8_t txBuffer[5];
static uint8_t rxBuffer[13];
static uint8_t cmdBuffer[5];

/*
 * Write a Byte to Register Function
 */
void I2C_WriteRegister(unsigned char SlaveAddress, uint8_t RegisterAddress, char Data)
{
    cmdBuffer[0] = SlaveAddress << 1;
    cmdBuffer[1] = RegisterAddress;
    txBuffer[0]  = Data;
    I2C_MasterSendDataPolling(I2C4_BASE, cmdBuffer, 2, txBuffer, 1);
}

/*
 * Read a Byte from a Register Function
 */
unsigned char I2C_ReadRegister(unsigned char SlaveAddress, uint8_t RegisterAddress)
{
    unsigned char result;
    cmdBuffer[0] = SlaveAddress << 1;
    cmdBuffer[1] = RegisterAddress;
    cmdBuffer[2] = (SlaveAddress << 1) + 1;
    I2C_MasterReceiveDataPolling(I2C4_BASE, cmdBuffer, 3, rxBuffer, 1);
    result = rxBuffer[0];
    return result;
}

/*
 * Read Bytes from Multi Register Function
 * !! Notice: Have some bugs with this Function
 */
void I2C_ReadMultiRegisters(unsigned char SlaveAddress, uint8_t RegisterAddress, uint32_t DataLength, uint8_t Data)
{
    cmdBuffer[0] = SlaveAddress << 1;
    cmdBuffer[1] = RegisterAddress;
    cmdBuffer[2] = (SlaveAddress << 1) + 1;
    I2C_MasterReceiveDataPolling(I2C4_BASE, cmdBuffer, 3, Data, DataLength);
}

/*
 * Send Data through I2C Function
 */
bool I2C_MasterSendDataPolling(I2C_Type *base,
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


/*
 * Receive Data through I2C Function
 */
bool I2C_MasterReceiveDataPolling(I2C_Type *base,
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


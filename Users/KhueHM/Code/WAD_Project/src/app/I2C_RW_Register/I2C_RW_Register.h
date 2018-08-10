/***********************************************************************
 *
 *      I2C Protocol Read & Write Register Header File
 *          * Base I2C 4 *
 *
 **********************************************************************/

#ifndef _I2C_RW_REGISTER_H_
#define _I2C_RW_REGISTER_H_

/* Include Header Files */
#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "i2c_imx.h"

/* FUNCTION */
void I2C_WriteRegister(unsigned char SlaveAddress, uint8_t RegisterAddress, char Data);
unsigned char I2C_ReadRegister(unsigned char SlaveAddress, uint8_t RegisterAddress);
//void I2C_ReadMultiRegister(unsigned char SlaveAddress, uint8_t RegisterAddress, uint32_t DataLength, uint8_t Data);
bool I2C_MasterSendDataPolling(I2C_Type *base,
                                      const uint8_t *cmdBuff,
                                      uint32_t cmdSize,
                                      const uint8_t *txBuff,
                                      uint32_t txSize);

bool I2C_MasterReceiveDataPolling(I2C_Type *base,
                                         const uint8_t *cmdBuff,
                                         uint32_t cmdSize,
                                         uint8_t *rxBuff,
                                         uint32_t rxSize);

#endif /* _I2C_RW_REGISTER_H_ */

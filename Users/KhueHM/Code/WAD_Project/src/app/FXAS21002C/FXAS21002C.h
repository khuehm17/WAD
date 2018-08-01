/***********************************************************************
 *
 *      FXAS21002C Header File
 *
 **********************************************************************/

#ifndef _FXAS21102C_H_
#define _FXAS21102C_H_

/* Include Header Files */
#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "debug_console_imx.h"
#include "i2c_imx.h"
#include "Sys_Delay.h"
#include <math.h>
#include "I2C_RW_Register.h"

/* Declare Gyroscope Data Structure */
struct Gyro {
    int16_t g_X;
    int16_t g_Y;
    int16_t g_Z;
    };

/* FXAS21002C Registers */
#define FXAS21002C_STATUS_REG            0x00		// Alias for DR_STATUS or F_STATUS

#define FXAS21002C_OUT_X_MSB_REG         0x01		// [7:0] are 8 MSBs of the 14-bit X-axis sample
#define FXAS21002C_OUT_X_LSB_REG         0x02		// [7:2] are the 6 LSB of 14-bit X-axis sample (1),(2)
#define FXAS21002C_OUT_Y_MSB_REG         0x03		// [7:0] are 8 MSBs of the 14-bit Y-axis sample
#define FXAS21002C_OUT_Y_LSB_REG         0x04		// [7:2] are the 6 LSB of 14-bit Y-axis sample (1),(2)
#define FXAS21002C_OUT_Z_MSB_REG         0x05		// [7:0] are 8 MSBs of the 14-bit Z-axis sample
#define FXAS21002C_OUT_Z_LSB_REG         0x06		// [7:2] are the 6 LSB of 14-bit Z-axis sample (1),(2),(3)

#define FXAS21002C_DR_STATUS_REG		 0x07		// Data-ready status information
#define FXAS21002C_F_STATUS_REG		     0x08		// FIFO status
#define FXAS21002C_F_SETUP_REG           0x09    	// F_SETUP FIFO Setup Register
#define FXAS21002C_F_EVENT_REG	         0x0A    	// FIFO event
#define FXAS21002C_INT_SCR_FLAG_REG      0x0B    	// Interrupt event source status flags

#define FXAS21002C_WHO_AM_I_REG          0x0C    	// WHO_AM_I Device ID Register

#define FXAS21002C_CTRL_REG0			 0x0D		// Control register 0: Full-scale range selection, high-pass filter control, SPI mode selection
#define FXAS21002C_RT_CFG			     0x0E		// Rate threshold function configuration
#define FXAS21002C_RT_SRC			     0x0F		// Rate threshold event flags status register
#define FXAS21002C_RT_THS			     0x10		// Rate threshold function threshold value register
#define FXAS21002C_RT_COUNT			     0x11		// Rate threshold function debounce counter

#define FXAS21002C_TEMP_REG              0x12		// Device temperature, valid range of -128 to 127 °C, with a scaling of 1 °C/LSB.

#define FXAS21002C_CTRL_REG1             0x13    	// Control register 1: Operating mode, ODR selection, self-test and reset control
#define FXAS21002C_CTRL_REG2             0x14    	// Control register 2: Interrupt configuration settings
#define FXAS21002C_CTRL_REG3             0x15    	// Control register 2: Interrupt configuration settings

/* FXAS21002C WHO AM I Value */
#define FXAS21002C_WHO_AM_I_VAL          0xD7

/* FXAS21002C 7-bit I2C address */
#define FXAS21002C_I2C_ADDRESS 	         0x20		// SA0 = 0

/* FXAS21002C Sensitivity */
#define SENSITIVITY_2000		         16
#define SENSITIVITY_1000		         32
#define SENSITIVITY_500		  	         64
#define SENSITIVITY_250		  	         128

/* FUNCTION */
void I2C_FXAS21002C_Init();
void FXAS21002C_Init();

void FXAS21002C_Standby();
void FXAS21002C_Ready();
void FXAS21002C_Active();

int FXAS21002C_WhoAmI_Check();

struct Gyro FXAS21002C_GetGyro();

#endif /* _FXAS21102C_H_ */

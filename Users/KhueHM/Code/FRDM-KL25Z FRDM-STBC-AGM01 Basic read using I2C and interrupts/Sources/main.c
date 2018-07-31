/******************************************************************************
* Freescale Semiconductor Inc.
* (c) Copyright 2015 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*******************************************************************************
Services performed by FREESCALE in this matter are performed AS IS and without
any warranty. CUSTOMER retains the final decision relative to the total design
and functionality of the end product. FREESCALE neither guarantees nor will be
held liable by CUSTOMER for the success of this project.
FREESCALE DISCLAIMS ALL WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY INCLUDING,
BUT NOT LIMITED TO, IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR
A PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE ORE ADVISE SUPPLIED TO THE 
PROJECT BY FREESCALE, AND OR NAY PRODUCT RESULTING FROM FREESCALE SERVICES.
IN NO EVENT SHALL FREESCALE BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THIS AGREEMENT.
CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands
or actions by anyone on account of any damage, or injury, whether commercial,
contractual, or tortuous, rising directly or indirectly as a result of the 
advise or assistance supplied CUSTOMER in connection with product, services
or goods supplied under this Agreement.
*******************************************************************************
* File: main.c
* Owner: b12257
* Version: 1.0
* Date: June-18-2015
* Classification General Business Information
* FXOS8700CQ+FXAS21002C example code with use of FRDM-KL25Z and 
* FRDM-STBC-AGM01 boards
*******************************************************************************
* This code has been written for the Freescale FRDM-KL25Z + FRDM-STBC-AGM01 
* boards and demonstrates how to read both the acceleration and magnetic data 
* from the FXOS8700CQ and angular rate from the FXAS21002C gyroscope using an
* interrupt technique. 
* 
* Both sensors are controlled via I2C by default.
*
* I2C slave addresses:
* FXOS8700 -> 0x1E
* FXAS21002 -> 0x20
*
* J5 selects the 21002 reset:
*   2:3 -> uses MCU reset
*
* J6 selects MCU I2C bus for SCL1_SCLK:
*   2:3 -> I2C_SCL1 (PTC1)
*
* J7 selects MCU I2C bus for SDA1_MOSI:
*    2:3 -> I2C_SDA1 (PTC2)
*
* INT1_8700 connected to PTD4 pin, INT1_21002 connected to PTA5 pin 
*******************************************************************************
Revision History:
Version		Date	 		Author		Description of Changes
1.0			June-18-2015	b12257		
******************************************************************************/

#include "derivative.h" 				// Include peripheral declarations 
#include "I2C.h"						// Include declarations of I2C communication routines 
#include "FXOS8700CQ.h"					// Include declarations of FXOS8700CQ registers 
#include "FXAS21002C.h"					// Include declarations of FXAS21002C registers 

/******************************************************************************
* Global variables and constants
******************************************************************************/

unsigned char AccelMagData[12];
short Xout_Accel_14_bit, Yout_Accel_14_bit, Zout_Accel_14_bit;
float Xout_g, Yout_g, Zout_g;
short Xout_Mag_16_bit, Yout_Mag_16_bit, Zout_Mag_16_bit;
float Xout_uT, Yout_uT, Zout_uT;

unsigned char GyroData[6];
short Xout_Gyro_16_bit, Yout_Gyro_16_bit, Zout_Gyro_16_bit;
float Roll, Pitch, Yaw;
char Temp;

char FXOS8700CQ_DataReady, FXAS21002C_DataReady;

/******************************************************************************
* Functions
******************************************************************************/

void MCU_Init(void);
void FXOS8700CQ_Init (void);
void FXOS8700CQ_Accel_Calibration (void);
void FXAS21002C_Init (void);

/******************************************************************************
* Main
******************************************************************************/  

int main (void)
{
	MCU_Init();
  	FXOS8700CQ_Init();
  	FXOS8700CQ_Accel_Calibration();
  	FXAS21002C_Init(); 
  	
  	FXAS21002C_DataReady = 0;
  	
  	while(1)
    {
		if (FXOS8700CQ_DataReady)		// Is a new set of accel+mag data ready? 
		{  		
			FXOS8700CQ_DataReady = 0;
																	
			I2C_ReadMultiRegisters(FXOS8700CQ_I2C_ADDRESS, OUT_X_MSB_REG, 12, AccelMagData);		// Read FXOS8700CQ data output registers 0x01-0x06 and 0x33 - 0x38
            
			// 14-bit accelerometer data
			Xout_Accel_14_bit = ((short) (AccelMagData[0]<<8 | AccelMagData[1])) >> 2;		// Compute 14-bit X-axis acceleration output value
			Yout_Accel_14_bit = ((short) (AccelMagData[2]<<8 | AccelMagData[3])) >> 2;		// Compute 14-bit Y-axis acceleration output value
			Zout_Accel_14_bit = ((short) (AccelMagData[4]<<8 | AccelMagData[5])) >> 2;		// Compute 14-bit Z-axis acceleration output value
			
			// Accelerometer data converted to g's 
			Xout_g = ((float) Xout_Accel_14_bit) / SENSITIVITY_2G;		// Compute X-axis output value in g's
			Yout_g = ((float) Yout_Accel_14_bit) / SENSITIVITY_2G;		// Compute Y-axis output value in g's
			Zout_g = ((float) Zout_Accel_14_bit) / SENSITIVITY_2G;		// Compute Z-axis output value in g's	
			
			// 16-bit magnetometer data			
			Xout_Mag_16_bit = (short) (AccelMagData[6]<<8 | AccelMagData[7]);		// Compute 16-bit X-axis magnetic output value
			Yout_Mag_16_bit = (short) (AccelMagData[8]<<8 | AccelMagData[9]);		// Compute 16-bit Y-axis magnetic output value
			Zout_Mag_16_bit = (short) (AccelMagData[10]<<8 | AccelMagData[11]);		// Compute 16-bit Z-axis magnetic output value
							
			// Magnetometer data converted to microteslas
			Xout_uT = (float) (Xout_Mag_16_bit) / SENSITIVITY_MAG;		// Compute X-axis output magnetic value in uT
			Yout_uT = (float) (Yout_Mag_16_bit) / SENSITIVITY_MAG;		// Compute Y-axis output magnetic value in uT
			Zout_uT = (float) (Zout_Mag_16_bit) / SENSITIVITY_MAG;		// Compute Z-axis output magnetic value in uT			
		}
		
		if (FXAS21002C_DataReady)		// Is a new set of gyro data ready? 
		{  		
			FXAS21002C_DataReady = 0;
																			
			I2C_ReadMultiRegisters(FXAS21002C_I2C_ADDRESS, GYRO_OUT_X_MSB_REG, 6, GyroData);		// Read FXAS21002C data output registers 0x01-0x06 
		            
			// 16-bit gyro data
			Xout_Gyro_16_bit = (short) (GyroData[0]<<8 | GyroData[1]);		// Compute 16-bit X-axis output value
			Yout_Gyro_16_bit = (short) (GyroData[2]<<8 | GyroData[3]);		// Compute 16-bit Y-axis output value
			Zout_Gyro_16_bit = (short) (GyroData[4]<<8 | GyroData[5]);		// Compute 16-bit Z-axis output value
					
			// Gyro data converted to dps
			Roll = (float) (Xout_Gyro_16_bit) / SENSITIVITY_250;		// Compute X-axis output value in dps
			Pitch = (float) (Yout_Gyro_16_bit) / SENSITIVITY_250;		// Compute Y-axis output value in dps
			Yaw = (float) (Zout_Gyro_16_bit) / SENSITIVITY_250;			// Compute Z-axis output value in dps	
			
			// Temperature data
			Temp = I2C_ReadRegister(FXAS21002C_I2C_ADDRESS, GYRO_TEMP_REG);			
		}   	
	}
}

/******************************************************************************
* MCU initialization function
******************************************************************************/ 

void MCU_Init(void)
{
	//I2C1 module initialization
	SIM_SCGC4 |= SIM_SCGC4_I2C1_MASK;		// Turn on clock to I2C1 module 
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;		// Turn on clock to Port C module 
	PORTC_PCR1 |= PORT_PCR_MUX(0x2);		// PTC1 pin is I2C1 SCL line 
	PORTC_PCR2 |= PORT_PCR_MUX(0x2);		// PTC2 pin is I2C1 SDA line 
	I2C1_F  |= I2C_F_ICR(0x14); 			// SDA hold time = 2.125us, SCL start hold time = 4.25us, SCL stop hold time = 5.125us
	I2C1_C1 |= I2C_C1_IICEN_MASK;    		// Enable I2C1 module 
		
	//Configure the PTD4 pin (connected to the INT1 of the FXOS8700CQ) for falling edge interrupts
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;		// Turn on clock to Port D module 
	PORTD_PCR4 |= (0|PORT_PCR_ISF_MASK|		// Clear the interrupt flag 
					PORT_PCR_MUX(0x1)|		// PTD4 is configured as GPIO 
					PORT_PCR_IRQC(0xA));	// PTD4 is configured for falling edge interrupts 	
	
	//Configure the PTA5 pin (connected to the INT1 of the FXAS21002) for falling edge interrupts
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;		// Turn on clock to Port A module 
	PORTA_PCR5 |= (0|PORT_PCR_ISF_MASK|		// Clear the interrupt flag 
					 PORT_PCR_MUX(0x1)|		// PTA5 is configured as GPIO 
					 PORT_PCR_IRQC(0xA));	// PTA5 is configured for falling edge interrupts
			
	//Enable PORTD interrupt on NVIC
	NVIC_ICPR |= 1 << ((INT_PORTD - 16)%32); 
	NVIC_ISER |= 1 << ((INT_PORTD - 16)%32);
	
	//Enable PORTA interrupt on NVIC
	NVIC_ICPR |= 1 << ((INT_PORTA - 16)%32); 
	NVIC_ISER |= 1 << ((INT_PORTA - 16)%32);
}

/******************************************************************************
* FXOS8700CQ initialization function
******************************************************************************/ 

void FXOS8700CQ_Init (void)
{
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG2, 0x40);          // Reset all registers to POR values
	Pause(0x631);		// ~1ms delay
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, XYZ_DATA_CFG_REG, 0x00);		// +/-2g range with 0.244mg/LSB	
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, M_CTRL_REG1, 0x1F);		// Hybrid mode (accelerometer + magnetometer), max OSR
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, M_CTRL_REG2, 0x20);		// M_OUT_X_MSB register 0x33 follows the OUT_Z_LSB register 0x06 (used for burst read)		
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG2, 0x02);     // High Resolution mode
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG3, 0x00);		// Push-pull, active low interrupt 
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG4, 0x01);		// Enable DRDY interrupt 
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG5, 0x01);		// DRDY interrupt routed to INT1 - PTD4 
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x25);		// ODR = 25Hz, Reduced noise, Active mode	
}

/******************************************************************************
* FXAS21002C initialization function
******************************************************************************/ 

void FXAS21002C_Init (void)
{
	I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, GYRO_CTRL_REG1, 0x40);	// Reset all registers to POR values
	Pause(0x631);		// ~1ms delay
	
	I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, GYRO_CTRL_REG0, 0x1F);	// High-pass filter enabled, HPF cutoff frequency = 0.031Hz, +/-250 dps range -> 7.8125 mdps/LSB = 128 LSB/dps 
	I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, GYRO_CTRL_REG2, 0x0C);	// Enable DRDY interrupt, mapped to INT1 - PTA5, push-pull, active low interrupt 
	I2C_WriteRegister(FXAS21002C_I2C_ADDRESS, GYRO_CTRL_REG1, 0x16);	// ODR = 25Hz, Active mode		
}

/******************************************************************************
* Simple accelerometer offset calibration
******************************************************************************/ 

void FXOS8700CQ_Accel_Calibration (void)
{
	char X_Accel_offset, Y_Accel_offset, Z_Accel_offset;
	
	FXOS8700CQ_DataReady = 0;	
	while (!FXOS8700CQ_DataReady){}		// Is a first set of data ready? 
	FXOS8700CQ_DataReady = 0;
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x00);		// Standby mode	
	
	I2C_ReadMultiRegisters(FXOS8700CQ_I2C_ADDRESS, OUT_X_MSB_REG, 6, AccelMagData);		// Read data output registers 0x01-0x06  	
		
	Xout_Accel_14_bit = ((short) (AccelMagData[0]<<8 | AccelMagData[1])) >> 2;		// Compute 14-bit X-axis acceleration output value
	Yout_Accel_14_bit = ((short) (AccelMagData[2]<<8 | AccelMagData[3])) >> 2;		// Compute 14-bit Y-axis acceleration output value
	Zout_Accel_14_bit = ((short) (AccelMagData[4]<<8 | AccelMagData[5])) >> 2;		// Compute 14-bit Z-axis acceleration output value
		
	X_Accel_offset = Xout_Accel_14_bit / 8 * (-1);		// Compute X-axis offset correction value
	Y_Accel_offset = Yout_Accel_14_bit / 8 * (-1);		// Compute Y-axis offset correction value
	Z_Accel_offset = (Zout_Accel_14_bit - SENSITIVITY_2G) / 8 * (-1);		// Compute Z-axis offset correction value
		
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, OFF_X_REG, X_Accel_offset);		
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, OFF_Y_REG, Y_Accel_offset);	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, OFF_Z_REG, Z_Accel_offset);	
		
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x35);		// Active mode again	
}

/******************************************************************************
* PORT D Interrupt handler
******************************************************************************/ 

void PORTD_IRQHandler()
{
	PORTD_PCR4 |= PORT_PCR_ISF_MASK;			// Clear the interrupt flag 
	FXOS8700CQ_DataReady = 1;	
}

/******************************************************************************
* PORT A Interrupt handler
******************************************************************************/ 

void PORTA_IRQHandler()
{
	PORTA_PCR5 |= PORT_PCR_ISF_MASK;			// Clear the interrupt flag 
	FXAS21002C_DataReady = 1;	
}

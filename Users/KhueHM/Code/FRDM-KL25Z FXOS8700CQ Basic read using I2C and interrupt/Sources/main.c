/******************************************************************************
* Freescale Semiconductor Inc.
* (c) Copyright 2014 Freescale Semiconductor, Inc.
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
* Date: May-25-2014
* Classification General Business Information
* FXOS8700CQ example code with use of FRDM-KL25Z and FRDM-FXS-MULTI(B) boards
*******************************************************************************
* This code has been written for the Freescale FRDM-KL25Z + FRDM-FXS-MULTI-B 
* boards and demonstrates how to read both the acceleration and magnetic data 
* from the FXOS8700CQ using an interrupt technique. 
* 
* This example also includes simple calibration for both the accelerometer and 
* magnetometer. Output values are available as signed 16-bit integer values
* and real values in g’s/uT. Simple heading angle calculation using the
* atan2 function is also demonstrated.
* 
* A detailed description of this example including hardware setup is available 
* on the sensors community web page at  
*******************************************************************************
Revision History:
Version		Date	 		Author		Description of Changes
1.0			May-25-2014		b12257		
******************************************************************************/

#include "derivative.h" 				// Include peripheral declarations 
#include "I2C.h"						// Include declarations of I2C communication routines 
#include "FXOS8700CQ.h"					// Include declarations of FXOS8700CQ registers 
#include <math.h>						// Include mathematical functions

/******************************************************************************
* Global variables and constants
******************************************************************************/

unsigned char AccelMagData[12];

short Xout_Accel_14_bit, Yout_Accel_14_bit, Zout_Accel_14_bit;
float Xout_g, Yout_g, Zout_g;

short Xout_Mag_16_bit, Yout_Mag_16_bit, Zout_Mag_16_bit;
float Xout_uT, Yout_uT, Zout_uT;
float Heading;

char DataReady;

#define PI					  3.14159

/******************************************************************************
* Functions
******************************************************************************/

void MCU_Init(void);
void FXOS8700CQ_Init (void);
void FXOS8700CQ_Accel_Calibration (void);
void FXOS8700CQ_Mag_Calibration (void);

/******************************************************************************
* Main
******************************************************************************/  

int main (void)
{
	MCU_Init();
  	FXOS8700CQ_Init();
  	FXOS8700CQ_Accel_Calibration();
   	FXOS8700CQ_Mag_Calibration();
  	
  	while(1)
    {
		if (DataReady)		// Is a new set of data ready? 
		{  		
			DataReady = 0;
																	
			I2C_ReadMultiRegisters(FXOS8700CQ_I2C_ADDRESS, OUT_X_MSB_REG, 12, AccelMagData);		// Read data output registers 0x01-0x06 and 0x33 - 0x38
            
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
			
			Heading = atan2 (Yout_uT, Xout_uT) * 180 / PI;		// Compute Yaw angle 
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
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;		// Turn on clock to Port A module 
	PORTD_PCR4 |= (0|PORT_PCR_ISF_MASK|		// Clear the interrupt flag 
					PORT_PCR_MUX(0x1)|		// PTD4 is configured as GPIO 
					PORT_PCR_IRQC(0xA));	// PTD4 is configured for falling edge interrupts 
		
	//Enable PORTD interrupt on NVIC
	NVIC_ICPR |= 1 << ((INT_PORTD - 16)%32); 
	NVIC_ISER |= 1 << ((INT_PORTD - 16)%32);
}

/******************************************************************************
* FXOS8700CQ initialization function
******************************************************************************/ 

void FXOS8700CQ_Init (void)
{
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG2, 0x40);		// Reset all registers to POR values
	
	Pause(0x631);		// ~1ms delay
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, XYZ_DATA_CFG_REG, 0x00);		// +/-2g range with 0.244mg/LSB	
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, M_CTRL_REG1, 0x1F);		// Hybrid mode (accelerometer + magnetometer), max OSR
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, M_CTRL_REG2, 0x20);		// M_OUT_X_MSB register 0x33 follows the OUT_Z_LSB register 0x06 (used for burst read)		
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG2, 0x02);     // High Resolution mode
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG3, 0x00);		// Push-pull, active low interrupt 
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG4, 0x01);		// Enable DRDY interrupt 
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG5, 0x01);		// DRDY interrupt routed to INT1 - PTD4 
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x35);		// ODR = 3.125Hz, Reduced noise, Active mode	
}

/******************************************************************************
* Simple accelerometer offset calibration
******************************************************************************/ 

void FXOS8700CQ_Accel_Calibration (void)
{
	char X_Accel_offset, Y_Accel_offset, Z_Accel_offset;
	
	DataReady = 0;	
	while (!DataReady){}		// Is a first set of data ready? 
	DataReady = 0;
	
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
* Simple magnetometer offset calibration
******************************************************************************/ 

void FXOS8700CQ_Mag_Calibration (void)
{
	short Xout_Mag_16_bit_avg, Yout_Mag_16_bit_avg, Zout_Mag_16_bit_avg;
	short Xout_Mag_16_bit_max, Yout_Mag_16_bit_max, Zout_Mag_16_bit_max;
	short Xout_Mag_16_bit_min, Yout_Mag_16_bit_min, Zout_Mag_16_bit_min; 
	char i = 0;
	
	DataReady = 0;
	
	while (i < 150)		// This will take ~30s (94 samples * 1/3.125) 
	{
		if (DataReady)		// Is a new set of data ready? 
		{  		
			DataReady = 0;
																						
			I2C_ReadMultiRegisters(FXOS8700CQ_I2C_ADDRESS, MOUT_X_MSB_REG, 6, AccelMagData);		// Read data output registers 0x33 - 0x38
		            
			Xout_Mag_16_bit = (short) (AccelMagData[0]<<8 | AccelMagData[1]);		// Compute 16-bit X-axis magnetic output value
			Yout_Mag_16_bit = (short) (AccelMagData[2]<<8 | AccelMagData[3]);		// Compute 16-bit Y-axis magnetic output value
			Zout_Mag_16_bit = (short) (AccelMagData[4]<<8 | AccelMagData[5]);		// Compute 16-bit Z-axis magnetic output value
			
			if (i == 0)
			{
				Xout_Mag_16_bit_max = Xout_Mag_16_bit;
				Xout_Mag_16_bit_min = Xout_Mag_16_bit;
					
				Yout_Mag_16_bit_max = Yout_Mag_16_bit;
				Yout_Mag_16_bit_min = Yout_Mag_16_bit;
					
				Zout_Mag_16_bit_max = Zout_Mag_16_bit;
				Zout_Mag_16_bit_min = Zout_Mag_16_bit;	
			}
			
			// Check to see if current sample is the maximum or minimum X-axis value
			if (Xout_Mag_16_bit > Xout_Mag_16_bit_max)	{Xout_Mag_16_bit_max = Xout_Mag_16_bit;}
			if (Xout_Mag_16_bit < Xout_Mag_16_bit_min)	{Xout_Mag_16_bit_min = Xout_Mag_16_bit;}

			// Check to see if current sample is the maximum or minimum Y-axis value
			if (Yout_Mag_16_bit > Yout_Mag_16_bit_max)	{Yout_Mag_16_bit_max = Yout_Mag_16_bit;}
			if (Yout_Mag_16_bit < Yout_Mag_16_bit_min)	{Yout_Mag_16_bit_min = Yout_Mag_16_bit;}
			
			// Check to see if current sample is the maximum or minimum Z-axis value
			if (Zout_Mag_16_bit > Zout_Mag_16_bit_max)	{Zout_Mag_16_bit_max = Zout_Mag_16_bit;}
			if (Zout_Mag_16_bit < Zout_Mag_16_bit_min)	{Zout_Mag_16_bit_min = Zout_Mag_16_bit;}
			
			i++;
		}	
	}
	
	Xout_Mag_16_bit_avg = (Xout_Mag_16_bit_max + Xout_Mag_16_bit_min) / 2;		// X-axis hard-iron offset
	Yout_Mag_16_bit_avg = (Yout_Mag_16_bit_max + Yout_Mag_16_bit_min) / 2;		// Y-axis hard-iron offset
	Zout_Mag_16_bit_avg = (Zout_Mag_16_bit_max + Zout_Mag_16_bit_min) / 2;		// Z-axis hard-iron offset
	
	// Left-shift by one as magnetometer offset registers are 15-bit only, left justified
	Xout_Mag_16_bit_avg <<= 1;		
	Yout_Mag_16_bit_avg <<= 1;
	Zout_Mag_16_bit_avg <<= 1;
			
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x00);		// Standby mode to allow writing to the offset registers
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, MOFF_X_LSB_REG, (char) (Xout_Mag_16_bit_avg & 0xFF));		
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, MOFF_X_MSB_REG, (char) ((Xout_Mag_16_bit_avg >> 8) & 0xFF));	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, MOFF_Y_LSB_REG, (char) (Yout_Mag_16_bit_avg & 0xFF));	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, MOFF_Y_MSB_REG, (char) ((Yout_Mag_16_bit_avg >> 8) & 0xFF));		
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, MOFF_Z_LSB_REG, (char) (Zout_Mag_16_bit_avg & 0xFF));	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, MOFF_Z_MSB_REG, (char) ((Zout_Mag_16_bit_avg >> 8) & 0xFF));	
	
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x35);		// Active mode again		
}

/******************************************************************************
* PORT D Interrupt handler
******************************************************************************/ 

void PORTD_IRQHandler()
{
	PORTD_PCR4 |= PORT_PCR_ISF_MASK;			// Clear the interrupt flag 
	DataReady = 1;	
}

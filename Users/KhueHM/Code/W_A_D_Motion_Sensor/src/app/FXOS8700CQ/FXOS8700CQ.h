/***********************************************************************
 *
 *      FXOS8700CQ Header File
 *
 **********************************************************************/

/* Include Header Files */
#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "debug_console_imx.h"
#include "i2c_imx.h"
#include "Sys_Delay.h"
#include <math.h>
#include "I2C_RW_Register.h"

/* FXOS8700CQ Registers */
#define FXOS8700CQ_STATUS_REG            	0x00		// STATUS Register

#define FXOS8700CQ_OUT_X_MSB_REG			0x01		// [7:0] are 8 MSBs of 14-bit acceleration data for X-axis
#define FXOS8700CQ_OUT_X_LSB_REG         	0x02		// [7:2] are the 6 LSB of 14-bit acceleration data for X-axis
#define FXOS8700CQ_OUT_Y_MSB_REG         	0x03		// [7:0] are 8 MSBs of 14-bit acceleration data for Y-axis
#define FXOS8700CQ_OUT_Y_LSB_REG         	0x04		// [7:2] are the 6 LSB of 14-bit acceleration data for Y-axis
#define FXOS8700CQ_OUT_Z_MSB_REG         	0x05		// [7:0] are 8 MSBs of 14-bit acceleration data for Z-axis
#define FXOS8700CQ_OUT_Z_LSB_REG         	0x06		// [7:2] are the 6 LSB of 14-bit acceleration data for Z-axis

#define FXOS8700CQ_F_SETUP_REG           	0x09    	// F_SETUP FIFO Setup Register
#define FXOS8700CQ_TRIG_CFG_REG          	0x0A    	// TRIG_CFG Map of FIFO data capture events
#define FXOS8700CQ_SYSMOD_REG            	0x0B    	// SYSMOD System Mode Register
#define FXOS8700CQ_INT_SOURCE_REG        	0x0C    	// INT_SOURCE System Interrupt Status Register
#define FXOS8700CQ_WHO_AM_I_REG          	0x0D    	// WHO_AM_I Device ID Register
#define FXOS8700CQ_XYZ_DATA_CFG_REG      	0x0E    	// XYZ_DATA_CFG Sensor Data Configuration Register
#define FXOS8700CQ_HP_FILTER_CUTOFF_REG  	0x0F    	// HP_FILTER_CUTOFF High Pass Filter Register

#define FXOS8700CQ_PL_STATUS_REG         	0x10    	// PL_STATUS Portrait/Landscape Status Register
#define FXOS8700CQ_PL_CFG_REG            	0x11    	// PL_CFG Portrait/Landscape Configuration Register
#define FXOS8700CQ_PL_COUNT_REG          	0x12    	// PL_COUNT Portrait/Landscape Debounce Register
#define FXOS8700CQ_PL_BF_ZCOMP_REG       	0x13    	// PL_BF_ZCOMP Back/Front and Z Compensation Register
#define FXOS8700CQ_P_L_THS_REG           	0x14    	// P_L_THS Portrait to Landscape Threshold Register

#define FXOS8700CQ_FF_MT_CFG_REG         	0x15    	// FF_MT_CFG Freefall and Motion Configuration Register
#define FXOS8700CQ_FF_MT_SRC_REG         	0x16    	// FF_MT_SRC Freefall and Motion Source Register
#define FXOS8700CQ_FT_MT_THS_REG         	0x17    	// FF_MT_THS Freefall and Motion Threshold Register
#define FXOS8700CQ_FF_MT_COUNT_REG       	0x18    	// FF_MT_COUNT Freefall Motion Count Register

#define FXOS8700CQ_TRANSIENT_CFG_REG     	0x1D    	// TRANSIENT_CFG Transient Configuration Register
#define FXOS8700CQ_TRANSIENT_SRC_REG     	0x1E    	// TRANSIENT_SRC Transient Source Register
#define FXOS8700CQ_TRANSIENT_THS_REG     	0x1F    	// TRANSIENT_THS Transient Threshold Register
#define FXOS8700CQ_TRANSIENT_COUNT_REG   	0x20    	// TRANSIENT_COUNT Transient Debounce Counter Register

#define FXOS8700CQ_PULSE_CFG_REG         	0x21    	// PULSE_CFG Pulse Configuration Register
#define FXOS8700CQ_PULSE_SRC_REG         	0x22    	// PULSE_SRC Pulse Source Register
#define FXOS8700CQ_PULSE_THSX_REG        	0x23    	// PULSE_THS XYZ Pulse Threshold Registers
#define FXOS8700CQ_PULSE_THSY_REG        	0x24
#define FXOS8700CQ_PULSE_THSZ_REG        	0x25
#define FXOS8700CQ_PULSE_TMLT_REG        	0x26    	// PULSE_TMLT Pulse Time Window Register
#define FXOS8700CQ_PULSE_LTCY_REG        	0x27    	// PULSE_LTCY Pulse Latency Timer Register
#define FXOS8700CQ_PULSE_WIND_REG        	0x28    	// PULSE_WIND Second Pulse Time Window Register

#define FXOS8700CQ_ASLP_COUNT_REG        	0x29    	// ASLP_COUNT Auto Sleep Inactivity Timer Register

#define FXOS8700CQ_CTRL_REG1             	0x2A    	// CTRL_REG1 System Control 1 Register
#define FXOS8700CQ_CTRL_REG2             	0x2B    	// CTRL_REG2 System Control 2 Register
#define FXOS8700CQ_CTRL_REG3             	0x2C    	// CTRL_REG3 Interrupt Control Register
#define FXOS8700CQ_CTRL_REG4             	0x2D    	// CTRL_REG4 Interrupt Enable Register
#define FXOS8700CQ_CTRL_REG5             	0x2E    	// CTRL_REG5 Interrupt Configuration Register

#define FXOS8700CQ_OFF_X_REG             	0x2F    	// XYZ Offset Correction Registers
#define FXOS8700CQ_OFF_Y_REG             	0x30
#define FXOS8700CQ_OFF_Z_REG             	0x31

#define FXOS8700CQ_M_DR_STATUS_REG			0x32		// M_DR_STATUS Magnetic data-ready status register

#define FXOS8700CQ_MOUT_X_MSB_REG			0x33		// MSB of 16-bit magnetic data for X-axis
#define FXOS8700CQ_MOUT_X_LSB_REG			0x34		// LSB of 16-bit magnetic data for X-axis
#define FXOS8700CQ_MOUT_Y_MSB_REG			0x35		// MSB of 16-bit magnetic data for Y-axis
#define FXOS8700CQ_MOUT_Y_LSB_REG			0x36		// LSB of 16-bit magnetic data for Y-axis
#define FXOS8700CQ_MOUT_Z_MSB_REG			0x37		// MSB of 16-bit magnetic data for Z-axis
#define FXOS8700CQ_MOUT_Z_LSB_REG			0x38		// LSB of 16-bit magnetic data for Z-axis

#define FXOS8700CQ_CMP_X_MSB_REG			0x39		// Bits [13:8] of integrated X-axis acceleration data
#define FXOS8700CQ_CMP_X_LSB_REG			0x3A		// Bits [7:0] of integrated X-axis acceleration data
#define FXOS8700CQ_CMP_Y_MSB_REG			0x3B		// Bits [13:8] of integrated Y-axis acceleration data
#define FXOS8700CQ_CMP_Y_LSB_REG			0x3C		// Bits [7:0] of integrated Y-axis acceleration data
#define FXOS8700CQ_CMP_Z_MSB_REG			0x3D		// Bits [13:8] of integrated Z-axis acceleration data
#define FXOS8700CQ_CMP_Z_LSB_REG			0x3E		// Bits [7:0] of integrated Z-axis acceleration data

#define FXOS8700CQ_MOFF_X_MSB_REG			0x3F		// MSB of magnetometer of X-axis offset
#define FXOS8700CQ_MOFF_X_LSB_REG			0x40		// LSB of magnetometer of X-axis offset
#define FXOS8700CQ_MOFF_Y_MSB_REG			0x41		// MSB of magnetometer of Y-axis offset
#define FXOS8700CQ_MOFF_Y_LSB_REG			0x42		// LSB of magnetometer of Y-axis offset
#define FXOS8700CQ_MOFF_Z_MSB_REG			0x43		// MSB of magnetometer of Z-axis offset
#define FXOS8700CQ_MOFF_Z_LSB_REG			0x44		// LSB of magnetometer of Z-axis offset

#define FXOS8700CQ_MAX_X_MSB_REG			0x45		// Magnetometer X-axis maximum value MSB
#define FXOS8700CQ_MAX_X_LSB_REG			0x46		// Magnetometer X-axis maximum value LSB
#define FXOS8700CQ_MAX_Y_MSB_REG			0x47		// Magnetometer Y-axis maximum value MSB
#define FXOS8700CQ_MAX_Y_LSB_REG			0x48		// Magnetometer Y-axis maximum value LSB
#define FXOS8700CQ_MAX_Z_MSB_REG			0x49		// Magnetometer Z-axis maximum value MSB
#define FXOS8700CQ_MAX_Z_LSB_REG			0x4A		// Magnetometer Z-axis maximum value LSB

#define FXOS8700CQ_MIN_X_MSB_REG			0x4B		// Magnetometer X-axis minimum value MSB
#define FXOS8700CQ_MIN_X_LSB_REG			0x4C		// Magnetometer X-axis minimum value LSB
#define FXOS8700CQ_MIN_Y_MSB_REG			0x4D		// Magnetometer Y-axis minimum value MSB
#define FXOS8700CQ_MIN_Y_LSB_REG			0x4E		// Magnetometer Y-axis minimum value LSB
#define FXOS8700CQ_MIN_Z_MSB_REG			0x4F		// Magnetometer Z-axis minimum value MSB
#define FXOS8700CQ_MIN_Z_LSB_REG			0x50		// Magnetometer Z-axis minimum value LSB

#define FXOS8700CQ_TEMP_REG				    0x51		// Device temperature
#define FXOS8700CQ_M_THS_CFG_REG			0x52		// Magnetic threshold detection function configuration register
#define FXOS8700CQ_M_THS_SRC_REG			0x53		// Magnetic threshold event source register

#define FXOS8700CQ_M_THS_X_MSB_REG			0x54		// X-axis magnetic threshold MSB
#define FXOS8700CQ_M_THS_X_LSB_REG			0x55		// X-axis magnetic threshold LSB
#define FXOS8700CQ_M_THS_Y_MSB_REG			0x56		// Y-axis magnetic threshold MSB
#define FXOS8700CQ_M_THS_Y_LSB_REG			0x57		// Y-axis magnetic threshold LSB
#define FXOS8700CQ_M_THS_Z_MSB_REG			0x58		// Z-axis magnetic threshold MSB
#define FXOS8700CQ_M_THS_Z_LSB_REG			0x59		// Z-axis magnetic threshold LSB

#define FXOS8700CQ_M_THS_COUNT_REG			0x5A		// Magnetic threshold debounce counter

#define FXOS8700CQ_M_CTRL_REG1				0x5B
#define FXOS8700CQ_M_CTRL_REG2				0x5C
#define FXOS8700CQ_M_CTRL_REG3				0x5D

#define FXOS8700CQ_M_INT_SRC_REG 			0x5E		// Magnetic interrupt source

#define FXOS8700CQ_A_VECM_CFG_REG			0x5F		// Acceleration vector-magnitude configuration register
#define FXOS8700CQ_A_VECM_THS_MSB_REG		0x60		// Acceleration vector-magnitude threshold MSB
#define FXOS8700CQ_A_VECM_THS_LSB_REG		0x61		// Acceleration vector-magnitude threshold LSB
#define FXOS8700CQ_A_VECM_CNT_REG			0x62		// Acceleration vector-magnitude debounce count

#define FXOS8700CQ_A_VECM_INITX_MSB_REG	    0x63		// Acceleration vector-magnitude X-axis reference value MSB
#define FXOS8700CQ_A_VECM_INITX_LSB_REG	    0x64		// Acceleration vector-magnitude X-axis reference value LSB
#define FXOS8700CQ_A_VECM_INITY_MSB_REG	    0x65		// Acceleration vector-magnitude Y-axis reference value MSB
#define FXOS8700CQ_A_VECM_INITY_LSB_REG	    0x66		// Acceleration vector-magnitude Y-axis reference value LSB
#define FXOS8700CQ_A_VECM_INITZ_MSB_REG	    0x67		// Acceleration vector-magnitude Z-axis reference value MSB
#define FXOS8700CQ_A_VECM_INITZ_LSB_REG	    0x68		// Acceleration vector-magnitude Z-axis reference value LSB

#define FXOS8700CQ_M_VECM_CFG_REG			0x69		// Magnetic vector-magnitude configuration register
#define FXOS8700CQ_M_VECM_THS_MSB			0x6A		// Magnetic vector-magnitude threshold MSB
#define FXOS8700CQ_M_VECM_THS_LSB			0x6B		// Magnetic vector-magnitude threshold LSB
#define FXOS8700CQ_M_VECM_CNT_REG			0x6C		// Magnetic vector-magnitude debounce count

#define FXOS8700CQ_M_VECM_INITX_MSB_REG	    0x6D		// Magnetic vector-magnitude X-axis reference value MSB
#define FXOS8700CQ_M_VECM_INITX_LSB_REG	    0x6E		// Magnetic vector-magnitude X-axis reference value LSB
#define FXOS8700CQ_M_VECM_INITY_MSB_REG	    0x6F		// Magnetic vector-magnitude Y-axis reference value MSB
#define FXOS8700CQ_M_VECM_INITY_LSB_REG	    0x70		// Magnetic vector-magnitude Y-axis reference value LSB
#define FXOS8700CQ_M_VECM_INITZ_MSB_REG	    0x71		// Magnetic vector-magnitude Z-axis reference value MSB
#define FXOS8700CQ_M_VECM_INITZ_LSB_REG	    0x72		// Magnetic vector-magnitude Z-axis reference value LSB

#define FXOS8700CQ_FFMT_THSX_MSB_REG		0x73		// X-axis FMT threshold MSB
#define FXOS8700CQ_A_FFMT_THSX_LSB_REG		0x74		// X-axis FMT threshold LSB
#define FXOS8700CQ_A_FFMT_THSY_MSB_REG		0x75		// Y-axis FMT threshold MSB
#define FXOS8700CQ_A_FFMT_THSY_LSB_REG		0x76		// Y-axis FMT threshold LSB
#define FXOS8700CQ_A_FFMT_THSZ_MSB_REG		0x77		// Z-axis FMT threshold MSB
#define FXOS8700CQ_A_FFMT_THSZ_LSB_REG		0x78		// Z-axis FMT threshold LSB

/* FXOS8700CQ WHO AM I Value */
#define FXOS8700CQ_WHO_AM_I_VAL             0xC7

/* FXOS8700CQ 7-bit I2C address */

#define FXOS8700CQ_I2C_ADDRESS 	            0x1E		// 7-bit I2C address is 0x1E

/* FXOS8700CQ Sensitivity */

#define SENSITIVITY_2G		  	            4096
#define SENSITIVITY_4G		  	            2048
#define SENSITIVITY_8G		  	            1024
#define SENSITIVITY_MAG		  	            10

/* FUNCTION */
void I2C_FXOS8700CQ_Init();
void FXOS8700CQ_Init();

void FXOS8700CQ_Standby();
void FXOS8700CQ_Active();
void FXOS8700CQ_Reset();

int FXOS8700CQ_WhoAmI_Check();

void FXOS8700CQ_Accel_Calib();
void FXOS8700CQ_Magnet_Calib();

void FXOS8700CQ_GetAccel();
void FXOS8700CQ_GetMag();

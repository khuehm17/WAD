/*
 * FXOS8700CQ.h
 *
 *  Created on: Jan 16, 2014
 *  Author: B12257
 */

#ifndef FXOS8700CQ_H_
#define FXOS8700CQ_H_

// FXOS8700CQ Registers

#define STATUS_REG 0x00  // STATUS Register

#define OUT_X_MSB_REG 0x01  // [7:0] are 8 MSBs of 14-bit acceleration data for X-axis
#define OUT_X_LSB_REG 0x02  // [7:2] are the 6 LSB of 14-bit acceleration data for X-axis
#define OUT_Y_MSB_REG 0x03  // [7:0] are 8 MSBs of 14-bit acceleration data for Y-axis
#define OUT_Y_LSB_REG 0x04  // [7:2] are the 6 LSB of 14-bit acceleration data for Y-axis
#define OUT_Z_MSB_REG 0x05  // [7:0] are 8 MSBs of 14-bit acceleration data for Z-axis
#define OUT_Z_LSB_REG 0x06  // [7:2] are the 6 LSB of 14-bit acceleration data for Z-axis

#define F_SETUP_REG 0x09  // F_SETUP FIFO Setup Register
#define TRIG_CFG_REG 0x0A  // TRIG_CFG Map of FIFO data capture events
#define SYSMOD_REG 0x0B  // SYSMOD System Mode Register
#define INT_SOURCE_REG 0x0C  // INT_SOURCE System Interrupt Status Register
#define WHO_AM_I_REG 0x0D  // WHO_AM_I Device ID Register
#define XYZ_DATA_CFG_REG 0x0E  // XYZ_DATA_CFG Sensor Data Configuration Register
#define HP_FILTER_CUTOFF_REG 0x0F  // HP_FILTER_CUTOFF High Pass Filter Register

#define PL_STATUS_REG 0x10  // PL_STATUS Portrait/Landscape Status Register
#define PL_CFG_REG 0x11  // PL_CFG Portrait/Landscape Configuration Register
#define PL_COUNT_REG 0x12  // PL_COUNT Portrait/Landscape Debounce Register
#define PL_BF_ZCOMP_REG 0x13  // PL_BF_ZCOMP Back/Front and Z Compensation Register
#define P_L_THS_REG 0x14  // P_L_THS Portrait to Landscape Threshold Register

#define FF_MT_CFG_REG 0x15  // FF_MT_CFG Freefall and Motion Configuration Register
#define FF_MT_SRC_REG 0x16  // FF_MT_SRC Freefall and Motion Source Register
#define FT_MT_THS_REG 0x17  // FF_MT_THS Freefall and Motion Threshold Register
#define FF_MT_COUNT_REG 0x18  // FF_MT_COUNT Freefall Motion Count Register

#define TRANSIENT_CFG_REG 0x1D  // TRANSIENT_CFG Transient Configuration Register
#define TRANSIENT_SRC_REG 0x1E  // TRANSIENT_SRC Transient Source Register
#define TRANSIENT_THS_REG 0x1F  // TRANSIENT_THS Transient Threshold Register
#define TRANSIENT_COUNT_REG 0x20  // TRANSIENT_COUNT Transient Debounce Counter Register

#define PULSE_CFG_REG 0x21  // PULSE_CFG Pulse Configuration Register
#define PULSE_SRC_REG 0x22  // PULSE_SRC Pulse Source Register
#define PULSE_THSX_REG 0x23  // PULSE_THS XYZ Pulse Threshold Registers
#define PULSE_THSY_REG 0x24
#define PULSE_THSZ_REG 0x25
#define PULSE_TMLT_REG 0x26  // PULSE_TMLT Pulse Time Window Register
#define PULSE_LTCY_REG 0x27  // PULSE_LTCY Pulse Latency Timer Register
#define PULSE_WIND_REG 0x28  // PULSE_WIND Second Pulse Time Window Register

#define ASLP_COUNT_REG 0x29  // ASLP_COUNT Auto Sleep Inactivity Timer Register

#define CTRL_REG1 0x2A  // CTRL_REG1 System Control 1 Register
#define CTRL_REG2 0x2B  // CTRL_REG2 System Control 2 Register
#define CTRL_REG3 0x2C  // CTRL_REG3 Interrupt Control Register
#define CTRL_REG4 0x2D  // CTRL_REG4 Interrupt Enable Register
#define CTRL_REG5 0x2E  // CTRL_REG5 Interrupt Configuration Register

#define OFF_X_REG 0x2F  // XYZ Offset Correction Registers
#define OFF_Y_REG 0x30
#define OFF_Z_REG 0x31

#define M_DR_STATUS_REG 0x32  // M_DR_STATUS Magnetic data-ready status register

#define MOUT_X_MSB_REG 0x33  // MSB of 16-bit magnetic data for X-axis
#define MOUT_X_LSB_REG 0x34  // LSB of 16-bit magnetic data for X-axis
#define MOUT_Y_MSB_REG 0x35  // MSB of 16-bit magnetic data for Y-axis
#define MOUT_Y_LSB_REG 0x36  // LSB of 16-bit magnetic data for Y-axis
#define MOUT_Z_MSB_REG 0x37  // MSB of 16-bit magnetic data for Z-axis
#define MOUT_Z_LSB_REG 0x38  // LSB of 16-bit magnetic data for Z-axis

#define CMP_X_MSB_REG 0x39  // Bits [13:8] of integrated X-axis acceleration data
#define CMP_X_LSB_REG 0x3A  // Bits [7:0] of integrated X-axis acceleration data
#define CMP_Y_MSB_REG 0x3B  // Bits [13:8] of integrated Y-axis acceleration data
#define CMP_Y_LSB_REG 0x3C  // Bits [7:0] of integrated Y-axis acceleration data
#define CMP_Z_MSB_REG 0x3D  // Bits [13:8] of integrated Z-axis acceleration data
#define CMP_Z_LSB_REG 0x3E  // Bits [7:0] of integrated Z-axis acceleration data

#define MOFF_X_MSB_REG 0x3F  // MSB of magnetometer of X-axis offset
#define MOFF_X_LSB_REG 0x40  // LSB of magnetometer of X-axis offset
#define MOFF_Y_MSB_REG 0x41  // MSB of magnetometer of Y-axis offset
#define MOFF_Y_LSB_REG 0x42  // LSB of magnetometer of Y-axis offset
#define MOFF_Z_MSB_REG 0x43  // MSB of magnetometer of Z-axis offset
#define MOFF_Z_LSB_REG 0x44  // LSB of magnetometer of Z-axis offset

#define MAX_X_MSB_REG 0x45  // Magnetometer X-axis maximum value MSB
#define MAX_X_LSB_REG 0x46  // Magnetometer X-axis maximum value LSB
#define MAX_Y_MSB_REG 0x47  // Magnetometer Y-axis maximum value MSB
#define MAX_Y_LSB_REG 0x48  // Magnetometer Y-axis maximum value LSB
#define MAX_Z_MSB_REG 0x49  // Magnetometer Z-axis maximum value MSB
#define MAX_Z_LSB_REG 0x4A  // Magnetometer Z-axis maximum value LSB

#define MIN_X_MSB_REG 0x4B  // Magnetometer X-axis minimum value MSB
#define MIN_X_LSB_REG 0x4C  // Magnetometer X-axis minimum value LSB
#define MIN_Y_MSB_REG 0x4D  // Magnetometer Y-axis minimum value MSB
#define MIN_Y_LSB_REG 0x4E  // Magnetometer Y-axis minimum value LSB
#define MIN_Z_MSB_REG 0x4F  // Magnetometer Z-axis minimum value MSB
#define MIN_Z_LSB_REG 0x50  // Magnetometer Z-axis minimum value LSB

#define TEMP_REG 0x51  // Device temperature
#define M_THS_CFG_REG 0x52  // Magnetic threshold detection function configuration register
#define M_THS_SRC_REG 0x53  // Magnetic threshold event source register

#define M_THS_X_MSB_REG 0x54  // X-axis magnetic threshold MSB
#define M_THS_X_LSB_REG 0x55  // X-axis magnetic threshold LSB
#define M_THS_Y_MSB_REG 0x56  // Y-axis magnetic threshold MSB
#define M_THS_Y_LSB_REG 0x57  // Y-axis magnetic threshold LSB
#define M_THS_Z_MSB_REG 0x58  // Z-axis magnetic threshold MSB
#define M_THS_Z_LSB_REG 0x59  // Z-axis magnetic threshold LSB

#define M_THS_COUNT_REG 0x5A  // Magnetic threshold debounce counter

#define M_CTRL_REG1 0x5B
#define M_CTRL_REG2 0x5C
#define M_CTRL_REG3 0x5D

#define M_INT_SRC_REG 0x5E  // Magnetic interrupt source

#define A_VECM_CFG_REG 0x5F  // Acceleration vector-magnitude configuration register
#define A_VECM_THS_MSB_REG 0x60  // Acceleration vector-magnitude threshold MSB
#define A_VECM_THS_LSB_REG 0x61  // Acceleration vector-magnitude threshold LSB
#define A_VECM_CNT_REG 0x62  // Acceleration vector-magnitude debounce count

#define A_VECM_INITX_MSB_REG 0x63  // Acceleration vector-magnitude X-axis reference value MSB
#define A_VECM_INITX_LSB_REG 0x64  // Acceleration vector-magnitude X-axis reference value LSB
#define A_VECM_INITY_MSB_REG 0x65  // Acceleration vector-magnitude Y-axis reference value MSB
#define A_VECM_INITY_LSB_REG 0x66  // Acceleration vector-magnitude Y-axis reference value LSB
#define A_VECM_INITZ_MSB_REG 0x67  // Acceleration vector-magnitude Z-axis reference value MSB
#define A_VECM_INITZ_LSB_REG 0x68  // Acceleration vector-magnitude Z-axis reference value LSB

#define M_VECM_CFG_REG 0x69  // Magnetic vector-magnitude configuration register
#define M_VECM_THS_MSB 0x6A  // Magnetic vector-magnitude threshold MSB
#define M_VECM_THS_LSB 0x6B  // Magnetic vector-magnitude threshold LSB
#define M_VECM_CNT_REG 0x6C  // Magnetic vector-magnitude debounce count

#define M_VECM_INITX_MSB_REG 0x6D  // Magnetic vector-magnitude X-axis reference value MSB
#define M_VECM_INITX_LSB_REG 0x6E  // Magnetic vector-magnitude X-axis reference value LSB
#define M_VECM_INITY_MSB_REG 0x6F  // Magnetic vector-magnitude Y-axis reference value MSB
#define M_VECM_INITY_LSB_REG 0x70  // Magnetic vector-magnitude Y-axis reference value LSB
#define M_VECM_INITZ_MSB_REG 0x71  // Magnetic vector-magnitude Z-axis reference value MSB
#define M_VECM_INITZ_LSB_REG 0x72  // Magnetic vector-magnitude Z-axis reference value LSB

#define A_FFMT_THSX_MSB_REG 0x73  // X-axis FMT threshold MSB
#define A_FFMT_THSX_LSB_REG 0x74  // X-axis FMT threshold LSB
#define A_FFMT_THSY_MSB_REG 0x75  // Y-axis FMT threshold MSB
#define A_FFMT_THSY_LSB_REG 0x76  // Y-axis FMT threshold LSB
#define A_FFMT_THSZ_MSB_REG 0x77  // Z-axis FMT threshold MSB
#define A_FFMT_THSZ_LSB_REG 0x78  // Z-axis FMT threshold LSB

// FXOS8700CQ 7-bit I2C address

#define FXOS8700CQ_I2C_ADDRESS 0x1E  // SA0&SA1 pins = 0 -> 7-bit I2C address is 0x1E

// FXOS8700CQ Sensitivity

#define SENSITIVITY_2G 4096
#define SENSITIVITY_4G 2048
#define SENSITIVITY_8G 1024
#define SENSITIVITY_MAG 10

#endif /* FXOS8700CQ_H_ */

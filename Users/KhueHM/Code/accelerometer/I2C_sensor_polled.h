/*
 * I2C_sensor_polled.h
 *
 *  Created on: Nov 4, 2014
 *      Author: B34874
 */

#ifndef I2C_SENSOR_POLLED_H_
#define I2C_SENSOR_POLLED_H_

#define I2C_SENSOR_BUS_ADDRESS 0x1D /* I2C bus address of sensor */
#define I2C_MCU_BUS_ADDRESS 0x60 /* I2C bus address of MCU */
#define I2C_BAUDRATE 375000 /* I2C bus baudrate */
// note: setings 400k will cause that 416k is real baudrate and this is over 400k specification
// therefore I set 375k as fast as possible below 400k

void I2C_init(void);
void i2c_write_register_polled(MQX_FILE_PTR, uint32_t, unsigned char);
void i2c_read_register_polled(MQX_FILE_PTR, uint32_t, unsigned char *, _mqx_int);

#endif /* I2C_SENSOR_POLLED_H_ */
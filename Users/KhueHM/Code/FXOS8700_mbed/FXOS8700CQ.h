/** @file FXOS8700CQ.h

@ brief FXOS8700CQ Library

@author Dr Craig A. Evans
@brief (c) University of Leeds, Jan 2017

@code

#include "mbed.h"
#include "FXOS8700CQ.h"

// create object and specifiy pins
FXOS8700CQ device(I2C_SDA,I2C_SCL);

int main()
{
    // call initialisation method
    device.init();

    while (1) {
        
        // poll the sensor and get the values, storing in a struct
        Data values = device.get_values();
        
        // print each struct member over serial
        printf("ax = %f ay = %f az = %f | mx = %f my = %f mz = %f\n"
               ,values.ax, values.ay, values.az
               ,values.mx, values.my, values.mz);
        
        wait(0.5);
    }
}

@endcode

*/

#ifndef FXOS8700CQ_H
#define FXOS8700CQ_H

#include "mbed.h"

// mbed API uses 8-bit addresses so need to left-shift 7-bit addresses by 1
#define FXOS8700CQ_ADDR   (0x1D << 1)    // for K64F board
// values from 13.2 datasheet
#define FXOS8700CQ_STATUS 0x00
#define FXOS8700CQ_WHO_AM_I 0x0D
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E
#define FXOS8700CQ_CTRL_REG1 0x2A
#define FXOS8700CQ_M_CTRL_REG1 0x5B
#define FXOS8700CQ_M_CTRL_REG2 0x5C
#define FXOS8700CQ_WHO_AM_I_VAL 0xC7
#define FXOS8700CQ_READ_LEN 13

#define PI 3.14159265359f
#define RAD2DEG 57.2957795131f

struct Data {
    float ax;
    float ay;
    float az;
    float mx;
    float my;
    float mz;
};

class FXOS8700CQ
{

public:
    FXOS8700CQ(PinName sda, PinName scl);
    ~FXOS8700CQ();
    void init();
    Data get_values();
    float get_pitch();
    float get_roll();

private:
    I2C* i2c;

    void send_byte_to_reg(char byte,char reg);
    char read_byte_from_reg(char reg);
    void read_bytes_from_reg(char reg,int number_of_bytes,char bytes[]);
};

#endif
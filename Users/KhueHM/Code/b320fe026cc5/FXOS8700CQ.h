/* FXOS8700CQ Library

Sample code from ELEC2645 - demonstrates how to create a library
for the K64F on-board accelerometer and magnetometer

(c) Craig A. Evans, University of Leeds, Jan 2017

*/

#ifndef FXOS8700CQ_H
#define FXOS8700CQ_H

#include "mbed.h"

// mbed API uses 8-bit addresses so need to left-shift 7-bit addresses by 1
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
//#define FXOS8700CQ_ADDR   FXOS8700CQ_SLAVE_ADDR1    // for K64F board
#define FXOS8700CQ_ADDR   FXOS8700CQ_SLAVE_ADDR0    // for Hexiwear board
// values from 13.2 datasheet
#define FXOS8700CQ_STATUS 0x00
#define FXOS8700CQ_WHO_AM_I 0x0D
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E
#define FXOS8700CQ_CTRL_REG1 0x2A
#define FXOS8700CQ_M_CTRL_REG1 0x5B
#define FXOS8700CQ_M_CTRL_REG2 0x5C
#define FXOS8700CQ_WHO_AM_I_VAL 0xC7
#define FXOS8700CQ_READ_LEN 13

#define PI 3.14159265358979323846f
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
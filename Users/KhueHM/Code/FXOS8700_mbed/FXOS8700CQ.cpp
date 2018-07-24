/* FXOS8700CQ Library

Sample code from ELEC2645 - demonstrates how to create a library
for the K64F on-board accelerometer and magnetometer

(c) Craig A. Evans, University of Leeds, Jan 2017

*/ 

#include "FXOS8700CQ.h"

// constructor is called when the object is created - use it to set pins and frequency
FXOS8700CQ::FXOS8700CQ(PinName sda, PinName scl)
{
    i2c = new I2C(sda,scl);        // create new I2C instance and initialise
}

// destructor is called when the object goes out of scope
FXOS8700CQ::~FXOS8700CQ()
{
    delete i2c;        // free memory
}

// based on 13.4 in datasheet - 200 Hz hybrid mode (both acc and mag)
void FXOS8700CQ::init()
{
    // i2c fast-mode - 10.1.1 data sheet
    i2c->frequency(400000);       // I2C Fast Mode - 400kHz

    // the device has an ID number so we check the value to ensure the correct
    // drive is on the i2c bus
    char data = read_byte_from_reg(FXOS8700CQ_WHO_AM_I);
    if (data != FXOS8700CQ_WHO_AM_I_VAL) { // if correct ID not found, hang and flash error message
        error("Incorrect ID!");
    }

    // write 0000 0000 = 0x00 to accelerometer control register 1 to place
    // FXOS8700CQ into standby
    // [7-1] = 0000 000
    // [0]: active=0
    data = 0x00;
    send_byte_to_reg(data,FXOS8700CQ_CTRL_REG1);

    // write 0001 1111 = 0x1F to magnetometer control register 1
    // [7]: m_acal=0: auto calibration disabled
    // [6]: m_rst=0: no one-shot magnetic reset
    // [5]: m_ost=0: no one-shot magnetic measurement
    // [4-2]: m_os=111=7: 8x oversampling (for 200Hz) to reduce magnetometer noise
    // [1-0]: m_hms=11=3: select hybrid mode with accel and magnetometer active
    data = 0x1F;
    send_byte_to_reg(data,FXOS8700CQ_M_CTRL_REG1);

    // write 0010 0000 = 0x20 to magnetometer control register 2
    // [7]: reserved
    // [6]: reserved
    // [5]: hyb_autoinc_mode=1 to map the magnetometer registers to follow
    // the accelerometer registers
    // [4]: m_maxmin_dis=0 to retain default min/max latching even though not used
    // [3]: m_maxmin_dis_ths=0
    // [2]: m_maxmin_rst=0
    // [1-0]: m_rst_cnt=00 to enable magnetic reset each cycle
    data = 0x20;
    send_byte_to_reg(data,FXOS8700CQ_M_CTRL_REG2);

    // write 0000 0001= 0x01 to XYZ_DATA_CFG register
    // [7]: reserved
    // [6]: reserved
    // [5]: reserved
    // [4]: hpf_out=0
    // [3]: reserved
    // [2]: reserved
    // [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg/LSB
    data = 0x01;
    send_byte_to_reg(data,FXOS8700CQ_XYZ_DATA_CFG);

    // write 0000 1101 = 0x0D to accelerometer control register 1
    // [7-6]: aslp_rate=00
    // [5-3]: dr=001 for 200Hz data rate (when in hybrid mode)
    // [2]: lnoise=1 for low noise mode
    // [1]: f_read=0 for normal 16 bit reads
    // [0]: active=1 to take the part out of standby and enable sampling
    data = 0x0D;
    send_byte_to_reg(data,FXOS8700CQ_CTRL_REG1);

}

Data FXOS8700CQ::get_values()
{
    // 13 bytes - status plus 6 channels (2 bytes each)
    // x,y,z for accelerometer and magnetometer
    char data[FXOS8700CQ_READ_LEN];
    read_bytes_from_reg(FXOS8700CQ_STATUS,FXOS8700CQ_READ_LEN,data);

    // copy the 14 bit accelerometer byte data into 16 bit words
    int acc_x = (int16_t)(((data[1] << 8) | data[2]))>> 2;
    int acc_y = (int16_t)(((data[3] << 8) | data[4]))>> 2;
    int acc_z = (int16_t)(((data[5] << 8) | data[6]))>> 2;

    // copy the magnetometer byte data into 16 bit words
    int mag_x = (int16_t) (data[7] << 8) | data[8];
    int mag_y = (int16_t) (data[9] << 8) | data[10];
    int mag_z = (int16_t) (data[11] << 8) | data[12];

    Data values;  // struct to hold values

    // 0.488 mg/LSB in 4 g mode (8.1 data sheet)
    values.ax = 0.488e-3*acc_x;
    values.ay = 0.488e-3*acc_y;
    values.az = 0.488e-3*acc_z;

    // the magnetometer sensitivity is fixed at 0.1 μT/LSB
    values.mx = 0.1e-6*mag_x;
    values.my = 0.1e-6*mag_y;
    values.mz = 0.1e-6*mag_z;

    return values;
}

void FXOS8700CQ::send_byte_to_reg(char byte,char reg)
{
    char data[2];
    data[0] = reg;
    data[1] = byte;
    // send the register address, followed by the data
    int nack = i2c->write(FXOS8700CQ_ADDR,data,2);
    if (nack)
        error("No acknowledgement received!");  // if we don't receive acknowledgement, send error message

}

// reads a byte from a specific register
char FXOS8700CQ::read_byte_from_reg(char reg)
{
    int nack = i2c->write(FXOS8700CQ_ADDR,&reg,1,true);  // send the register address to the slave
    // true as need to send repeated start condition (5.10.1 datasheet)
    // http://www.i2c-bus.org/repeated-start-condition/
    if (nack)
        error("No acknowledgement received!");  // if we don't receive acknowledgement, send error message

    char rx;
    nack = i2c->read(FXOS8700CQ_ADDR,&rx,1);  // read a byte from the register and store in buffer
    if (nack)
        error("No acknowledgement received!");  // if we don't receive acknowledgement, send error message

    return rx;
}

// reads a series of bytes, starting from a specific register
void FXOS8700CQ::read_bytes_from_reg(char reg,int number_of_bytes,char bytes[])
{
    int nack = i2c->write(FXOS8700CQ_ADDR,&reg,1,true);  // send the slave write address and the configuration register address
    // true as need to send repeated start condition (5.10.1 datasheet)
    // http://www.i2c-bus.org/repeated-start-condition/

    if (nack)
        error("No acknowledgement received!");  // if we don't receive acknowledgement, send error message

    nack = i2c->read(FXOS8700CQ_ADDR,bytes,number_of_bytes);  // read bytes
    if (nack)
        error("No acknowledgement received!");  // if we don't receive acknowledgement, send error message

}
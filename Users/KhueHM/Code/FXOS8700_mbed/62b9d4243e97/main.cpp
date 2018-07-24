/* FXOS8700CQ Library

Sample code from ELEC2645 - demonstrates how to create a library
for the K64F on-board accelerometer and magnetometer

(c) Craig A. Evans, University of Leeds, Jan 2017

*/ 

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
                
        wait(1.0);
    }
}
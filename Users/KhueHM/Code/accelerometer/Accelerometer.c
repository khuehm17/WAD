/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the source for the I2C sensor example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "FXOS8700CQ.h"  // Include declarations of FXOS8700CQ registers
#include "I2C_sensor_polled.h"  // Include polled I2C driver for sensor
#include <math.h>  // Include mathematical functions


/* Global variables */


LWGPIO_STRUCT ledR, ledG, ledB, btn1;
LWEVENT_STRUCT_PTR lwevent1_ptr;
uint8_t result;
uint8_t buffer[1];
uint8_t AccelMagData[12];
int16_t Xout_Accel_14_bit, Yout_Accel_14_bit, Zout_Accel_14_bit;
int16_t Xout_Mag_16_bit, Yout_Mag_16_bit, Zout_Mag_16_bit;
float Xout_g, Yout_g, Zout_g;
float Xout_uT, Yout_uT, Zout_uT;
float Heading;
// uint8_t DataReady;
LWSEM_STRUCT lwsem;
extern MQX_FILE_PTR fd;

#define PI 3.14159

#define SENSOR_USE_INTERRUPT 1  // Sensor generates interrupt when new data is available. When 0, it reads sensor status in loop 
#define ZYXDR_MASK 0x08  // DR_STATUS Register and M_DR_STATUS Magnetic data-ready status register zyxdr bit mask

/* Task IDs */
#define FXOS8700CQ_TASK 5

void FXOS8700CQ_task(uint32_t);
void int_service_routine(void *);
void int1_init(void);
extern double atan2(double, double);


const TASK_TEMPLATE_STRUCT MQX_template_list[] = {
    /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { FXOS8700CQ_TASK, FXOS8700CQ_task, 1500, 8, "FXOS8700CQ", MQX_AUTO_START_TASK, 0, 0 },
    { 0 }
};


void FXOS8700CQ_Init(void)
{
    i2c_read_register_polled(fd, WHO_AM_I_REG, buffer, 1);  // WHO_AM_I, result should be 0xC7
    i2c_write_register_polled(fd, CTRL_REG2, 0x40);  // write 0x00 to accelerometer control register 1 to place FXOS8700CQ into standby mode
    _time_delay(1);  // time delay 1ms
    i2c_write_register_polled(fd, XYZ_DATA_CFG_REG, 0x00);  // +/-2g range with 0.244mg/LSB
    i2c_write_register_polled(fd, M_CTRL_REG1, 0x1F);  // Hybrid mode (accelerometer + magnetometer), max OSR
    i2c_write_register_polled(fd, M_CTRL_REG2, 0x20);  // M_OUT_X_MSB register 0x33 follows the OUT_Z_LSB register 0x06 (used for burst read)
    i2c_write_register_polled(fd, CTRL_REG2, 0x02);  // High Resolution mode

    i2c_write_register_polled(fd, CTRL_REG3, 0x01);  // Open-drain, active low interrupt
#if SENSOR_USE_INTERRUPT
    i2c_write_register_polled(fd, CTRL_REG4, 0x01);  // Enable DRDY interrupt
    i2c_write_register_polled(fd, CTRL_REG5, 0x01);  // DRDY interrupt routed to INT1 - PTD4
#endif
    i2c_write_register_polled(fd, CTRL_REG1, 0x35);  // ODR = 3.125Hz, Reduced noise, Active mode
}
void FXOS8700CQ_Accel_Calibration(void)
{
    uint8_t X_Accel_offset, Y_Accel_offset, Z_Accel_offset;
#if SENSOR_USE_INTERRUPT
    _lwsem_wait(&lwsem);
#else
    do
    {
        i2c_read_register_polled(fd, STATUS_REG, buffer, 1);  // Read data output registers 0x01-0x06
    } while (!(buffer[0] && ZYXDR_MASK));  // while new set of accelerometer data is ready
#endif

    i2c_write_register_polled(fd, CTRL_REG1, 0x00);  // Standby mode
    i2c_read_register_polled(fd, OUT_X_MSB_REG, AccelMagData, 6);  // Read data output registers 0x01-0x06

    Xout_Accel_14_bit = ((int16_t)(AccelMagData[0] << 8 | AccelMagData[1])) >> 2;  // Compute 14-bit X-axis acceleration output value
    Yout_Accel_14_bit = ((int16_t)(AccelMagData[2] << 8 | AccelMagData[3])) >> 2;  // Compute 14-bit Y-axis acceleration output value
    Zout_Accel_14_bit = ((int16_t)(AccelMagData[4] << 8 | AccelMagData[5])) >> 2;  // Compute 14-bit Z-axis acceleration output value

    X_Accel_offset = Xout_Accel_14_bit / 8 * (-1);  // Compute X-axis offset correction value
    Y_Accel_offset = Yout_Accel_14_bit / 8 * (-1);  // Compute Y-axis offset correction value
    Z_Accel_offset = (Zout_Accel_14_bit - SENSITIVITY_2G) / 8 * (-1);  // Compute Z-axis offset correction value

    i2c_write_register_polled(fd, OFF_X_REG, X_Accel_offset);
    i2c_write_register_polled(fd, OFF_Y_REG, Y_Accel_offset);
    i2c_write_register_polled(fd, OFF_Z_REG, Z_Accel_offset);
    i2c_write_register_polled(fd, CTRL_REG1, 0x35);  // Active mode again
}

void FXOS8700CQ_Mag_Calibration(void)

{
    int16_t Xout_Mag_16_bit_avg, Yout_Mag_16_bit_avg, Zout_Mag_16_bit_avg;
    int16_t Xout_Mag_16_bit_max, Yout_Mag_16_bit_max, Zout_Mag_16_bit_max;
    int16_t Xout_Mag_16_bit_min, Yout_Mag_16_bit_min, Zout_Mag_16_bit_min;
    uint8_t i = 0;

    printf("Please rotate next 30s with board for magnetometer calibration \n");

    while (i < 94)  // This takes ~30s (94 samples * 1/3.125)
    {
#if SENSOR_USE_INTERRUPT
        _lwsem_wait(&lwsem);
#else
        do
        {
            i2c_read_register_polled(fd, M_DR_STATUS_REG, buffer, 1);  // Read data status register
        } while (!(buffer[0] && ZYXDR_MASK));  // while new set of magnetometer data is ready
#endif
        i2c_read_register_polled(fd, MOUT_X_MSB_REG, AccelMagData, 6);  // Read data output registers 0x33 - 0x38
        Xout_Mag_16_bit = (int16_t)(AccelMagData[0] << 8 | AccelMagData[1]);  // Compute 16-bit X-axis magnetic output value
        Yout_Mag_16_bit = (int16_t)(AccelMagData[2] << 8 | AccelMagData[3]);  // Compute 16-bit Y-axis magnetic output value
        Zout_Mag_16_bit = (int16_t)(AccelMagData[4] << 8 | AccelMagData[5]);  // Compute 16-bit Z-axis magnetic output value
        // Assign first sample to maximum and minimum values
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
        if (Xout_Mag_16_bit > Xout_Mag_16_bit_max)
        {
            Xout_Mag_16_bit_max = Xout_Mag_16_bit;
        }
        if (Xout_Mag_16_bit < Xout_Mag_16_bit_min)
        {
            Xout_Mag_16_bit_min = Xout_Mag_16_bit;
        }
        // Check to see if current sample is the maximum or minimum Y-axis value
        if (Yout_Mag_16_bit > Yout_Mag_16_bit_max)
        {
            Yout_Mag_16_bit_max = Yout_Mag_16_bit;
        }
        if (Yout_Mag_16_bit < Yout_Mag_16_bit_min)
        {
            Yout_Mag_16_bit_min = Yout_Mag_16_bit;
        }
        // Check to see if current sample is the maximum or minimum Z-axis value
        if (Zout_Mag_16_bit > Zout_Mag_16_bit_max)
        {
            Zout_Mag_16_bit_max = Zout_Mag_16_bit;
        }
        if (Zout_Mag_16_bit < Zout_Mag_16_bit_min)
        {
            Zout_Mag_16_bit_min = Zout_Mag_16_bit;
        }
        i++;
    }
    Xout_Mag_16_bit_avg = (Xout_Mag_16_bit_max + Xout_Mag_16_bit_min) / 2;  // X-axis hard-iron offset
    Yout_Mag_16_bit_avg = (Yout_Mag_16_bit_max + Yout_Mag_16_bit_min) / 2;  // Y-axis hard-iron offset
    Zout_Mag_16_bit_avg = (Zout_Mag_16_bit_max + Zout_Mag_16_bit_min) / 2;  // Z-axis hard-iron offset
    // Left-shift by one as magnetometer offset registers are 15-bit only, left justified
    Xout_Mag_16_bit_avg <<= 1;
    Yout_Mag_16_bit_avg <<= 1;
    Zout_Mag_16_bit_avg <<= 1;
    i2c_write_register_polled(fd, CTRL_REG1, 0x00);  // Standby mode to allow writing to the offset registers
    i2c_write_register_polled(fd, MOFF_X_LSB_REG, (uint8_t)(Xout_Mag_16_bit_avg & 0xFF));
    i2c_write_register_polled(fd, MOFF_X_MSB_REG, (uint8_t)((Xout_Mag_16_bit_avg >> 8) & 0xFF));
    i2c_write_register_polled(fd, MOFF_Y_LSB_REG, (uint8_t)(Yout_Mag_16_bit_avg & 0xFF));
    i2c_write_register_polled(fd, MOFF_Y_MSB_REG, (uint8_t)((Yout_Mag_16_bit_avg >> 8) & 0xFF));
    i2c_write_register_polled(fd, MOFF_Z_LSB_REG, (uint8_t)(Zout_Mag_16_bit_avg & 0xFF));
    i2c_write_register_polled(fd, MOFF_Z_MSB_REG, (uint8_t)((Zout_Mag_16_bit_avg >> 8) & 0xFF));
    i2c_write_register_polled(fd, CTRL_REG1, 0x35);  // Active mode again
}


/******************************************************************************
*
* Functio Name      : int_service_routine
* Comments          : The interrupt service routine triggered by gpio
*
******************************************************************************/
#if SENSOR_USE_INTERRUPT
void int_service_routine(void *pin)
{
    lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR)pin);
    _lwsem_post(&lwsem);
}
#endif

/******************************************************************************
*
* Functio Name      : int1_init_routine
* Comments          : The interrupt 1 gpio pin initialization
*
******************************************************************************/
#if SENSOR_USE_INTERRUPT
void int1_init(void)
{

    /* Create the lightweight semaphore */
    result = _lwsem_create(&lwsem, 1);
    if (result != MQX_OK)
    {
        printf("\nCreating sem failed: 0x%X", result);
        _task_block();
    }

    /* Open the pin (BSP_BTN1=INT1) for input, initialize interrupt
         and set interrupt handler */
    /* opening pins for input */
    if (!lwgpio_init(&btn1, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing button GPIO as input failed.\n");
        _task_block();
    }
    /* Some platforms require to setup MUX to IRQ functionality,
    for the rest just set MUXto GPIO functionality */
    lwgpio_set_functionality(&btn1, BSP_BUTTON1_MUX_IRQ);
#if defined(BSP_BUTTONS_ACTIVE_HIGH)
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#else
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* enable gpio functionality for given pin, react on falling edge */
    if (!lwgpio_int_init(&btn1, LWGPIO_INT_MODE_FALLING))
    {
        printf("Initializing button GPIO for interrupt failed.\n");
        _task_block();
    }
    /* install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&btn1), int_service_routine, (void *)&btn1);
    /* set the interrupt level, and unmask the interrupt in interrupt controller */
    _bsp_int_init(lwgpio_int_get_vector(&btn1), 4, 0, TRUE);
    /* enable interrupt on GPIO peripheral module */
    lwgpio_int_enable(&btn1, TRUE);
}
#endif
/******************************************************************************
*
* Functio Name      : LED_init_routine
* Comments          : The RGB LED initialization
*
******************************************************************************/
void LED_init(void)
{
#ifdef BSP_LED1
    /******************************************************************************
        Open the BSP_LED1) pin as output and drive the output level high.
    ******************************************************************************/
    /* initialize lwgpio handle (ledR) for BSP_LED1 pin
     * (defined in mqx/source/bsp/<bsp_name>/<bsp_name>.h file)
     */
    if (!lwgpio_init(&ledR, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED1 GPIO as output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(&ledR, BSP_LED1_MUX_GPIO);

    /* write logical 1 to the pin */
    lwgpio_set_value(&ledR, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif
#ifdef BSP_LED2
    /******************************************************************************
        Open the BSP_LED2) pin as output and drive the output level high.
    ******************************************************************************/
    /* initialize lwgpio handle (ledG) for BSP_LED2 pin
     * (defined in mqx/source/bsp/<bsp_name>/<bsp_name>.h file)
     */
    if (!lwgpio_init(&ledG, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED2 GPIO as output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(&ledG, BSP_LED2_MUX_GPIO);

    /* write logical 1 to the pin */
    lwgpio_set_value(&ledG, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif
#ifdef BSP_LED3
    /******************************************************************************
        Open the BSP_LED3) pin as output and drive the output level high.
    ******************************************************************************/
    /* initialize lwgpio handle (ledB) for BSP_LED3 pin
     * (defined in mqx/source/bsp/<bsp_name>/<bsp_name>.h file)
     */
    if (!lwgpio_init(&ledB, BSP_LED3, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED3 GPIO as output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(&ledB, BSP_LED3_MUX_GPIO);

    /* write logical 1 to the pin */
    lwgpio_set_value(&ledB, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif
}

/*TASK*-----------------------------------------------------
*
* Task Name    : FXOS8700CQ_task
* Comments     :
*    This task connects to FXOS8700CQ
*
*END*-----------------------------------------------------*/
void FXOS8700CQ_task(uint32_t initial_data)
{
#if SENSOR_USE_INTERRUPT
    int1_init();  // initialize interrupt signal from sensor
#endif
    LED_init();  // initialize LED signals for Accelerometer
    I2C_init();  // initialize I2C driver
    FXOS8700CQ_Init();  // initialize sensor
    FXOS8700CQ_Accel_Calibration();
    FXOS8700CQ_Mag_Calibration();  // be patient, it takes approximately 30s


    while (1)
    {
#if SENSOR_USE_INTERRUPT
        _lwsem_wait(&lwsem);
#else
        do
        {
            i2c_read_register_polled(fd, STATUS_REG, buffer, 1);  // Read data status register
        } while (!(buffer[0] && ZYXDR_MASK));  // while new set of accelerometer data is ready
#endif
        {
            i2c_read_register_polled(fd, OUT_X_MSB_REG, AccelMagData, 12);  // Read data output registers 0x01-0x06 and 0x33 - 0x38

            // 14-bit accelerometer data
            Xout_Accel_14_bit = ((int16_t)(AccelMagData[0] << 8 | AccelMagData[1])) >> 2;  // Compute 14-bit X-axis acceleration output value
            Yout_Accel_14_bit = ((int16_t)(AccelMagData[2] << 8 | AccelMagData[3])) >> 2;  // Compute 14-bit Y-axis acceleration output value
            Zout_Accel_14_bit = ((int16_t)(AccelMagData[4] << 8 | AccelMagData[5])) >> 2;  // Compute 14-bit Z-axis acceleration output value
            // Accelerometer data converted to g's
            Xout_g = ((float)Xout_Accel_14_bit) / SENSITIVITY_2G;  // Compute X-axis output value in g's
            Yout_g = ((float)Yout_Accel_14_bit) / SENSITIVITY_2G;  // Compute Y-axis output value in g's
            Zout_g = ((float)Zout_Accel_14_bit) / SENSITIVITY_2G;  // Compute Z-axis output value in g's
            if ((Xout_g > 0.5) | (Xout_g < -0.5))
                lwgpio_set_value(&ledR, FALSE);  // enable RED LED when g > 0,5 in X direction
            else
                lwgpio_set_value(&ledR, TRUE);  // disable RED LED when g < 0,5 in X direction
            if ((Yout_g > 0.5) | (Yout_g < -0.5))
                lwgpio_set_value(&ledG, FALSE);  // enable GREEN LED when g > 0,5 in Y direction
            else
                lwgpio_set_value(&ledG, TRUE);  // disable GREEN LED when g < 0,5 in Z direction
            if ((Zout_g > 0.5) | (Zout_g < -0.5))
                lwgpio_set_value(&ledB, FALSE);  // enable BLUE LED when g > 0,5 in Z direction
            else
                lwgpio_set_value(&ledB, TRUE);  // disable BLUE LED when g < 0,5 in Z direction

            // 16-bit magnetometer data
            Xout_Mag_16_bit = (int16_t)(AccelMagData[6] << 8 | AccelMagData[7]);  // Compute 16-bit X-axis magnetic output value
            Yout_Mag_16_bit = (int16_t)(AccelMagData[8] << 8 | AccelMagData[9]);  // Compute 16-bit Y-axis magnetic output value
            Zout_Mag_16_bit = (int16_t)(AccelMagData[10] << 8 | AccelMagData[11]);  // Compute 16-bit Z-axis magnetic output value
            // Magnetometer data converted to microteslas
            Xout_uT = (float)(Xout_Mag_16_bit) / SENSITIVITY_MAG;  // Compute X-axis output magnetic value in uT
            Yout_uT = (float)(Yout_Mag_16_bit) / SENSITIVITY_MAG;  // Compute Y-axis output magnetic value in uT
            Zout_uT = (float)(Zout_Mag_16_bit) / SENSITIVITY_MAG;  // Compute Z-axis output magnetic value in uT
            Heading = atan2(Yout_uT, Xout_uT) * 180 / PI;  // Compute Yaw angle

            // Print results
            printf("Accelerometer X=%f, Y=%f , Z=%f \n", Xout_g, Yout_g, Zout_g);
            printf("Magnetometer Heading=%f \n", Heading);
        }
    }
}

/* EOF */

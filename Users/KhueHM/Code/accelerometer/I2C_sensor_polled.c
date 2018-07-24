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
#include <fio.h>
#include <i2c.h>
#include "I2C_sensor_polled.h"  // Include polled I2C driver for sensor

#if !BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if defined BSP_VYBRID_AUTOEVB_A5 || defined BSP_VYBRID_AUTOEVB_M4
#define I2C_DEVICE_POLLED "i2c1:"
#define I2C_DEVICE_INTERRUPT "ii2c1:"
#define ENABLE_I2C_INTERRUPT BSPCFG_ENABLE_II2C1
#if !BSPCFG_ENABLE_I2C1
#error This application requires BSPCFG_ENABLE_I2C1 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#elif defined BSP_TWRPXS30
#define I2C_DEVICE_POLLED "i2c2:"
#define I2C_DEVICE_INTERRUPT "ii2c2:"
#define ENABLE_I2C_INTERRUPT BSPCFG_ENABLE_II2C2
#if !BSPCFG_ENABLE_I2C2
#error This application requires BSPCFG_ENABLE_I2C2 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#else
#define I2C_DEVICE_POLLED "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"
#define ENABLE_I2C_INTERRUPT BSPCFG_ENABLE_II2C0
#if !BSPCFG_ENABLE_I2C0
#error This application requires BSPCFG_ENABLE_I2C0 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif
#endif

MQX_FILE_PTR fd;
LWSEM_STRUCT lock;

/******************************************************************************
*
* Functio Name      : I2C_init_routine
* Comments          : I2C initialization
*
******************************************************************************/
void I2C_init(void)
{
    _mqx_int param;
    I2C_STATISTICS_STRUCT stats;

    /* I2C transaction lock */
    _lwsem_create(&lock, 1);

    /* Open the I2C driver */
    fd = fopen(I2C_DEVICE_POLLED, NULL);
    if (fd == NULL)
    {
        printf("ERROR opening the I2C driver!\n");
        _task_block();
    }

    /* Test ioctl commands */
    param = I2C_BAUDRATE;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_SET_BAUD, &param))
    {
        printf("ERROR set budrate\n");
    }

    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_GET_BAUD, &param))
    {
        printf("ERROR get budrate\n");
    }

    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL))
    {
        printf("ERROR set mode\n");
    }

    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_GET_MODE, &param))
    {
        printf("ERROR get mode\n");
    }

    param = I2C_MCU_BUS_ADDRESS;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_SET_STATION_ADDRESS, &param))
    {
        printf("ERROR set station address\n");
    }

    param = 0x00;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_GET_STATION_ADDRESS, &param))
    {
        printf("ERROR get station address\n");
    }

    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL))
    {
        printf("ERROR clear stats\n");
    }

    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats))
    {
        printf("ERROR get stats\n");
    }

    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param))
    {
        printf("ERROR get state\n");
    }

    param = I2C_SENSOR_BUS_ADDRESS;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
    {
        printf("ERROR set destination address\n");
    }

    param = 0x00;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, &param))
    {
        printf("ERROR get destination address\n");
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : i2c_write_register_polled
* Returned Value   : void
* Comments         :
*   Writes the provided data buffer at address in I2C sensors
*
*END*********************************************************************/

void i2c_write_register_polled(
    /* [IN] The file pointer for the I2C channel */
    MQX_FILE_PTR fd,

    /* [IN] The address of register to write to */
    uint32_t addr,

    /* [IN] The character that are to be written in register */
    unsigned char data)
{ /* Body */

    _mqx_int result;
    uint8_t mem[1];

    /* Protect I2C transaction in multitask environment */
    _lwsem_wait(&lock);

    mem[0] = I2C_SENSOR_BUS_ADDRESS;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, mem))
    {
        printf("ERROR set destination address\n");
    }

    /* Initiate start and send I2C bus address */
    result = fwrite(mem, 1, 0, fd);

    /* Check ack (device exists) */
    if (I2C_OK == ioctl(fd, IO_IOCTL_FLUSH_OUTPUT, mem))
    {
        if (mem[0])
        {
            /* Stop I2C transfer */
            if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_STOP, NULL))
            {
                printf("ERROR generate stop\n");
            }
        }
    }
    else
    {
        printf("ERROR flushes the output buffer\n");
    }

    /* send register address */
    mem[0] = (uint8_t)addr;
    result = fwrite(mem, 1, 1, fd);
    if (1 != result)
    {
        printf("ERROR send address\n");
    }

    /* write data to register*/
    mem[0] = data;
    result = fwrite(mem, 1, 1, fd);
    if (1 != result)
    {
        printf("ERROR write data\n");
    }

    /* Wait for completion */
    result = fflush(fd);
    if (MQX_OK != result)
    {
        printf("ERROR flush\n");
    }

    /* Stop I2C transfer - initiate sensor write cycle */
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_STOP, NULL))
    {
        printf("ERROR generate stop\n");
    }

    /* Release the transaction lock */
    _lwsem_post(&lock);
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : i2c_read_register_polled
* Returned Value   : void
* Comments         :
*   Reads into the provided data buffer from address in I2C sensor
*
*END*********************************************************************/

void i2c_read_register_polled(
    /* [IN] The file pointer for the I2C channel */
    MQX_FILE_PTR fd,

    /* [IN] The address of register to read from */
    uint32_t addr,

    /* [IN] The array of characters to be written into */
    unsigned char *buffer,

    /* [IN] Number of bytes to read */
    _mqx_int length)
{ /* Body */
    _mqx_int result;
    uint8_t mem[1];

    if (0 == length)
    {
        printf("  Nothing to do.\n");
        return;
    }

    /* Protect I2C transaction in multitask environment */
    _lwsem_wait(&lock);

    /* I2C bus address also contains memory block index */
    mem[0] = I2C_SENSOR_BUS_ADDRESS;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, mem))
    {
        printf("ERROR set destination address\n");
    }
    /* Initiate start and send I2C bus address */
    result = fwrite(mem, 1, 0, fd);

    /* Check ack (device exists) */
    if (I2C_OK == ioctl(fd, IO_IOCTL_FLUSH_OUTPUT, mem))
    {
        if (mem[0])
        {
            /* Stop I2C transfer */
            if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_STOP, NULL))
            {
                printf("ERROR generate stop\n");
            }

            /* Release the transaction lock */
            _lwsem_post(&lock);
            return;
        }
    }
    else
    {
        printf("ERROR flushes the output buffer\n");
    }

    /* send register address */
    mem[0] = (uint8_t)addr;
    result = fwrite(mem, 1, 1, fd);
    if (1 != result)
    {
        printf("ERROR send address\n");
    }

    /* Wait for completion */
    result = fflush(fd);
    if (MQX_OK != result)
    {
        printf("ERROR flush\n");
    }

    /* Restart I2C transfer for reading */
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_REPEATED_START, NULL))
    {
        printf("ERROR restart I2C transfer for reading\n");
    }

    /* Set read request */
    mem[0] = length;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_SET_RX_REQUEST, mem))
    {
        printf("ERROR set read request\n");
    }

    /* Read all data */
    result = fread(buffer, 1, length, fd);
    if (result != length)
    {
        printf("ERROR read data\n");
    }

    /* Stop I2C transfer */
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_STOP, NULL))
    {
        printf("ERROR generate stop\n");
    }

    /* Release the transaction lock */
    _lwsem_post(&lock);
} /* Endbody */

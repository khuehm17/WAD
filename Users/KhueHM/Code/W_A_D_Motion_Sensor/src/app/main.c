/////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "uart_imx.h"
#include "board.h"
#include "debug_console_imx.h"
#include "gpio_imx.h"
#include "rdc_semaphore.h"
#include "debug_console_imx.h"
#include "Sys_Delay.h"



const uint8_t bufferData2[] = "Hello World\r\n ";

static void UART_SendData(UART_Type *base, const uint8_t *txBuff, uint32_t txSize);



int main(void)
{
    uart_init_config_t initConfig = {
        .baudRate   = 115200u,
        .wordLength = uartWordLength8Bits,
        .stopBitNum = uartStopBitNumOne,
        .parity     = uartParityDisable,
        .direction  = uartDirectionTxRx
    };

    

    /* Initialize board specified hardware. */
    hardware_init();

    // Get current module clock frequency.
    initConfig.clockRate  = get_uart_clock_freq(BOARD_DEBUG_UART_BASEADDR);

    /* Initialize UART baud rate, bit count, parity, stop bit and direction. */
    UART_Init(BOARD_DEBUG_UART_BASEADDR, &initConfig);

    /* Set UART build-in hardware FIFO Watermark. */
    UART_SetTxFifoWatermark(BOARD_DEBUG_UART_BASEADDR, 16);
    UART_SetRxFifoWatermark(BOARD_DEBUG_UART_BASEADDR, 1);

    /* Finally, enable the UART module */
    UART_Enable(BOARD_DEBUG_UART_BASEADDR);
	
	uint32_t byteCount = 0;
    byteCount = sizeof(bufferData2);

    while (true)
    {
        UART_SendData(BOARD_DEBUG_UART_BASEADDR, bufferData2, byteCount);
        ms_delay(100);
    };
}

static void UART_SendData(UART_Type *base, const uint8_t *txBuff, uint32_t txSize)
{
    while (txSize--)
    {
        while (!UART_GetStatusFlag(base, uartStatusTxComplete));
        UART_Putchar(base, *txBuff++);
    }
}

// UART Receiving Data Function
static void UART_ReceiveData(UART_Type *base, uint8_t *rxBuff, uint32_t rxSize)
{
    while (rxSize--)
    {
        while (!UART_GetStatusFlag(base, uartStatusRxReady));
        *rxBuff = UART_Getchar(base);
        rxBuff++;

        if (UART_GetStatusFlag(base, uartStatusRxOverrun))
            UART_ClearStatusFlag(base, uartStatusRxOverrun);
    }
}

/***********************************************************************
 *
 *      Lora SX1278 Source File
 *
 **********************************************************************/
/*
 * Include Header Files
 */
#include "Lora.h"


/*
 * UART Lora Initialization
 */
void UART_Lora_Init()
{
    // UART Lora Configuration
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
}

/*
 * Lora Send Data Function
 */
void Lora_Send(const char *Data)
{
    debug_printf(Data);
}

/*
 * Lora Receive Data Function
 */
unsigned char Lora_Receive()
{
    unsigned char Data = 0;
    UART_ReceiveData(BOARD_DEBUG_UART_BASEADDR, Data, sizeof(Data));
    return Data;
}

/*
 * UART Send Data Function
 */
void UART_SendData(UART_Type *base, const uint8_t *txBuff, uint32_t txSize)
{
    while (txSize--)
    {
        while (!UART_GetStatusFlag(base, uartStatusTxComplete));
        UART_Putchar(base, *txBuff++);
    }
}

/*
 * UART Receive Data Function
 */
void UART_ReceiveData(UART_Type *base, uint8_t *rxBuff, uint32_t rxSize)
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

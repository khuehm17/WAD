/***********************************************************************
 *
 *      Lora SX1278 Header File
 *
 **********************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "uart_imx.h"
#include "board.h"

/* FUNCTION */
void UART_Lora_Init();
void UART_SendData(UART_Type *base, const uint8_t *txBuff, uint32_t txSize);
void UART_ReceiveData(UART_Type *base, uint8_t *rxBuff, uint32_t rxSize);
void Lora_Send(const char *Data);
unsigned char Lora_Receive();

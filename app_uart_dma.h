#ifndef APP_UART_DMA_H
#define APP_UART_DMA_H

#include "drv_dma.h"
#include "drv_uart_dma.h"

/*
 * App-layer wrapper for one UART instance.
 * Put global uart_dma_rx_t + ring buffer here to keep drv_uart_dma clean.
 * Duplicate this file for multiple UARTs if needed.
 */

void APP_UART0_DMA_Init(void);
void APP_UART0_DMA_TimerIsr(void);

uint16_t APP_UART0_DMA_PeekFrame(const uint8_t **p1,
                                 uint16_t *len1,
                                 const uint8_t **p2,
                                 uint16_t *len2);

void APP_UART0_DMA_ConsumeFrame(void);

/* App-level DMA config wrappers (no parameters). */
void APP_UART0_DMA_SetCh0(void);
void APP_UART0_DMA_SetCh1(void);

/* DMA ISR handlers for UART TX/RX. */
void APP_UART0_DMA_TxIsr(void);
void APP_UART0_DMA_RxIsr(void);

/* done flag is asserted by idle-timeout (not by RX DMA ISR). */
uint8_t APP_UART0_DMA_RxDoneFlagGet(void);
void APP_UART0_DMA_RxDoneFlagClear(void);

/* Write + setup + start TX DMA in one call. */
void APP_UART0_DMA_TxSend(const uint8_t *data, uint16_t len);

/* Configure PEG window to allow DMA access to SRAM */
void APP_UART0_PEG_Set(void);


void APP_UART0_DMA_Tx_process(void);
void APP_UART0_DMA_Rx_process(void);

void APP_UART0_DMA_DebugDump(void);
void APP_UART0_DMA_RxDebugDump(void);
void APP_UART0_DMA_TxDump(const uint8_t *pucBuff, uint16_t nBytes);
void APP_UART0_DMA_RxDump(uint8_t first_byte, 
                            uint16_t last_byte,
                            const uint8_t *p1,
                            uint16_t len1,
                            const uint8_t *p2,
                            uint16_t len2);
                            
#endif /* APP_UART_DMA_H */

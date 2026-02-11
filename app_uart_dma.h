#ifndef APP_UART_DMA_H
#define APP_UART_DMA_H

#include "drv_dma.h"

void APP_UART0_DMA_Init(void);
void APP_UART0_DMA_TimerIsr(void);

/* App-level DMA config wrappers. */
void APP_UART0_DMA_SetCh0(uint16_t len);
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


/* TX flow is handled by GMSL module (APP_GMSL_TxProcess). */
void APP_UART0_DMA_Tx_process(void);
void APP_UART0_DMA_Rx_process(void);

void APP_UART0_DMA_DebugDump(void);
void APP_UART0_DMA_RxDebugDump(void);
void APP_UART0_DMA_TxDump(const uint8_t *pucBuff, uint16_t nBytes);
                            
#endif /* APP_UART_DMA_H */

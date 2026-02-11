#ifndef APP_UART0_CONFIG_H
#define APP_UART0_CONFIG_H

#include "r_cg_macrodriver.h"
#include "drv_dma.h"

/* LOCAL_RAM_CPU1 start address (PEG) */
#define ADDR_LOCAL_RAM_CPU1         (0xFEBF0000)

/* UART0 common parameters (shared by RX-only and DMA). */
#define APP_UART0_RX_BUF_SIZE       (384U)
#define APP_UART0_TX_BUF_SIZE       (384U)
// #define APP_UART0_BAUD              (415000U)
#define APP_UART0_BAUD              (115200U)

/* UART0 RX mode select: 1 = DMA RX, 0 = interrupt RX-only */
#define APP_UART0_RX_MODE_DMA       (0U)

/* RLIN30 TX , RX data register*/
#define APP_UART0_TX_DR             (RLN30.LUTDR.UINT8[0])
#define APP_UART0_RX_DR             (RLN30.LURDR.UINT8[0])

/* UART0 DMA config. */
#define APP_UART0_DMA_RX_RING_SIZE  (APP_UART0_RX_BUF_SIZE)
#define APP_UART0_DMA_TX_BUF_SIZE   (APP_UART0_TX_BUF_SIZE)

/* DMA routing for this UART TX/RX channel. */
#define APP_UART0_DMA_UNIT          (DRV_DMA_UNIT0)
#define APP_UART0_DMA_TX_CH         (0U)
#define APP_UART0_DMA_RX_CH         (1U)

/* CH0 = UART0 TX */
#define APP_DMA_CH0_SRC_ADDR        ((uint32_t)s_uart0_dma_tx_buf)
#define APP_DMA_CH0_DST_ADDR        ((uint32_t)&APP_UART0_TX_DR)
#define APP_DMA_CH0_COUNT           (APP_UART0_DMA_TX_BUF_SIZE)

/* CH1 = UART0 RX */
#define APP_DMA_CH1_SRC_ADDR        ((uint32_t)&APP_UART0_RX_DR)
#define APP_DMA_CH1_DST_ADDR        ((uint32_t)s_uart0_dma_rx_ring)
#define APP_DMA_CH1_COUNT           (APP_UART0_DMA_RX_RING_SIZE)


typedef enum
{
    STATE_RX_INIT,              /*!< Receiver is in initial state. */
    STATE_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_RX_RCV,               /*!< Frame is beeing received. */
    STATE_RX_ERROR              /*!< If the frame is invalid. */
} eUARTRcvState;

typedef struct _uart_rx_manager_t
{
    eUARTRcvState e_rcv_state;
    uint16_t g_bufferPos;
    uint16_t g_bufferLastByte;
    uint8_t g_rx_buffer[APP_UART0_RX_BUF_SIZE];
	uint8_t g_uartrxbuf;
	uint8_t g_uartrxerr;
    uint8_t g_packet_data;
    uint8_t g_rcv_data_finish;

    uint32_t rx_err_total;
    uint32_t rx_err_overrun;
    uint32_t rx_irq_cnt;

    uint32_t rx_last_err_type;   /* optional: snapshot for debug */    
}UART_RX_MANAGER_T;

/* DMA transfer parameters. */
extern volatile uint8_t s_uart0_dma_rx_ring[APP_UART0_DMA_RX_RING_SIZE];
extern volatile uint8_t s_uart0_dma_tx_buf[APP_UART0_DMA_TX_BUF_SIZE];


#endif /* APP_UART0_CONFIG_H */

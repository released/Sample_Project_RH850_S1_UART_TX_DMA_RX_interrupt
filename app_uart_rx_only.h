#ifndef APP_UART0_RX_ONLY_H
#define APP_UART0_RX_ONLY_H

#include "r_cg_macrodriver.h"
#include "app_uart0_config.h"


// typedef enum
// {
//     STATE_RX_INIT,              /*!< Receiver is in initial state. */
//     STATE_RX_IDLE,              /*!< Receiver is in idle state. */
//     STATE_RX_RCV,               /*!< Frame is beeing received. */
//     STATE_RX_ERROR              /*!< If the frame is invalid. */
// } eUARTRcvState;

// typedef struct _uart_rx_manager_t
// {
//     eUARTRcvState e_rcv_state;
//     uint16_t g_bufferPos;
//     uint16_t g_bufferLastByte;
//     uint8_t g_rx_buffer[APP_UART0_RX_BUF_SIZE];
// 	uint8_t g_uartrxbuf;
// 	uint8_t g_uartrxerr;
//     uint8_t g_packet_data;
//     uint8_t g_rcv_data_finish;
// }UART_RX_MANAGER_T;

void APP_UART0_RX_callback_receiveend(void);
void APP_UART0_RX_callback_error(uint32_t err_type);

void APP_UART0_RX_Init(void);
void APP_UART0_RX_TimerIsr(void);
void APP_UART0_RX_Process(void);

#endif /* APP_UART0_RX_ONLY_H */

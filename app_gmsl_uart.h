#ifndef APP_GMSL_UART_H
#define APP_GMSL_UART_H

#include "r_cg_macrodriver.h"

/* GMSL UART Base Mode constants. */
#define GMSL_UART_SYNC_BYTE         (0x79U)
#define GMSL_UART_ACK_BYTE          (0xC3U)
/* Length field is 1 byte; 0x00 represents 256 bytes in base mode. */
#define GMSL_UART_MAX_DATA_LEN      (256U)

typedef enum
{
    GMSL_RESP_NONE = 0,
    GMSL_RESP_ACK,
    GMSL_RESP_DATA
} gmsl_resp_type_t;

/* Packet builders (no DMA side-effects). */
uint16_t APP_GMSL_BuildWritePacket(uint8_t dev_addr,
                                  uint8_t reg_addr,
                                  const uint8_t *data,
                                  uint16_t data_len,
                                  uint8_t *tx_buf,
                                  uint16_t tx_buf_size);
uint16_t APP_GMSL_BuildReadPacket(uint8_t dev_addr,
                                 uint8_t reg_addr,
                                 uint16_t read_len,
                                 uint8_t *tx_buf,
                                 uint16_t tx_buf_size);

/* RX expectation controls (used by RX-only). */
void APP_GMSL_ExpectAck(void);
void APP_GMSL_ExpectData(uint16_t len);
void APP_GMSL_ClearExpect(void);
uint8_t APP_GMSL_IsWaiting(void);

gmsl_resp_type_t APP_GMSL_GetExpect(void);
uint16_t APP_GMSL_GetExpectedLen(void);
void APP_GMSL_SetRecvLen(uint16_t len);
uint16_t APP_GMSL_GetRecvLen(void);
void APP_GMSL_SetAckOk(uint8_t ok);
uint8_t APP_GMSL_GetAckOk(void);
void APP_GMSL_SetError(uint8_t err);
uint8_t APP_GMSL_GetError(void);
void APP_GMSL_SetTimeout(uint8_t tmo);
uint8_t APP_GMSL_GetTimeout(void);

/* GMSL command demo TX process (uses UART0 DMA send). */
void APP_GMSL_TxProcess(void);

#endif /* APP_GMSL_UART_H */

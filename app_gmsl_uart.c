#include "r_smc_entry.h"
#include "misc_config.h"
#include "retarget.h"

#include "app_uart0_config.h"
#include "app_uart_dma.h"
#include "app_gmsl_uart.h"

typedef struct
{
    gmsl_resp_type_t expect;
    uint16_t expected_len;
    uint16_t recv_len;
    uint8_t ack_ok;
    uint8_t err;
    uint8_t tmo;
} gmsl_rx_ctx_t;

static volatile gmsl_rx_ctx_t s_gmsl_rx_ctx =
{
    .expect = GMSL_RESP_NONE,
    .expected_len = 0U,
    .recv_len = 0U,
    .ack_ok = 0U,
    .err = 0U,
    .tmo = 0U,
};

static void app_gmsl_rx_reset_ctx(void)
{
    s_gmsl_rx_ctx.expect = GMSL_RESP_NONE;
    s_gmsl_rx_ctx.expected_len = 0U;
    s_gmsl_rx_ctx.recv_len = 0U;
    s_gmsl_rx_ctx.ack_ok = 0U;
    s_gmsl_rx_ctx.err = 0U;
    s_gmsl_rx_ctx.tmo = 0U;
}

/* GMSL UART Base Mode helpers. */
static uint8_t app_gmsl_len_to_field(uint16_t len, uint8_t *len_field)
{
    if (len_field == NULL)
    {
        return 0U;
    }

    if (len == 0U)
    {
        return 0U;
    }

    if (len <= 255U)
    {
        *len_field = (uint8_t)len;
        return 1U;
    }

    if (len == 256U)
    {
        *len_field = 0x00U;
        return 1U;
    }

    return 0U;
}

/* GMSL UART Base Mode WRITE packet format (MCU -> GMSL):
 * [0] SYNC        = 0x79
 * [1] DEV_ADDR_RW = (dev_addr << 1) | 0
 * [2] REG_ADDR
 * [3] LEN         = N (1..255, 256 => 0x00)
 * [4..] DATA[0..N-1]
 *
 * Response (GMSL -> MCU): ACK 0xC3 only.
 */
uint16_t APP_GMSL_BuildWritePacket(uint8_t dev_addr,
                                  uint8_t reg_addr,
                                  const uint8_t *data,
                                  uint16_t data_len,
                                  uint8_t *tx_buf,
                                  uint16_t tx_buf_size)
{
    uint16_t total_len;
    uint16_t i;
    uint8_t len_field;

    if ((data == NULL) || (tx_buf == NULL))
    {
        return 0U;
    }

    if (app_gmsl_len_to_field(data_len, &len_field) == 0U)
    {
        return 0U;
    }

    total_len = (uint16_t)(4U + data_len);
    if (tx_buf_size < total_len)
    {
        return 0U;
    }

    tx_buf[0] = (uint8_t)GMSL_UART_SYNC_BYTE;
    tx_buf[1] = (uint8_t)((dev_addr << 1) | 0x00U); /* Write */
    tx_buf[2] = reg_addr;
    tx_buf[3] = len_field;

    for (i = 0U; i < data_len; i++)
    {
        tx_buf[4U + i] = data[i];
    }

    return total_len;
}

/* GMSL UART Base Mode READ request format (MCU -> GMSL):
 * [0] SYNC        = 0x79
 * [1] DEV_ADDR_RW = (dev_addr << 1) | 1
 * [2] REG_ADDR
 * [3] LEN         = N (1..255, 256 => 0x00)
 *
 * Response (GMSL -> MCU): DATA[0..N-1] only (no ACK).
 */
uint16_t APP_GMSL_BuildReadPacket(uint8_t dev_addr,
                                 uint8_t reg_addr,
                                 uint16_t read_len,
                                 uint8_t *tx_buf,
                                 uint16_t tx_buf_size)
{
    uint16_t total_len;
    uint8_t len_field;

    if (tx_buf == NULL)
    {
        return 0U;
    }

    if (app_gmsl_len_to_field(read_len, &len_field) == 0U)
    {
        return 0U;
    }

    total_len = 4U;
    if (tx_buf_size < total_len)
    {
        return 0U;
    }

    tx_buf[0] = (uint8_t)GMSL_UART_SYNC_BYTE;
    tx_buf[1] = (uint8_t)((dev_addr << 1) | 0x01U); /* Read */
    tx_buf[2] = reg_addr;
    tx_buf[3] = len_field;

    return total_len;
}

void APP_GMSL_ExpectAck(void)
{
    s_gmsl_rx_ctx.expect = GMSL_RESP_ACK;
    s_gmsl_rx_ctx.expected_len = 1U;
    s_gmsl_rx_ctx.recv_len = 0U;
    s_gmsl_rx_ctx.ack_ok = 0U;
    s_gmsl_rx_ctx.err = 0U;
    s_gmsl_rx_ctx.tmo = 0U;
}

void APP_GMSL_ExpectData(uint16_t len)
{
    if ((len == 0U) ||
        (len > (uint16_t)GMSL_UART_MAX_DATA_LEN) ||
        (len > (uint16_t)APP_UART0_RX_BUF_SIZE))
    {
        app_gmsl_rx_reset_ctx();
        s_gmsl_rx_ctx.err = 1U;
        return;
    }

    s_gmsl_rx_ctx.expect = GMSL_RESP_DATA;
    s_gmsl_rx_ctx.expected_len = len;
    s_gmsl_rx_ctx.recv_len = 0U;
    s_gmsl_rx_ctx.ack_ok = 0U;
    s_gmsl_rx_ctx.err = 0U;
    s_gmsl_rx_ctx.tmo = 0U;
}

void APP_GMSL_ClearExpect(void)
{
    app_gmsl_rx_reset_ctx();
}

uint8_t APP_GMSL_IsWaiting(void)
{
    return (s_gmsl_rx_ctx.expect == GMSL_RESP_NONE) ? 0U : 1U;
}

gmsl_resp_type_t APP_GMSL_GetExpect(void)
{
    return s_gmsl_rx_ctx.expect;
}

uint16_t APP_GMSL_GetExpectedLen(void)
{
    return s_gmsl_rx_ctx.expected_len;
}

void APP_GMSL_SetRecvLen(uint16_t len)
{
    s_gmsl_rx_ctx.recv_len = len;
}

uint16_t APP_GMSL_GetRecvLen(void)
{
    return s_gmsl_rx_ctx.recv_len;
}

void APP_GMSL_SetAckOk(uint8_t ok)
{
    s_gmsl_rx_ctx.ack_ok = ok;
}

uint8_t APP_GMSL_GetAckOk(void)
{
    return s_gmsl_rx_ctx.ack_ok;
}

void APP_GMSL_SetError(uint8_t err)
{
    s_gmsl_rx_ctx.err = err;
}

uint8_t APP_GMSL_GetError(void)
{
    return s_gmsl_rx_ctx.err;
}

void APP_GMSL_SetTimeout(uint8_t tmo)
{
    s_gmsl_rx_ctx.tmo = tmo;
}

uint8_t APP_GMSL_GetTimeout(void)
{
    return s_gmsl_rx_ctx.tmo;
}

/* ------------- Demo TX process (GMSL commands) ------------- */
/* DEMO: 7-bit device address (0x90/0x91 on wire => 0x48 here). */
#define GMSL_DEMO_DEV_ADDR          (0x48U)
/* DEMO registers (user must change to match target). */
#define GMSL_DEMO_WRITE_REG         (0xAAU)
#define GMSL_DEMO_READ_REG          (0xBBU)
/* Response wait ticks based on APP_GMSL_TxProcess call period (250ms). */
#define GMSL_DEMO_WAIT_TICKS        (4U)

/* DEMO lengths: write/read in this order. */
static const uint16_t s_gmsl_demo_len_list[] = { 8U, 16U, 32U, 48U, 64U, 128U };

static uint8_t s_gmsl_tx_payload[APP_UART0_DMA_TX_BUF_SIZE];

void APP_GMSL_TxProcess(void)
{
    uint16_t total_len;
    uint16_t data_len;
    uint16_t i;
    static uint8_t s_is_write_phase = 1U;
    static uint8_t s_len_idx = 0U;
    static uint8_t s_write_seed = 0U;
#if (APP_UART0_RX_MODE_DMA == 0U)
    static uint8_t s_wait_ticks = 0U;
#endif

#if (APP_UART0_RX_MODE_DMA == 0U)
    if (APP_GMSL_IsWaiting() != 0U)
    {
        if (s_wait_ticks > 0U)
        {
            s_wait_ticks--;
            return;
        }
        APP_GMSL_ClearExpect();
        tiny_printf("[GMSL RX] TIMEOUT\r\n");
        return;
    }
    s_wait_ticks = 0U;
#endif

    data_len = s_gmsl_demo_len_list[s_len_idx];
    if (data_len > (uint16_t)GMSL_UART_MAX_DATA_LEN)
    {
        data_len = (uint16_t)GMSL_UART_MAX_DATA_LEN;
    }

    if (s_is_write_phase != 0U)
    {
        tiny_printf("\r\n[GMSL TX] WRITE len=%u\r\n", data_len);
        /* Fill demo write data into TX buffer (no extra RAM buffer). */
        for (i = 0U; i < data_len; i++)
        {
            s_gmsl_tx_payload[4U + i] = (uint8_t)(s_write_seed + i);
        }
        s_write_seed++;

        total_len = APP_GMSL_BuildWritePacket(GMSL_DEMO_DEV_ADDR,
                                              GMSL_DEMO_WRITE_REG,
                                              (const uint8_t *)&s_gmsl_tx_payload[4U],
                                              data_len,
                                              (uint8_t *)s_gmsl_tx_payload,
                                              (uint16_t)sizeof(s_gmsl_tx_payload));
        if (total_len == 0U)
        {
            tiny_printf("gmsl tx:build write fail\r\n");
            return;
        }
#if (APP_UART0_RX_MODE_DMA == 0U)
        APP_GMSL_ExpectAck();
        s_wait_ticks = GMSL_DEMO_WAIT_TICKS;
#endif
        APP_UART0_DMA_TxSend((uint8_t *)s_gmsl_tx_payload, total_len);
    }
    else
    {
        tiny_printf("\r\n[GMSL TX] READ len=%u\r\n", data_len);
        total_len = APP_GMSL_BuildReadPacket(GMSL_DEMO_DEV_ADDR,
                                             GMSL_DEMO_READ_REG,
                                             data_len,
                                             (uint8_t *)s_gmsl_tx_payload,
                                             (uint16_t)sizeof(s_gmsl_tx_payload));
        if (total_len == 0U)
        {
            tiny_printf("gmsl tx:build read fail\r\n");
            return;
        }
#if (APP_UART0_RX_MODE_DMA == 0U)
        APP_GMSL_ExpectData(data_len);
        s_wait_ticks = GMSL_DEMO_WAIT_TICKS;
#endif
        APP_UART0_DMA_TxSend((uint8_t *)s_gmsl_tx_payload, total_len);
    }

    s_len_idx++;
    if (s_len_idx >= (uint8_t)(sizeof(s_gmsl_demo_len_list) / sizeof(s_gmsl_demo_len_list[0])))
    {
        s_len_idx = 0U;
        s_is_write_phase = (s_is_write_phase != 0U) ? 0U : 1U;
    }

    /* debug log */
    APP_UART0_DMA_TxDump((uint8_t *)s_gmsl_tx_payload, total_len);
}

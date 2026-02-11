#include "r_smc_entry.h"
#include "misc_config.h"
#include "retarget.h"

#include "app_uart_dma.h"
#include "app_uart0_config.h"
#include "app_gmsl_uart.h"

extern uint16_t g_tmr_compare_vaue;

static volatile UART_RX_MANAGER_T UART0_DMA_RX_Manager =
{
    .e_rcv_state        = STATE_RX_ERROR,
    .g_bufferPos        = 0U,
    .g_bufferLastByte   = 0U,
    .g_rx_buffer        = {0},
    .g_uartrxbuf        = 0U,
    .g_uartrxerr        = 0U,
    .g_packet_data      = 0U,
    .g_rcv_data_finish  = 0U,
    
    .rx_err_total       = 0U,
    .rx_err_overrun     = 0U,
    .rx_irq_cnt         = 0U,
};

static volatile uint32_t s_uart0_dma_rx_err_count;
static volatile uint32_t s_uart0_dma_rx_tc_count;
static uint16_t s_uart0_dma_rx_last_pos;
static uint16_t s_uart0_dma_gmsl_wait_ticks;

/* GMSL response timeout in TAUJ1 ticks (match RX-only wait ~1s). */
#define APP_UART0_DMA_GMSL_TMO_TICKS     (4608U)

/*
 * Usage example (UART0):
 *
 * init:
 *   APP_UART0_DMA_Init();
 *   APP_UART0_DMA_SetCh0(len);   // TX DMA (update tx buffer/count first)
 *   APP_UART0_DMA_SetCh1();   // RX DMA
 *
 * timer_isr (idle timeout, TAUJ1):
 *   APP_UART0_DMA_TimerIsr();
 *
 * dma_isr (TX channel):
 *   APP_UART0_DMA_TxIsr();
 *
 * dma_isr (RX channel):
 *   APP_UART0_DMA_RxIsr();   // clear flags (no ring reload)
 *
 * main loop:
 *   APP_UART0_DMA_Rx_process();
 */

static uint16_t app_uart0_dma_get_dtc(drv_dma_unit_t unit, uint8_t channel)
{
    volatile struct __tag494 *base;
    volatile uint32_t *dtc;
    drv_dma_offset_t offset0;
    drv_dma_offset_t stride;

    if (channel >= DRV_DMA_MAX_CHANNELS)
    {
        return 0U;
    }

    if (unit == DRV_DMA_UNIT0)
    {
        base = &PDMA0;
    }
#if defined(PDMA1)
    else if (unit == DRV_DMA_UNIT1)
    {
        base = &PDMA1;
    }
#endif
    else
    {
        return 0U;
    }

    offset0 = DRV_DMA_OFFSETOF(struct __tag494, DTC0);
    stride = DRV_DMA_OFFSETOF(struct __tag494, DTC1) - DRV_DMA_OFFSETOF(struct __tag494, DTC0);
    dtc = (volatile uint32_t *)((drv_dma_addr_t)base + offset0 + ((drv_dma_addr_t)stride * channel));

    return (uint16_t)(*dtc & 0xFFFFU);
}

/* TAUJ1_0 timer: idle-timeout (same flow as RX-only) */
static void app_uart0_dma_timer_reset(void)
{
    TAUJ1.CDR0 = g_tmr_compare_vaue;
    /* Reload Data Enable Register */
    TAUJ1.RDE = _PORT_SET_BIT0;
    /* Reload Data Trigger Register */
    TAUJ1.RDT = _PORT_SET_BIT0;
}

static void app_uart0_dma_timer_enable(void)
{
    app_uart0_dma_timer_reset();
    R_Config_TAUJ1_0_Start();
}

static void app_uart0_dma_timer_disable(void)
{
    R_Config_TAUJ1_0_Stop();
    app_uart0_dma_timer_reset();
}

static void app_uart0_dma_reset_state(void)
{
    UART0_DMA_RX_Manager.g_bufferPos = 0U;
    UART0_DMA_RX_Manager.g_bufferLastByte = 0U;
    UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_IDLE;
    s_uart0_dma_rx_last_pos = 0U;
    s_uart0_dma_gmsl_wait_ticks = 0U;
}

static void app_uart0_dma_stop_rx(void)
{
    R_Config_DMAC01_Suspend();
}

static void app_uart0_dma_restart_rx(void)
{
    (void)DRV_DMA_ClearTcAndErr(APP_UART0_DMA_UNIT, APP_UART0_DMA_RX_CH);
    (void)DRV_DMA_ReloadAndEnable(APP_UART0_DMA_UNIT,
                                  APP_UART0_DMA_RX_CH,
                                  APP_DMA_CH1_SRC_ADDR,
                                  APP_DMA_CH1_DST_ADDR,
                                  APP_DMA_CH1_COUNT);
    app_uart0_dma_reset_state();
    UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_INIT;
    app_uart0_dma_timer_enable();
}

void APP_UART0_DMA_Init(void)
{    
    reset_buffer((uint8_t *)s_uart0_dma_rx_ring, 0x00U, APP_UART0_DMA_RX_RING_SIZE);
    reset_buffer((uint8_t *)UART0_DMA_RX_Manager.g_rx_buffer, 0x00U, sizeof(UART0_DMA_RX_Manager.g_rx_buffer));

    UART0_DMA_RX_Manager.g_rcv_data_finish = 0U;
    app_uart0_dma_reset_state();
    UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_INIT;
    s_uart0_dma_rx_err_count = 0U;
    s_uart0_dma_rx_tc_count = 0U;
    s_uart0_dma_rx_last_pos = 0U;
}

void APP_UART0_DMA_TimerIsr(void)
{
    volatile uint16_t dma_remaining = app_uart0_dma_get_dtc(APP_UART0_DMA_UNIT, APP_UART0_DMA_RX_CH);
    volatile uint16_t write_pos;
    uint16_t expected_len;
    uint8_t expect;

    if (dma_remaining > APP_UART0_DMA_RX_RING_SIZE)
    {
        dma_remaining = APP_UART0_DMA_RX_RING_SIZE;
    }

    write_pos = (uint16_t)(APP_UART0_DMA_RX_RING_SIZE - dma_remaining);

    /* GMSL response handling (DMA RX). */
    if (APP_GMSL_IsWaiting() != 0U)
    {
        expect = (uint8_t)APP_GMSL_GetExpect();
        expected_len = APP_GMSL_GetExpectedLen();

        if (write_pos != s_uart0_dma_rx_last_pos)
        {
            s_uart0_dma_rx_last_pos = write_pos;
            s_uart0_dma_gmsl_wait_ticks = 0U;
            if ((UART0_DMA_RX_Manager.e_rcv_state == STATE_RX_INIT) ||
                (UART0_DMA_RX_Manager.e_rcv_state == STATE_RX_IDLE) ||
                (UART0_DMA_RX_Manager.e_rcv_state == STATE_RX_ERROR))
            {
                UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_RCV;
            }
        }
        else
        {
            if (s_uart0_dma_gmsl_wait_ticks < (uint16_t)APP_UART0_DMA_GMSL_TMO_TICKS)
            {
                s_uart0_dma_gmsl_wait_ticks++;
            }
        }

        if ((expect == (uint8_t)GMSL_RESP_ACK) && (write_pos >= 1U))
        {
            UART0_DMA_RX_Manager.g_bufferPos = 1U;
            UART0_DMA_RX_Manager.g_bufferLastByte = 0U;
            UART0_DMA_RX_Manager.g_rcv_data_finish = 1U;
            app_uart0_dma_timer_disable();
            app_uart0_dma_stop_rx();
            UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_IDLE;
            return;
        }

        if ((expect == (uint8_t)GMSL_RESP_DATA) &&
            (expected_len != 0U) &&
            (write_pos >= expected_len))
        {
            UART0_DMA_RX_Manager.g_bufferPos = expected_len;
            UART0_DMA_RX_Manager.g_bufferLastByte = (uint16_t)(expected_len - 1U);
            UART0_DMA_RX_Manager.g_rcv_data_finish = 1U;
            app_uart0_dma_timer_disable();
            app_uart0_dma_stop_rx();
            UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_IDLE;
            return;
        }

        if (s_uart0_dma_gmsl_wait_ticks >= (uint16_t)APP_UART0_DMA_GMSL_TMO_TICKS)
        {
            APP_GMSL_SetTimeout(1U);
            APP_GMSL_SetError(1U);
            UART0_DMA_RX_Manager.g_bufferPos = write_pos;
            UART0_DMA_RX_Manager.g_bufferLastByte = (write_pos == 0U) ? 0U : (uint16_t)(write_pos - 1U);
            UART0_DMA_RX_Manager.g_rcv_data_finish = 1U;
            app_uart0_dma_timer_disable();
            app_uart0_dma_stop_rx();
            UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_IDLE;
            return;
        }

        return;
    }

    if (write_pos != s_uart0_dma_rx_last_pos)
    {
        s_uart0_dma_rx_last_pos = write_pos;
        if ((UART0_DMA_RX_Manager.e_rcv_state == STATE_RX_INIT) ||
            (UART0_DMA_RX_Manager.e_rcv_state == STATE_RX_IDLE) ||
            (UART0_DMA_RX_Manager.e_rcv_state == STATE_RX_ERROR))
        {
            UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_RCV;
        }
        return;
    }

    if ((UART0_DMA_RX_Manager.e_rcv_state == STATE_RX_RCV) && (write_pos > 0U))
    {
        UART0_DMA_RX_Manager.g_bufferPos = write_pos;
        UART0_DMA_RX_Manager.g_bufferLastByte = (uint16_t)(write_pos - 1U);
        UART0_DMA_RX_Manager.g_rcv_data_finish = 1U;

        app_uart0_dma_timer_disable();
        app_uart0_dma_stop_rx();
        UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_IDLE;
    }
}

/*
    CH1
    uart0 rx
    src : rx dr         (count : fix)
    dest : ram buffer   (count : increase)
*/
void APP_UART0_DMA_SetCh1(void)
{
    (void)DRV_DMA_SetChannelEx(APP_UART0_DMA_UNIT,
                              APP_UART0_DMA_RX_CH,
                              APP_DMA_CH1_SRC_ADDR,
                              APP_DMA_CH1_DST_ADDR,
                              APP_DMA_CH1_COUNT);
    app_uart0_dma_timer_enable();
}

void APP_UART0_DMA_RxIsr(void)
{
    uint32_t cleared = 0U;

    /* RX ISR: clear flags, set done flag (no ring reload) */
    (void)DRV_DMA_ClearStatus(APP_UART0_DMA_UNIT,
                              APP_UART0_DMA_RX_CH,
                              (_DMAC_TRANSFER_COMPLETION_FLAG_CLEAR | _DMAC_TRANSFER_ERROR_FLAG_CLEAR),
                              &cleared);

    if ((cleared & _DMAC_TRANSFER_ERROR_FLAG_CLEAR) != 0U)
    {
        s_uart0_dma_rx_err_count++;
    }

    if ((cleared & _DMAC_TRANSFER_COMPLETION_FLAG_CLEAR) != 0U)
    {
        s_uart0_dma_rx_tc_count++;
        UART0_DMA_RX_Manager.g_bufferPos = APP_UART0_DMA_RX_RING_SIZE;
        UART0_DMA_RX_Manager.g_bufferLastByte = (uint16_t)(APP_UART0_DMA_RX_RING_SIZE - 1U);
        UART0_DMA_RX_Manager.g_rcv_data_finish = 1U;

        app_uart0_dma_timer_disable();
        app_uart0_dma_stop_rx();
        UART0_DMA_RX_Manager.e_rcv_state = STATE_RX_IDLE;
    }
}

uint8_t APP_UART0_DMA_RxDoneFlagGet(void)
{
    return UART0_DMA_RX_Manager.g_rcv_data_finish;
}

void APP_UART0_DMA_RxDoneFlagClear(void)
{
    UART0_DMA_RX_Manager.g_rcv_data_finish = 0U;
}

void APP_UART0_DMA_Rx_process(void)
{
    uint16_t len1;
    uint8_t expect;

    if (APP_UART0_DMA_RxDoneFlagGet() != 0U)
    {
        len1 = UART0_DMA_RX_Manager.g_bufferPos;
        if (len1 > 0U)
        {
            /* Copy to RX manager buffer for other modules usage */
            copy_buffer((uint8_t *)UART0_DMA_RX_Manager.g_rx_buffer,
                        (uint8_t *)s_uart0_dma_rx_ring,
                        len1);

            if (APP_GMSL_GetExpect() != GMSL_RESP_NONE)
            {
                expect = (uint8_t)APP_GMSL_GetExpect();
                if (APP_GMSL_GetError() != 0U)
                {
                    if (APP_GMSL_GetTimeout() != 0U)
                    {
                        tiny_printf("[GMSL RX DMA] TIMEOUT\r\n");
                    }
                    else
                    {
                        tiny_printf("gmsl rx:error\r\n");
                    }
                }
                else if (expect == (uint8_t)GMSL_RESP_ACK)
                {
                    if ((len1 >= 1U) &&
                        (UART0_DMA_RX_Manager.g_rx_buffer[0] == (uint8_t)GMSL_UART_ACK_BYTE))
                    {
                        APP_GMSL_SetAckOk(1U);
                        APP_GMSL_SetRecvLen(1U);
                        tiny_printf("[GMSL RX DMA] ACK\r\n");
                        tiny_printf("[GMSL RX DMA] ---- end write ----\r\n");
                    }
                    else
                    {
                        APP_GMSL_SetError(1U);
                        tiny_printf("gmsl rx:ack invalid\r\n");
                    }
                }
                else if (expect == (uint8_t)GMSL_RESP_DATA)
                {
                    APP_GMSL_SetRecvLen(len1);
                    tiny_printf("[GMSL RX DMA] DATA len=%u\r\n", len1);
                    dump_buffer8((uint8_t *)UART0_DMA_RX_Manager.g_rx_buffer, len1);
                    tiny_printf("[GMSL RX DMA] ---- end read ----\r\n");
                }

                APP_GMSL_ClearExpect();
            }
            else
            {
                tiny_printf("uart rx:buffer\r\n");
                dump_buffer8((uint8_t *)UART0_DMA_RX_Manager.g_rx_buffer, len1);
            }

            reset_buffer((uint8_t *)UART0_DMA_RX_Manager.g_rx_buffer, 0x00U, len1);
        }

        APP_UART0_DMA_RxDoneFlagClear();
        app_uart0_dma_restart_rx();
    }

    if (s_uart0_dma_rx_err_count != 0U)
    {
        tiny_printf("UART0_DMA RX ERR=%lu, TC=%lu\r\n",
                    (unsigned long)s_uart0_dma_rx_err_count,
                    (unsigned long)s_uart0_dma_rx_tc_count);
        s_uart0_dma_rx_err_count = 0U;
    }
}

void APP_UART0_DMA_RxDebugDump(void)
{
    /* PDMA0 RX (CH1) debug: read-only dump */
    #if 0
    tiny_printf("DEBUG(RX CH1):\r\n");
    tiny_printf("DTCT1:0x%08X,", PDMA0.DTCT1);
    tiny_printf("DSA1:0x%08X,", PDMA0.DSA1);
    tiny_printf("DDA1:0x%08X,", PDMA0.DDA1);
    tiny_printf("DTC1:0x%08X,", PDMA0.DTC1);
    tiny_printf("DCEN1:0x%08X,", PDMA0.DCEN1);
    tiny_printf("DCST1:0x%08X,", PDMA0.DCST1);
    tiny_printf("DMACER:0x%08X,", PDMA0.DMACER);
    tiny_printf("DM01CM:0x%08X,", PDMA0.DM01CM);
    tiny_printf("\r\n\r\n");
    #endif
}

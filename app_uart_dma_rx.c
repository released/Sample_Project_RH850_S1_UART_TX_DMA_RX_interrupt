#include "r_smc_entry.h"
#include "retarget.h"

#include "app_uart_dma.h"
#include "app_uart0_config.h"

static uart_dma_rx_t s_uart0_dma_rx;
static volatile uint8_t s_uart0_dma_rx_done;
static volatile uint32_t s_uart0_dma_rx_err_count;
static volatile uint32_t s_uart0_dma_rx_tc_count;

/*
 * Usage example (UART0):
 *
 * init:
 *   APP_UART0_DMA_Init();
 *   APP_UART0_DMA_SetCh0();   // TX DMA (update tx buffer/count first)
 *   APP_UART0_DMA_SetCh1();   // RX DMA
 *
 * timer_isr (periodic, suggested 10us for 415000 baud):
 *   APP_UART0_DMA_TimerIsr();
 *
 * dma_isr (TX channel):
 *   APP_UART0_DMA_TxIsr();
 *
 * dma_isr (RX channel):
 *   APP_UART0_DMA_RxIsr();   // clear flags + reload (no done flag here)
 *
 * main loop:
 *   if (APP_UART0_DMA_RxDoneFlagGet())
 *   {
 *       const uint8_t *p1; const uint8_t *p2; uint16_t l1, l2;
 *       if (APP_UART0_DMA_PeekFrame(&p1, &l1, &p2, &l2) > 0U)
 *       {
 *           // decode/print buffer (example)
 *           // for(uint16_t i=0;i<l1;i++) printf("%02X ", p1[i]);
 *           // for(uint16_t i=0;i<l2;i++) printf("%02X ", p2[i]);
 *           APP_UART0_DMA_ConsumeFrame();
 *       }
 *       APP_UART0_DMA_RxDoneFlagClear();
 *   }
 *
 * Idle timeout recommendation:
 * - Idle bits = N bits, time = N / baud.
 * - For 8N1, one char ~= 10 bits. A common choice is 3 chars = 30 bits.
 *   Example (30 bits):
 *     baud 115200 -> 260.4 us (timer tick >= 100us ok)
 *     baud 415000 -> 72.3 us (suggest 10us tick, idle_ticks ~ 8)
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

void APP_UART0_DMA_Init(void)
{    
    uint16_t idle_ticks = UART_DMA_IdleTicksFromBaud(APP_UART0_DMA_BAUD,
                                                     APP_UART0_DMA_TIMER_HZ,
                                                     APP_UART0_DMA_IDLE_BITS);

    UART_DMA_Rx_Init(&s_uart0_dma_rx, (uint8_t *) s_uart0_dma_rx_ring, APP_UART0_DMA_RX_RING_SIZE, idle_ticks);
}

void APP_UART0_DMA_TimerIsr(void)
{
    uint16_t dma_remaining = app_uart0_dma_get_dtc(APP_UART0_DMA_UNIT, APP_UART0_DMA_RX_CH);
    if (dma_remaining == 0U)
    {
        /* If TC occurred but ISR hasn't reloaded yet, treat as wrapped to 0. */
        dma_remaining = APP_UART0_DMA_RX_RING_SIZE;
    }
    if (dma_remaining > APP_UART0_DMA_RX_RING_SIZE)
    {
        dma_remaining = APP_UART0_DMA_RX_RING_SIZE;
    }

    UART_DMA_Rx_OnTimerTick(&s_uart0_dma_rx, dma_remaining);

    /* Set done flag when idle timeout produces a complete frame */
    if (s_uart0_dma_rx_done == 0U)
    {
        const uint8_t *p1;
        const uint8_t *p2;
        uint16_t l1;
        uint16_t l2;
        if (UART_DMA_Rx_PeekFrame(&s_uart0_dma_rx, &p1, &l1, &p2, &l2) > 0U)
        {
            s_uart0_dma_rx_done = 1U;
        }
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
}

void APP_UART0_DMA_RxIsr(void)
{
    uint32_t cleared = 0U;

    /* RX ISR: clear flags, set done flag, then reload */
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
        (void)DRV_DMA_ReloadAndEnable(APP_UART0_DMA_UNIT,
                                      APP_UART0_DMA_RX_CH,
                                      APP_DMA_CH1_SRC_ADDR,
                                      APP_DMA_CH1_DST_ADDR,
                                      APP_DMA_CH1_COUNT);
    }
}

uint8_t APP_UART0_DMA_RxDoneFlagGet(void)
{
    return s_uart0_dma_rx_done;
}

void APP_UART0_DMA_RxDoneFlagClear(void)
{
    s_uart0_dma_rx_done = 0U;
}

uint16_t APP_UART0_DMA_PeekFrame(const uint8_t **p1,
                                 uint16_t *len1,
                                 const uint8_t **p2,
                                 uint16_t *len2)
{
    return UART_DMA_Rx_PeekFrame(&s_uart0_dma_rx, p1, len1, p2, len2);
}

void APP_UART0_DMA_ConsumeFrame(void)
{
    UART_DMA_Rx_ConsumeFrame(&s_uart0_dma_rx);
}

void APP_UART0_DMA_Rx_process(void)
{
    const uint8_t *p1; 
    const uint8_t *p2; 
    uint16_t len1, len2;
    uint8_t first_byte = 0U;
    uint8_t last_byte = 0U;

    if (APP_UART0_DMA_RxDoneFlagGet() != 0U)
    {
        if (APP_UART0_DMA_PeekFrame(&p1, &len1, &p2, &len2) > 0U)
        {
            /*
                packet first byte : p1[0]
                packet last byte
                if len2 > 0
                    (p2[len2 - 1])
                else
                    (p1[len1 - 1])            
            */
            first_byte = p1[0];
            last_byte = (len2 > 0) ? p2[len2-1] : p1[len1-1];

            
            //debug log
            APP_UART0_DMA_RxDump(first_byte,
                                last_byte,
                                p1,
                                len1,
                                p2,
                                len2);
            
            APP_UART0_DMA_ConsumeFrame();
        }
        APP_UART0_DMA_RxDoneFlagClear();
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

void APP_UART0_DMA_RxDump(uint8_t first_byte, 
                            uint16_t last_byte,
                            const uint8_t *p1,
                            uint16_t len1,
                            const uint8_t *p2,
                            uint16_t len2)
{
    #if 1
    uint16_t i=0;

    tiny_printf("UART0_DMA RX decode\r\n");
    tiny_printf("[first]:0x%02X,",first_byte);
    tiny_printf("[last]:0x%02X,",last_byte);

    #if 1
    for(i=0;i<len1;i++) 
    {
        tiny_printf("%02X,", p1[i]);
    }

    for(i=0;i<len2;i++) 
    {
        tiny_printf("%02X,", p2[i]);
    }
    #else
    tiny_printf("len1:0x%02X,",len1);
    tiny_printf("len2:0x%02X,",len2);
    #endif
            
    tiny_printf("\r\n\r\n");
    #endif
}


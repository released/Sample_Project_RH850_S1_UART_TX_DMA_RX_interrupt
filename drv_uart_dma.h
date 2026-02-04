#ifndef DRV_UART_DMA_H
#define DRV_UART_DMA_H

#ifndef NULL
#define NULL ((void *)0)
#endif

/*
 * UART DMA RX helper (ring buffer + idle timeout).
 * Detailed usage example is in app_uart_dma.c to keep this driver portable.
 */

typedef struct
{
    uint8_t *ring;
    uint16_t size;
    uint16_t last_pos;
    uint16_t frame_start;
    uint16_t frame_end;
    uint16_t frame_len;
    uint16_t idle_ticks;
    uint16_t idle_cnt;
    uint8_t frame_ready;
} uart_dma_rx_t;

/* Call once during init (before enabling DMA RX). */
void UART_DMA_Rx_Init(uart_dma_rx_t *ctx, uint8_t *ring, uint16_t size, uint16_t idle_ticks);

/* Use when baud/timer changes. */
void UART_DMA_Rx_SetIdleTicks(uart_dma_rx_t *ctx, uint16_t idle_ticks);

/*
 * Call from timer ISR.
 * dma_remaining: current DMA remaining count (DTC) for RX channel.
 */
void UART_DMA_Rx_OnTimerTick(uart_dma_rx_t *ctx, uint16_t dma_remaining);

/*
 * Call in main loop (consumer side).
 * Returns total frame length. If frame_ready == 0, returns 0.
 * If wrap occurs, returns two segments: [p1, len1] + [p2, len2].
 */
uint16_t UART_DMA_Rx_PeekFrame(const uart_dma_rx_t *ctx,
                               const uint8_t **p1,
                               uint16_t *len1,
                               const uint8_t **p2,
                               uint16_t *len2);

/* Call in main loop after you finish processing a frame. */
void UART_DMA_Rx_ConsumeFrame(uart_dma_rx_t *ctx);

/*
 * Use during setup to compute idle ticks from baud/timer.
 * Helper: compute idle ticks from baud/timer.
 * idle_bits: e.g. 30 bits (3 chars @ 8N1).
 * timer_hz:  e.g. 1000 for 1 ms tick.
 */
uint16_t UART_DMA_IdleTicksFromBaud(uint32_t baud, uint32_t timer_hz, uint16_t idle_bits);

#endif /* DRV_UART_DMA_H */

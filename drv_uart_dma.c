
#include "r_smc_entry.h"

#include "drv_uart_dma.h"

static uint16_t uart_dma_ring_distance(uint16_t start, uint16_t end, uint16_t size)
{
    if (end >= start)
    {
        return (uint16_t)(end - start);
    }
    return (uint16_t)(size - start + end);
}

void UART_DMA_Rx_Init(uart_dma_rx_t *ctx, uint8_t *ring, uint16_t size, uint16_t idle_ticks)
{
    if ((ctx == 0) || (ring == 0) || (size == 0U))
    {
        return;
    }

    ctx->ring = ring;
    ctx->size = size;
    ctx->last_pos = 0U;
    ctx->frame_start = 0U;
    ctx->frame_end = 0U;
    ctx->frame_len = 0U;
    ctx->idle_ticks = idle_ticks;
    ctx->idle_cnt = 0U;
    ctx->frame_ready = 0U;
}

/* Use when baud/timer changes. */
void UART_DMA_Rx_SetIdleTicks(uart_dma_rx_t *ctx, uint16_t idle_ticks)
{
    if (ctx == 0)
    {
        return;
    }

    ctx->idle_ticks = idle_ticks;
}

/* Call from timer ISR (idle timeout detection). */
void UART_DMA_Rx_OnTimerTick(uart_dma_rx_t *ctx, uint16_t dma_remaining)
{
    uint16_t write_pos;

    if ((ctx == 0) || (ctx->size == 0U))
    {
        return;
    }

    if (ctx->frame_ready != 0U)
    {
        return;
    }

    if (dma_remaining > ctx->size)
    {
        dma_remaining = ctx->size;
    }

    write_pos = (uint16_t)(ctx->size - dma_remaining);

    if (write_pos != ctx->last_pos)
    {
        ctx->last_pos = write_pos;
        ctx->idle_cnt = 0U;
        return;
    }

    if (ctx->idle_cnt < ctx->idle_ticks)
    {
        ctx->idle_cnt++;
    }

    if (ctx->idle_cnt >= ctx->idle_ticks)
    {
        uint16_t len = uart_dma_ring_distance(ctx->frame_start, write_pos, ctx->size);
        if (len != 0U)
        {
            ctx->frame_end = write_pos;
            ctx->frame_len = len;
            ctx->frame_ready = 1U;
        }
        ctx->idle_cnt = 0U;
    }
}

/* Call in main loop to access a completed frame. */
uint16_t UART_DMA_Rx_PeekFrame(const uart_dma_rx_t *ctx,
                               const uint8_t **p1,
                               uint16_t *len1,
                               const uint8_t **p2,
                               uint16_t *len2)
{
    uint16_t start;
    uint16_t end;

    if ((ctx == 0) || (ctx->frame_ready == 0U) || (ctx->frame_len == 0U))
    {
        if (p1 != 0) { *p1 = 0; }
        if (p2 != 0) { *p2 = 0; }
        if (len1 != 0) { *len1 = 0; }
        if (len2 != 0) { *len2 = 0; }
        return 0U;
    }

    start = ctx->frame_start;
    end = ctx->frame_end;

    if (end >= start)
    {
        if (p1 != 0) { *p1 = &ctx->ring[start]; }
        if (len1 != 0) { *len1 = (uint16_t)(end - start); }
        if (p2 != 0) { *p2 = 0; }
        if (len2 != 0) { *len2 = 0; }
    }
    else
    {
        if (p1 != 0) { *p1 = &ctx->ring[start]; }
        if (len1 != 0) { *len1 = (uint16_t)(ctx->size - start); }
        if (p2 != 0) { *p2 = &ctx->ring[0]; }
        if (len2 != 0) { *len2 = end; }
    }

    return ctx->frame_len;
}

/* Call in main loop after processing the frame. */
void UART_DMA_Rx_ConsumeFrame(uart_dma_rx_t *ctx)
{
    if ((ctx == 0) || (ctx->frame_ready == 0U))
    {
        return;
    }

    ctx->frame_start = ctx->frame_end;
    ctx->frame_end = ctx->frame_start;
    ctx->frame_len = 0U;
    ctx->frame_ready = 0U;
}

/* Use during setup to compute idle ticks from baud/timer. */
uint16_t UART_DMA_IdleTicksFromBaud(uint32_t baud, uint32_t timer_hz, uint16_t idle_bits)
{
    uint64_t numerator;
    uint32_t ticks;

    if ((baud == 0U) || (timer_hz == 0U) || (idle_bits == 0U))
    {
        return 0U;
    }

    numerator = (uint64_t)idle_bits * (uint64_t)timer_hz;
    ticks = (uint32_t)((numerator + baud - 1U) / baud);
    if (ticks > 0xFFFFU)
    {
        ticks = 0xFFFFU;
    }
    return (uint16_t)ticks;
}

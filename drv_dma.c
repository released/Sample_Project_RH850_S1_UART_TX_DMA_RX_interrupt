
#include "r_smc_entry.h"

#include "drv_dma.h"

extern volatile uint32_t g_cg_sync_read;

#define DRV_DMA_OFF_DSA0   (DRV_DMA_OFFSETOF(struct __tag494, DSA0))
#define DRV_DMA_OFF_DDA0   (DRV_DMA_OFFSETOF(struct __tag494, DDA0))
#define DRV_DMA_OFF_DTC0   (DRV_DMA_OFFSETOF(struct __tag494, DTC0))
#define DRV_DMA_OFF_DCEN0  (DRV_DMA_OFFSETOF(struct __tag494, DCEN0))
#define DRV_DMA_OFF_DCST0  (DRV_DMA_OFFSETOF(struct __tag494, DCST0))
#define DRV_DMA_OFF_DCSTC0 (DRV_DMA_OFFSETOF(struct __tag494, DCSTC0))

#define DRV_DMA_DSA_STRIDE   (DRV_DMA_OFFSETOF(struct __tag494, DSA1) - DRV_DMA_OFFSETOF(struct __tag494, DSA0))
#define DRV_DMA_DDA_STRIDE   (DRV_DMA_OFFSETOF(struct __tag494, DDA1) - DRV_DMA_OFFSETOF(struct __tag494, DDA0))
#define DRV_DMA_DTC_STRIDE   (DRV_DMA_OFFSETOF(struct __tag494, DTC1) - DRV_DMA_OFFSETOF(struct __tag494, DTC0))
#define DRV_DMA_DCEN_STRIDE  (DRV_DMA_OFFSETOF(struct __tag494, DCEN1) - DRV_DMA_OFFSETOF(struct __tag494, DCEN0))
#define DRV_DMA_DCST_STRIDE  (DRV_DMA_OFFSETOF(struct __tag494, DCST1) - DRV_DMA_OFFSETOF(struct __tag494, DCST0))
#define DRV_DMA_DCSTC_STRIDE (DRV_DMA_OFFSETOF(struct __tag494, DCSTC1) - DRV_DMA_OFFSETOF(struct __tag494, DCSTC0))

static volatile struct __tag494 *drv_dma_base(drv_dma_unit_t unit)
{
    if (unit == DRV_DMA_UNIT0)
    {
        return &PDMA0;
    }
#if defined(PDMA1)
    if (unit == DRV_DMA_UNIT1)
    {
        return &PDMA1;
    }
#endif
    return (volatile struct __tag494 *)0;
}

static volatile uint32_t *drv_dma_reg_ptr(volatile struct __tag494 *base,
                                          drv_dma_offset_t offset0,
                                          drv_dma_offset_t stride,
                                          uint8_t channel)
{
    return (volatile uint32_t *)((drv_dma_addr_t)base + offset0 + ((drv_dma_addr_t)stride * channel));
}

/* Use in init code or before starting DMA.
 * UART RX: source can be RLN3n.LURDR.UINT8[0]
 * UART TX: destination can be RLN3n.LUTDR.UINT8[0]
 * This avoids hard-coded absolute addresses (e.g. 0xFFCE2024 / 0xFFCE2026).
 */
drv_dma_status_t DRV_DMA_SetChannelEx(drv_dma_unit_t unit,
                                      uint8_t channel,
                                      uint32_t source_address,
                                      uint32_t destination_address,
                                      uint32_t transfer_count)
{
    volatile struct __tag494 *base;
    volatile uint32_t *dsa;
    volatile uint32_t *dda;
    volatile uint32_t *dtc;
    volatile uint32_t *dcen;

    if (channel >= DRV_DMA_MAX_CHANNELS)
    {
        return DRV_DMA_STATUS_INVALID_CHANNEL;
    }

    base = drv_dma_base(unit);
    if (base == 0)
    {
        return DRV_DMA_STATUS_INVALID_CHANNEL;
    }

    dsa = drv_dma_reg_ptr(base, DRV_DMA_OFF_DSA0, DRV_DMA_DSA_STRIDE, channel);
    dda = drv_dma_reg_ptr(base, DRV_DMA_OFF_DDA0, DRV_DMA_DDA_STRIDE, channel);
    dtc = drv_dma_reg_ptr(base, DRV_DMA_OFF_DTC0, DRV_DMA_DTC_STRIDE, channel);
    dcen = drv_dma_reg_ptr(base, DRV_DMA_OFF_DCEN0, DRV_DMA_DCEN_STRIDE, channel);

    /* Ensure channel is disabled before updating transfer settings. */
    *dcen = _DMAC_CHANNEL_OPERATION_DISABLED;

    *dsa = source_address;
    *dda = destination_address;
    *dtc = transfer_count;

    /* Synchronization processing */
    g_cg_sync_read = *dtc;
    __syncp();

    return DRV_DMA_STATUS_OK;
}

/* Default unit = DMAC0. */
drv_dma_status_t DRV_DMA_SetChannel(uint8_t channel,
                                    uint32_t source_address,
                                    uint32_t destination_address,
                                    uint32_t transfer_count)
{
    return DRV_DMA_SetChannelEx(DRV_DMA_UNIT0, channel, source_address, destination_address, transfer_count);
}

/* Use in DMA ISR to clear flags. */
drv_dma_status_t DRV_DMA_ClearStatus(drv_dma_unit_t unit,
                                     uint8_t channel,
                                     uint32_t clear_mask,
                                     uint32_t *cleared_mask)
{
    volatile struct __tag494 *base;
    volatile uint32_t *dcst;
    volatile uint32_t *dcstc;
    uint32_t status;
    uint32_t to_clear;

    if (channel >= DRV_DMA_MAX_CHANNELS)
    {
        return DRV_DMA_STATUS_INVALID_CHANNEL;
    }

    base = drv_dma_base(unit);
    if (base == 0)
    {
        return DRV_DMA_STATUS_INVALID_CHANNEL;
    }

    dcst = drv_dma_reg_ptr(base, DRV_DMA_OFF_DCST0, DRV_DMA_DCST_STRIDE, channel);
    dcstc = drv_dma_reg_ptr(base, DRV_DMA_OFF_DCSTC0, DRV_DMA_DCSTC_STRIDE, channel);

    status = *dcst;
    to_clear = status & clear_mask;
    if (to_clear != 0U)
    {
        *dcstc = to_clear;
        g_cg_sync_read = *dcstc;
        __syncp();
    }

    if (cleared_mask != NULL)
    {
        *cleared_mask = to_clear;
    }

    return DRV_DMA_STATUS_OK;
}

/* Use in DMA ISR (common case). */
drv_dma_status_t DRV_DMA_ClearTcAndErr(drv_dma_unit_t unit, uint8_t channel)
{
    return DRV_DMA_ClearStatus(unit,
                               channel,
                               (_DMAC_TRANSFER_COMPLETION_FLAG_CLEAR | _DMAC_TRANSFER_ERROR_FLAG_CLEAR),
                               NULL);
}

/* Use when you want to reload buffer and re-enable channel. */
drv_dma_status_t DRV_DMA_ReloadAndEnable(drv_dma_unit_t unit,
                                        uint8_t channel,
                                        uint32_t source_address,
                                        uint32_t destination_address,
                                        uint32_t transfer_count)
{
    volatile struct __tag494 *base;
    volatile uint32_t *dcen;
    drv_dma_status_t status;

    status = DRV_DMA_SetChannelEx(unit, channel, source_address, destination_address, transfer_count);
    if (status != DRV_DMA_STATUS_OK)
    {
        return status;
    }

    base = drv_dma_base(unit);
    if (base == 0)
    {
        return DRV_DMA_STATUS_INVALID_CHANNEL;
    }

    dcen = drv_dma_reg_ptr(base, DRV_DMA_OFF_DCEN0, DRV_DMA_DCEN_STRIDE, channel);
    *dcen = _DMAC_CHANNEL_OPERATION_ENABLED;

    /* Synchronization processing */
    g_cg_sync_read = *dcen;
    __syncp();

    return DRV_DMA_STATUS_OK;
}

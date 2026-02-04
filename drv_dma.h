#ifndef DRV_DMA_H
#define DRV_DMA_H

#ifndef NULL
#define NULL ((void *)0)
#endif

/*
 * Set DMAC channel transfer parameters without editing auto-generated files.
 * Call these from R_Config_DMAC00_Create_UserInit / R_Config_DMAC01_Create_UserInit.
 */
typedef unsigned long drv_dma_addr_t;
typedef unsigned long drv_dma_offset_t;

#ifndef DRV_DMA_OFFSETOF
#define DRV_DMA_OFFSETOF(type, member) ((drv_dma_offset_t)(&(((type *)0)->member)))
#endif

#ifndef DRV_DMA_MAX_CHANNELS
/* Override per project, e.g. -DDRV_DMA_MAX_CHANNELS=32 */
#define DRV_DMA_MAX_CHANNELS (16U)
#endif

typedef enum
{
    DRV_DMA_STATUS_OK = 0U,
    DRV_DMA_STATUS_INVALID_CHANNEL = 1U
} drv_dma_status_t;

typedef enum
{
    DRV_DMA_UNIT0 = 0U,
    DRV_DMA_UNIT1 = 1U
} drv_dma_unit_t;

/*
 * Use in init code or before starting DMA.
 * unit: DRV_DMA_UNIT0/1, channel: 0..(DRV_DMA_MAX_CHANNELS-1)
 */
drv_dma_status_t DRV_DMA_SetChannelEx(drv_dma_unit_t unit,
                                      uint8_t channel,
                                      uint32_t source_address,
                                      uint32_t destination_address,
                                      uint32_t transfer_count);

/* Same as SetChannelEx, default unit = DRV_DMA_UNIT0. */
drv_dma_status_t DRV_DMA_SetChannel(uint8_t channel,
                                    uint32_t source_address,
                                    uint32_t destination_address,
                                    uint32_t transfer_count);

/*
 * Use in DMA ISR to clear flags (e.g. completion/error).
 * clear_mask: OR of _DMAC_TRANSFER_COMPLETION_FLAG_CLEAR / _DMAC_TRANSFER_ERROR_FLAG_CLEAR
 */
drv_dma_status_t DRV_DMA_ClearStatus(drv_dma_unit_t unit,
                                     uint8_t channel,
                                     uint32_t clear_mask,
                                     uint32_t *cleared_mask);

/* Use in DMA ISR (common case). Clears completion + error flags. */
drv_dma_status_t DRV_DMA_ClearTcAndErr(drv_dma_unit_t unit, uint8_t channel);

/*
 * Use when you want to reload buffer and re-enable channel
 * (e.g. in DMA RX ISR for ring buffer).
 */
drv_dma_status_t DRV_DMA_ReloadAndEnable(drv_dma_unit_t unit,
                                        uint8_t channel,
                                        uint32_t source_address,
                                        uint32_t destination_address,
                                        uint32_t transfer_count);

#endif /* DRV_DMA_H */

#include "r_smc_entry.h"
#include "misc_config.h"
#include "retarget.h"

#include "app_uart_dma.h"
#include "app_uart0_config.h"
#include "app_gmsl_uart.h"

/* DMA busy: software flag set on TxSend, cleared in DMA TX ISR. */
static volatile uint8_t s_uart0_dma_tx_busy = 0;
static volatile uint16_t s_uart0_dma_tx_len = 0;

#pragma section dma_buf
volatile uint8_t s_uart0_dma_rx_ring[APP_UART0_DMA_RX_RING_SIZE];
volatile uint8_t s_uart0_dma_tx_buf[APP_UART0_DMA_TX_BUF_SIZE];
#pragma section default


/* UART busy: hardware status (RLN30.LST UTS). */
#define UART0_TX_IS_BUSY()          ((RLN30.LST & _UART_TRANSMISSION_OPERATED) != 0U)
#define UART0_TX_IS_IDLE()          ((RLN30.LST & _UART_TRANSMISSION_OPERATED) == 0U)

/*
    RLN30.LST FTC
    0: UART buffer transmission has not been completed. 
    1: UART buffer transmission has been completed. 
*/
#define UART0_TX_LAST_DONE()        ((RLN30.LST & _UART_TRANSMISSION_COMPLETED) != 0U)

/*
    CH0
    uart0 tx
    src : ram buffer    (count : increase)
    dest : tx dr        (count : fix)
*/
void APP_UART0_DMA_SetCh0(uint16_t len)
{
    s_uart0_dma_tx_len = len;
    (void)DRV_DMA_SetChannelEx(APP_UART0_DMA_UNIT,
                              APP_UART0_DMA_TX_CH,
                              APP_DMA_CH0_SRC_ADDR,
                              APP_DMA_CH0_DST_ADDR,
                              s_uart0_dma_tx_len);
}

void APP_UART0_DMA_TxIsr(void)
{
    /* TX ISR: only clear DMA completion/error flags */
    (void)DRV_DMA_ClearTcAndErr(APP_UART0_DMA_UNIT, APP_UART0_DMA_TX_CH);
    s_uart0_dma_tx_busy = 0U;

    #if 0
    tiny_printf("uart tx dma isr\r\n");
    #endif
}

void APP_UART0_PEG_Set(void)
{
    /*
        Enable PEG access permission 
        When accessing from the DMA to the local RAM and the retention RAM in the PE, the 
        PEGSP register, 
        PEGGnMK register (n = 0 to 3), 
        PEGGnBA register (n = 0 to 3) and 
        PDMAnDMyiCM register (yi = 00 to 07, 10 to 17) must be set
    */   
    PDMA0.DM00CM = _DMAC_PE1_SETTING | _DMAC_SPID0_SETTING | _DMAC_SUPERVISON_MODE;    
    PDMA0.DM01CM = _DMAC_PE1_SETTING | _DMAC_SPID0_SETTING | _DMAC_SUPERVISON_MODE;
    PEG.SP.UINT32 = 0x00000001U;                /* SPEN = 1, permit access */
    PEG.G0MK.UINT32 = 0xFFFFF000U;              /* 32KB window mask */
    PEG.G0BA.UINT32 = ADDR_LOCAL_RAM_CPU1 |     /* Base address of PE Guard protection Area (start of Local RAM) */
                    (0x1U<<7U) |                /* Enable Access for SPID 3 */
                    (0x1U<<6U) |                /* Enable Access for SPID 2 */
                    (0x1U<<5U) |                /* Enable Access for SPID 1 */
                    (0x1U<<4U) |                /* Enable Access for SPID 0 */
                    (0x1U<<2U) |                /* Write access is enabled */
                    (0x1U<<1U) |                /* Read access is enabled */
                    (0x1U<<0U);                 /* Settings for access enable conditions are enabled */

    #if 0
    tiny_printf("DEBUG(SP):\r\n");
    tiny_printf("SP:0x%08X,",PEG.SP);
    tiny_printf("G0MK:0x%08X,",PEG.G0MK);
    tiny_printf("G0BA:0x%08X,",PEG.G0BA);
    tiny_printf("\r\n\r\n");
    #endif

}

static uint16_t app_uart0_dma_txbuf_write(const uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint16_t copy_len;

    if (data == NULL)
    {
        return 0U;
    }

    copy_len = len;
    if (copy_len > APP_UART0_DMA_TX_BUF_SIZE)
    {
        copy_len = APP_UART0_DMA_TX_BUF_SIZE;
    }

    // tiny_printf("len1:%u\r\n", len);
    // tiny_printf("copy_len1:%u\r\n", copy_len);
    for (i = 0U; i < copy_len; i++)
    {
        s_uart0_dma_tx_buf[i] = data[i];
    }

    /* Update DMA count to actual payload length */
    s_uart0_dma_tx_len = copy_len;
    return copy_len;
}

void APP_UART0_DMA_TxSend(const uint8_t *data, uint16_t len)
{
    uint16_t copy_len;
 
    // memset(s_uart0_dma_tx_buf, 0xAA, sizeof(s_uart0_dma_tx_buf));


    if ((data == 0) || (len == 0U))
    {
        return;
    }

    while (UART0_TX_IS_BUSY())
    {
        /* wait until TX can accept data */
    }

    R_Config_DMAC00_Suspend();
    R_Config_DMAC00_Stop();
    (void)DRV_DMA_ClearTcAndErr(APP_UART0_DMA_UNIT, APP_UART0_DMA_TX_CH);

    copy_len = app_uart0_dma_txbuf_write(data, len);
    if (copy_len == 0U)
    {
        return;
    }
    // tiny_printf("len2:%u\r\n", len);
    // tiny_printf("copy_len2:%u\r\n", copy_len);

    APP_UART0_DMA_SetCh0(copy_len);
                              
    s_uart0_dma_tx_busy = 1U;
    R_Config_DMAC00_Resume();
    R_Config_DMAC00_Start();

    APP_UART0_TX_DR = s_uart0_dma_tx_buf[0];

    // while (! (PDMA0.DCST0 & _DMAC_TRANSFER_COMPLETION_FLAG_CLEAR));
    while (s_uart0_dma_tx_busy != 0U)
    {
        /* optional: add timeout / feed WDT */
    }
}


void APP_UART0_DMA_TxDump(const uint8_t *pucBuff, uint16_t nBytes)
{
    #if 1
    uint16_t i = 0U;

    if (nBytes == 0U)
    {
        return;
    }
        
    tiny_printf("UART0_DMA TX:%2d\r\n" , nBytes);    

    dump_buffer8((uint8_t *) pucBuff , nBytes);	

    reset_buffer((uint8_t*) pucBuff, 0x00U , nBytes);   


    // for (i = 0U ; i < nBytes ; i++)
    // {
    //     tiny_printf("0x%02X," , pucBuff[i]);
    //     if ((i+1U)%8U ==0U)
    //     {
    //         tiny_printf("\r\n");
    //     }            
    // }
    // tiny_printf("\r\n\r\n");
    #endif
}

void APP_UART0_DMA_Tx_process(void)
{
    APP_GMSL_TxProcess();
}



void APP_UART0_DMA_DebugDump(void)
{   
    /* PDMA0 debug */
    #if 0
    tiny_printf("DEBUG(PDMA0):\r\n");
    tiny_printf("DTCT0:0x%08X,",PDMA0.DTCT0);
    tiny_printf("DSA0:0x%08X,",PDMA0.DSA0);
    tiny_printf("DDA0:0x%08X,",PDMA0.DDA0);
    tiny_printf("DTC0:0x%08X,",PDMA0.DTC0);
    tiny_printf("DCEN0:0x%08X,",PDMA0.DCEN0);
    tiny_printf("DCST0:0x%08X,",PDMA0.DCST0);
    tiny_printf("DMACER:0x%08X,",PDMA0.DMACER);     
    tiny_printf("DM00CM:0x%08X,",PDMA0.DM00CM);

    if (PDMA0.DMACER == 1)
    {
        tiny_printf("FLAG:0x%08X,",SEG.FLAG);
        tiny_printf("ADDR:0x%08X,",SEG.ADDR);
    }

    tiny_printf("\r\n\r\n");
    #endif

    /* IPG debug */
    #if 0
    tiny_printf("IPG: ECRUM=0x%04X ADRUM=0x%08lX ENUM=0x%02X PMTUM0=0x%02X PMTUM2=0x%02X PMTUM3=0x%02X PMTUM4=0x%02X\r\n",
                IPG.ECRUM.UINT16,
                IPG.ADRUM.UINT32,
                IPG.ENUM,
                IPG.PMTUM0,
                IPG.PMTUM2,
                IPG.PMTUM3,
                IPG.PMTUM4);

    /* ERRSLV03 debug */
    tiny_printf("ERRSLV03: STAT=0x%08lX ADDR=0x%08lX TYPE=0x%08lX\r\n",
                PBG30.ERRSLV03STAT.UINT32,
                PBG30.ERRSLV03ADDR,
                PBG30.ERRSLV03TYPE.UINT32);

    /* ERRSLV05 debug */
    tiny_printf("ERRSLV04: STAT=0x%08lX ADDR=0x%08lX TYPE=0x%08lX\r\n",
                PBG31.ERRSLV04STAT.UINT32,
                PBG31.ERRSLV04ADDR,
                PBG31.ERRSLV04TYPE.UINT32);

    tiny_printf("\r\n\r\n");
    #endif

}

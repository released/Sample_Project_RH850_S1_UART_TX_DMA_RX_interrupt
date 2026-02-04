# Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt
Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt

update @ 2026/02/04

1. base on EVM : Y-BLDC-SK-RH850F1KM-S1-V2 (R7F701684/LQFP100/1MB flash) , initial below function

	- project MCU change to R7F7016923 (LQFP64/512K flash)

	- TAUJ0_0 : timer interval for 1ms interrupt

	- UART1 : RLIN31 (TX > P10_12 , RX > P10_11) , for printf and receive from keyboard

	- TAUJ1_0 : timer interval for XXX us interrupt , to detect UART1 RX idle bytes
	
		- baud rate 115200 : 217us

		- baud rate 415000 : 60.24us

	- UART0 : RLIN30 (TX > P10_10 , RX > P10_09) , for printf and receive from keyboard

2. before use DMA , need to know

	- need to set __PEG__ register , before DMA to access SRAM normally

	- need to enable PEGSP register, PEGGnMK register, PEGGnBA register , PDMAnDMyiCM register 

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/PEG_access.jpg)


about __PEGSP__ register 

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/PEGSP.jpg)



about __PEGGnMK__ register 

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/PEGGnMK.jpg)


about __PDMAnDMyiCM__ register 

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/Identifiers_for_Slave_Guard_RH850_F1KM_S1.jpg)


about __PEGGnBA__ register 

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/PEGGnBA_1.jpg)

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/PEGGnBA_2.jpg)

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/PEGGnBA_3.jpg)
 

```c
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

```

```c
/* LOCAL_RAM_CPU1 start address (PEG) */
#define ADDR_LOCAL_RAM_CPU1         (0xFEBF0000)
```

3. SRAM setting 

memory map (RH850/F1KM-S1)

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/Memory_Map_F1KM_S1.jpg)


| FLASH  | LOCAL RAM(CPU1) | RETENTION RAM(CPU1) |LOCAL RAM(SELF) | RETENTION RAM(SELF) |
|--------|----------------|--------------------|---------------|--------------------|
| 512K   | __0xFEBF0000__ ~ 0xFEBF7FFF | 0xFEBF8000 ~ 0xFEBFFFFF | __0xFEDF0000__ ~ 0xFEDF7FFF| 0xFEDF8000 ~ 0xFEDFFFFF|
| 768K   | __0xFEBE8000__ ~ 0xFEBF7FFF | 0xFEBF8000 ~ 0xFEBFFFFF | __0xFEDE8000__ ~ 0xFEDF7FFF| 0xFEDF8000 ~ 0xFEDFFFFF|
| 1MB    | __0xFEBE0000__ ~ 0xFEBF7FFF | 0xFEBF8000 ~ 0xFEBFFFFF | __0xFEDE0000__ ~ 0xFEDF7FFF| 0xFEDF8000 ~ 0xFEDFFFFF|


CPU1 area vs Self area

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/CPU1_area_Self_area_RH850_F1KM_S1.jpg)

DMA access area

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/Address_Space_Viewed_from_Bus_Master_S1_1M_1.jpg)

need to allocate SRAM section : dma_buff `0xFEBF0000` (FLASH : 512K)

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/cs_link_options_section_settings.jpg)


define TX , RX DMA ram buffer in dma_buf section

```c
#pragma section dma_buf
volatile uint8_t s_uart0_dma_rx_ring[APP_UART0_DMA_RX_RING_SIZE];
volatile uint8_t s_uart0_dma_tx_buf[APP_UART0_DMA_TX_BUF_SIZE];
#pragma section default
```

the DMA ram buffer allocation result in map file

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/map_dma_tx_rx_buffer_addr.jpg)


UART DMA TX , RX CONFIG 

| Item | DMA source address | DMA source address count direction |DMA dest. address|DMA dest. address count direction|
|------|--------------------|------------------------------------|-----------------|---------------------------------|
| TX |s_uart0_dma_tx_buf|increase|APP_UART0_TX_DR |fix|
| RX |APP_UART0_RX_DR |fix|s_uart0_dma_rx_ring|increase|


4. DMA address 

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/register_base_DMA0.jpg)

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/DMA_src_dest_addr.jpg)

5. UART address

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/register_base_RLIN3x.jpg)

TX : RLN3nLUTDR

RX : RLN3nLURDR

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/RLIN3n_register_tx_rx.jpg)

6. Smart config setting

target trigger source : UART0

INTRLIN30UR0 : RLIN30 transmit interrupt 

INTRLIN30UR1 : RLIN30 receive complete interrupt 

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/DMA_trig_source_uart.jpg)

TX : 

src : set in code (s_uart0_dma_tx_buf)

dest : set in code (APP_UART0_TX_DR)

src count : increase

dest count : fixed

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/smc_DMA00_config_tx.jpg)


RX : 

src : set in code (APP_UART0_RX_DR)

dest : set in code (s_uart0_dma_rx_buf)

src count : fixed

dest count : increase


![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/smc_DMA01_config_rx.jpg)

7. due to UART function have no idle interrupt , need to use timer for idle detection (target : 2.5 bytes)

```
        for uart rx idle timer isr : 
        8N1 , 1 byte = 10 bits
        target : 2.5 bytes = 25 bits
        target_isr_timing
        - baud rate 115200 = (25 / 115200)*10^6 = 217.0us
        - baud rate 415000 = (25 / 415000)*10^6 = 60.24us
        
        timer_freq ​= PCLK(80MHz) /prescaler (1)
        CDR0 = (timer_freq * target_isr_timing/10^6) - 1
        - baud rate 115200 217.0us = (80MHz * 217us/10^6) - 1 = 80 * 217 - 1 = 0x43CF
        - baud rate 415000 60.24us = (80MHz * 60.24us/10^6) - 1 = 80 * 60.24 - 1 = 0x12D3    
```

```c
// #define APP_UART0_BAUD              (415000U)
#define APP_UART0_BAUD              (115200U)


#if (APP_UART0_BAUD == 115200U)
    TAUJ1.CDR0 = 0x43CFU;   /* 217 us = 2.5 bytes @115200 */
#elif (APP_UART0_BAUD == 415000U)
    TAUJ1.CDR0 = 0x12D3U;   /* 60.24 us = 2.5 bytes @415000 */
#else
    #error "Unsupported APP_UART0_BAUD"
#endif
```
below is RX TIMER IDLE detction flow

```mermaid
flowchart TD
    A[APP_UART0_RX_Init] --> B[Reset RX buffer & state]
    B --> C[UART0 Receive 1 byte enable]
    C --> D[UART0 Start]
    D --> E[STATE = RX_INIT]
    E --> F[Start t2.5 Timer]

    %% UART RX interrupt
    F -->|UART RX IRQ| G[APP_UART0_RX_callback_receiveend]
    G --> H[Store received byte]
    H --> I[_prvvUARTRxISR]

    %% State machine
    I -->|STATE_RX_INIT| J[Enable Timer]
    I -->|STATE_RX_ERROR| J
    I -->|STATE_RX_IDLE| K[bufferPos=0]
    K --> L[Store first byte]
    L --> M[STATE = RX_RCV]
    M --> J

    I -->|STATE_RX_RCV| N{bufferPos < BUF_SIZE?}
    N -->|Yes| O[Store byte]
    O --> J
    N -->|No| P[STATE = RX_ERROR]
    P --> Q[Reset state]

    %% Timer expiry (IDLE detected)
    J -->|Timer expired| R[APP_UART0_RX_TimerIsr]
    R --> S[_prvvTIMERExpiredISR]
    S --> T{STATE == RX_RCV?}
    T -->|Yes| U[UART Stop]
    U --> V[g_rcv_data_finish = 1]
    T -->|No| W[Ignore]
    V --> X[STATE = RX_IDLE]

    %% Main loop process
    X --> Y[APP_UART0_RX_Process]
    Y -->|g_rcv_data_finish| Z[Dump RX buffer]
    Z --> AA[Clear buffer]
    AA --> AB[Reset state]
    AB --> AC[UART Start]
```

8. if need to check variable in watch windows , need to modify

enable [Access during the execution] at Debugger Property > Debug Tool Settings tab

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/watch_windows.jpg)

9. below is log message :

log : when __send__ mult-len tx data

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/log_tx.jpg)

log : when __receive__ 128 bytes rx data ( connect tx and rx wire)

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/log_rx_128_1.jpg)
![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/log_rx_128_2.jpg)


log : when __receive__ 256 bytes rx data( connect tx and rx wire)

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/log_rx_256.jpg)

waveform : send tx per 250ms

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/scope_period_send_250ms.jpg)


waveform : send tx with 8 bytes

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/scope_tx_8.jpg)

waveform : send tx with 16 bytes

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/scope_tx_16.jpg)

waveform : send tx with 24 bytes

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/scope_tx_24_1.jpg)
![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/scope_tx_24_2.jpg)

waveform : send tx with 64 bytes

![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/scope_tx_64_1.jpg)
![image](https://github.com/released/Sample_Project_RH850_S1_UART_TX_DMA_RX_interrupt/blob/main/scope_tx_64_2.jpg)

/*_____ I N C L U D E S ____________________________________________________*/
// #include <stdio.h>
#include <string.h>
#include "r_smc_entry.h"

#include "misc_config.h"
#include "custom_func.h"
#include "retarget.h"

#include "drv_dma.h"
#include "app_gmsl_uart.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

volatile struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_TIMER_PERIOD_SPECIFIC           	    (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_REVERSE2                 	        	(flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                    		    (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


#define FLAG_PROJ_TRIG_1                                (flag_PROJ_CTL.bit8)
#define FLAG_PROJ_TRIG_2                                (flag_PROJ_CTL.bit9)
#define FLAG_PROJ_TRIG_3                                (flag_PROJ_CTL.bit10)
#define FLAG_PROJ_TRIG_4                                (flag_PROJ_CTL.bit11)
#define FLAG_PROJ_TRIG_5                                (flag_PROJ_CTL.bit12)
#define FLAG_PROJ_REVERSE13                             (flag_PROJ_CTL.bit13)
#define FLAG_PROJ_REVERSE14                             (flag_PROJ_CTL.bit14)
#define FLAG_PROJ_REVERSE15                             (flag_PROJ_CTL.bit15)

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned short counter_tick = 0U;
volatile unsigned long ostmr_tick = 0U;

#define BTN_PRESSED_LONG                                (2500U)

#pragma section privateData

const unsigned char dummy_3 = 0x5AU;

volatile unsigned char dummy_2 = 0xFFU;

volatile unsigned char dummy_1;

#pragma section default

volatile unsigned long g_u32_counter = 0U;

volatile UART_MANAGER_T UART1Manager = 
{
	.g_uartrxbuf = 0U,                                         /* UARTx receive buffer */
	.g_uartrxerr = 0U,                                         /* UARTx receive error status */
};

uint16_t g_tmr_compare_vaue = 0UL;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/



void ostmr_tick_counter(void)
{
	ostmr_tick++;
}

void ostmr_1ms_IRQ(void)
{
	ostmr_tick_counter();
}

void ostimer_dealyms(unsigned long ms)
{
    R_Config_OSTM0_Start();
    ostmr_tick = 0U;

    while(ostmr_tick < ms);

    R_Config_OSTM0_Stop();

}

unsigned short get_tick(void)
{
	return (counter_tick);
}

void set_tick(unsigned short t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000U)
    {
        set_tick(0U);
    }
}

void delay_ms(unsigned long ms)
{
    unsigned long tickstart = get_tick();
    unsigned long wait = ms;
	unsigned long tmp = 0U;
	
    while (1)
    {
		if (get_tick() > tickstart)	// tickstart = 59000 , tick_counter = 60000
		{
			tmp = get_tick() - tickstart;
		}
		else // tickstart = 59000 , tick_counter = 2048
		{
			tmp = 60000U -  tickstart + get_tick();
		}		
		
		if (tmp > wait)
			break;
    }
}

/* round-up to avoid early idle */
static uint16_t calc_tauj_cdr0_from_baud(uint32_t baud)
{
    /* TAUJ PCLK */
    const uint32_t pclk_hz = 80000000UL;

    /* 2.5 bytes @8N1 => 25 bits */
    const uint32_t idle_bits = 25UL;

    uint64_t num;
    uint32_t ticks;

    if (baud == 0UL)
    {
        return 0U;
    }

    /* ticks = round(pclk_hz * idle_bits / baud) */
    num = (uint64_t)pclk_hz * (uint64_t)idle_bits + (uint64_t)(baud / 2UL);
    ticks = (uint32_t)(num / (uint64_t)baud);

    if (ticks == 0UL)
    {
        ticks = 1UL;
    }

    return (uint16_t)(ticks - 1UL);
}


unsigned char R_PORT_GetGPIOLevel(unsigned short n,unsigned char Pin)
{
    unsigned short PortLevel;

    switch(n)
    {
        case 0U:
            PortLevel = PORT.PPR0;
            break;
        case 8U:
            PortLevel = PORT.PPR8;
            break;
        case 9U:
            PortLevel = PORT.PPR9;
            break;
        case 10U:
            PortLevel = PORT.PPR10;
            break;
        case 11U:
            PortLevel = PORT.PPR11;
            break;
        case 0x2C8U:
            PortLevel = PORT.APPR0;
            break;
    }
    PortLevel &= 1U<<Pin;
    
    if(PortLevel == 0U)
    {
        return 0U;
    }
    else
    {
        return 1U;
    }
}
void tmr_1ms_IRQ(void)
{
    tick_counter();

    if ((get_tick() % 1000U) == 0U)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 1U;
    }

    if ((get_tick() % 250U) == 0U)
    {
        FLAG_PROJ_TIMER_PERIOD_SPECIFIC = 1U;
    }

    if ((get_tick() % 50U) == 0U)
    {

    }	

}

void LED_Toggle(void)
{
    static unsigned char flag_gpio = 0U;
		
    GPIO_TOGGLE(0,14);//PORT.PNOT0 |= 1u<<14;
	
	if (!flag_gpio)
	{
		flag_gpio = 1U;
        GPIO_HIGH(P8,5);//PORT.P8 |= 1u<<5;
	}
	else
	{
		flag_gpio = 0U;
		GPIO_LOW(P8,5);//PORT.P8 &= ~(1u<<5);
	}	
}

void loop(void)
{
	// static unsigned long LOG1 = 0U;

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0U;

        g_u32_counter++;
        LED_Toggle();   
        // tiny_printf("timer:%4d\r\n",LOG1++);

        APP_UART0_DMA_DebugDump();
        
        #if (APP_UART0_RX_MODE_DMA != 0U)
        APP_UART0_DMA_RxDebugDump();
        #endif
    }

    if (FLAG_PROJ_TIMER_PERIOD_SPECIFIC)
    {
        FLAG_PROJ_TIMER_PERIOD_SPECIFIC = 0U;

        APP_GMSL_TxProcess();
        
        // R_Config_UART0_Send("1234",4);  // for test UART hardware
    }

    #if (APP_UART0_RX_MODE_DMA != 0U)
    APP_UART0_DMA_Rx_process();
    #else
    APP_UART0_RX_Process();
    #endif
}

void UARTx_ErrorCheckProcess(unsigned char err)
{
    if (err)          /* Check reception error */
    {   
        /* Reception error */
        switch(err)
        {
            case _UART_PARITY_ERROR_FLAG:   /* Parity error */
                tiny_printf("uart rx:Parity Error Flag\r\n");
                break;
            case _UART_FRAMING_ERROR_FLAG:  /* Framing error */
                tiny_printf("uart rx:Framing Error Flag\r\n");
                break;
            case _UART_OVERRUN_ERROR_FLAG:  /* Overrun error */
                tiny_printf("uart rx:Overrun Error Flag\r\n");
                break;
            case _UART_BIT_ERROR_FLAG:      /* Bit error */
                tiny_printf("uart rx:Bit Error Flag\r\n");
                break;
        }
        UART1Manager.g_uartrxerr = 0U;
    }
}

void UARTx_Process(unsigned char rxbuf)
{    
    if (rxbuf == 0x00U)
    {
        return;
    }

    if (rxbuf > 0x7FU)
    {
        tiny_printf("invalid command\r\n");
    }
    else
    {
        tiny_printf("press:%c(0x%02X)\r\n" , rxbuf,rxbuf);   // %c :  C99 libraries.
        switch(rxbuf)
        {
            case '1':
                break;

            case 'X':
            case 'x':
            case 'Z':
            case 'z':
                RH850_software_reset();
                break;

            default:       
                // exception
                break;                
        }
    }
}

void RH850_software_reset(void)
{
    unsigned long  reg32_value;

    reg32_value = 0x00000001UL;
    WPROTR.PROTCMD0 = _WRITE_PROTECT_COMMAND;
    RESCTL.SWRESA = reg32_value;
    RESCTL.SWRESA = (unsigned long) ~reg32_value;
    RESCTL.SWRESA = reg32_value;
    while (WPROTR.PROTS0 != reg32_value)
    {
        NOP();
    }
}

void RLIN31_UART_SendChar(unsigned char c)
{
    /*
        UTS : 0 - transmission is not in progress    
    */
    while (((RLN31.LST & _UART_TRANSMISSION_OPERATED) != 0U));    
    RLN31.LUTDR.UINT16 = c;
    // RLN31.LUTDR.UINT8[L] = (unsigned char) c;  
}

void SendChar(unsigned char ch)
{
    RLIN31_UART_SendChar(ch);
}

void hardware_init(void)
{
    EI();

    R_Config_TAUJ0_0_Start();
    R_Config_OSTM0_Start();

    /*
        LED : 
            - LED18 > P0_14
            - LED17 > P8_5 
        UART : (printf)
            - TX > P10_12
            - RX > P10_11    
    */
    R_Config_UART1_Receive((uint8_t *)&UART1Manager.g_uartrxbuf, 1U);
    R_Config_UART1_Start();

    /*
        UART0 : (transmit , baud rate : 115200 , 415000)
        - TX > P10_10
        - RX > P10_09    
    */
    APP_UART0_PEG_Set();
    // APP_UART0_DMA_SetCh0(0U); // uart tx dma register init (len set at send)
    #if (APP_UART0_RX_MODE_DMA != 0U)
    APP_UART0_DMA_Init();
    APP_UART0_DMA_SetCh1();
    #else
    APP_UART0_RX_Init();
    #endif

    // after UART TX , RX DMA parameter init ready , init low level driver
    R_Config_DMAC00_Start();
    #if (APP_UART0_RX_MODE_DMA != 0U)
    R_Config_DMAC01_Start();
    #endif

    // init uart0
    R_Config_UART0_Start();    
    R_Config_UART0_Send("\r",1);
    #if (APP_UART0_RX_MODE_DMA != 0U)
    /* Disable UART0 RX interrupt when using DMA RX (avoid double-read). */
    INTC2.ICRLIN30UR1.BIT.MKRLIN30UR1 = _INT_PROCESSING_DISABLED;
    INTC2.ICRLIN30UR1.BIT.RFRLIN30UR1 = _INT_REQUEST_NOT_OCCUR;
    #endif
    
    /*
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
    */
    #if 1   // formula calculation
    g_tmr_compare_vaue = calc_tauj_cdr0_from_baud(APP_UART0_BAUD);
    tiny_printf("g_tmr_compare_vaue:%lu,0x%8lX\r\n",g_tmr_compare_vaue,g_tmr_compare_vaue);
    TAUJ1.CDR0 = g_tmr_compare_vaue; 
    #else   // hardcode
#if (APP_UART0_BAUD == 115200U)
    TAUJ1.CDR0 = 0x43CFU;   /* 217 us = 2.5 bytes @115200 */
#elif (APP_UART0_BAUD == 415000U)
    TAUJ1.CDR0 = 0x12D3U;   /* 60.24 us = 2.5 bytes @415000 */
#else
    #error "Unsupported APP_UART0_BAUD"
#endif
    #endif


    // R_Config_TAUJ1_0_Start();

   
    tiny_printf("\r\nhardware_init rdy\r\n");

}

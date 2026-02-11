
#include "r_smc_entry.h"
#include "misc_config.h"
#include "retarget.h"

#include "app_uart_rx_only.h"
#include "app_gmsl_uart.h"

extern uint16_t g_tmr_compare_vaue;

volatile UART_RX_MANAGER_T UART0_RX_Manager = 
{
    .e_rcv_state        = STATE_RX_ERROR,
    .g_bufferPos        = 0U,
    .g_bufferLastByte   = 0U,
    .g_rx_buffer        = {0},
	.g_uartrxbuf        = 0U,  /* UARTx receive buffer */
	.g_uartrxerr        = 0U,  /* UARTx receive error status */
    .g_packet_data      = 0U,
    .g_rcv_data_finish  = 0U,
};

/*TAUJ1_0 timer*/
void _vComPortResetCounter(void)
{
    TAUJ1.CDR0 = g_tmr_compare_vaue;
    // Reload Data Enable Register
    TAUJ1.RDE = _PORT_SET_BIT0;//1U << 0U;    
    // Reload Data Trigger Register
    TAUJ1.RDT = _PORT_SET_BIT0;//1U << 0U; 
}

void _vComPortTimersEnable(void)
{
    _vComPortResetCounter();
    R_Config_TAUJ1_0_Start();
}

void _vComPortTimersDisable(void)
{
    R_Config_TAUJ1_0_Stop();
    _vComPortResetCounter();
}

void _vComPortResetState(void)
{
	UART0_RX_Manager.g_bufferPos = 0U;					
	UART0_RX_Manager.e_rcv_state = STATE_RX_IDLE;
    APP_GMSL_ClearExpect();
}

void _prvvTIMERExpiredISR(void)
{
    if (APP_GMSL_IsWaiting() != 0U)
    {
        /* For GMSL responses, use the higher-level wait/timeout logic. */
        _vComPortTimersDisable();
        return;
    }

	UART0_RX_Manager.g_bufferLastByte = UART0_RX_Manager.g_bufferPos - 1U;

    /*
        buffer condition check
    */

	if (UART0_RX_Manager.e_rcv_state == STATE_RX_RCV)
	{
		R_Config_UART0_Stop();

		UART0_RX_Manager.g_rcv_data_finish = 1U;	
	}

	_vComPortTimersDisable();   
	UART0_RX_Manager.e_rcv_state = STATE_RX_IDLE;

    if (APP_GMSL_IsWaiting() != 0U)
    {
        APP_GMSL_SetTimeout(1U);
        APP_GMSL_SetError(1U);
    }
}

void _prvvUARTRxISR(void)
{
    uint8_t ucByte = 0UL;
    ucByte = UART0_RX_Manager.g_packet_data;

    switch ( UART0_RX_Manager.e_rcv_state )
    {
        case STATE_RX_INIT:
            _vComPortTimersEnable();
            break;

        case STATE_RX_ERROR:
            _vComPortTimersEnable();
            break;

        case STATE_RX_IDLE:	
            UART0_RX_Manager.g_bufferPos = 0U;
            UART0_RX_Manager.g_rx_buffer[UART0_RX_Manager.g_bufferPos++] = ucByte;
			UART0_RX_Manager.e_rcv_state = STATE_RX_RCV;
			/* Enable t3.5 timers. */
			_vComPortTimersEnable();

            if (APP_GMSL_GetExpect() == GMSL_RESP_ACK)
            {
                if (ucByte == (uint8_t)GMSL_UART_ACK_BYTE)
                {
                    APP_GMSL_SetAckOk(1U);
                    APP_GMSL_SetRecvLen(1U);
                    UART0_RX_Manager.g_rcv_data_finish = 1U;
                    R_Config_UART0_Stop();
                    _vComPortTimersDisable();
                    UART0_RX_Manager.e_rcv_state = STATE_RX_IDLE;
                }
                else
                {
                    /* Ignore unexpected byte while waiting for ACK. */
                    UART0_RX_Manager.g_bufferPos = 0U;
                }
            }
            else if ((APP_GMSL_GetExpect() == GMSL_RESP_DATA) &&
                     (APP_GMSL_GetExpectedLen() != 0U) &&
                     (UART0_RX_Manager.g_bufferPos >= APP_GMSL_GetExpectedLen()))
            {
                APP_GMSL_SetRecvLen(UART0_RX_Manager.g_bufferPos);
                UART0_RX_Manager.g_rcv_data_finish = 1U;
                R_Config_UART0_Stop();
                _vComPortTimersDisable();
                UART0_RX_Manager.e_rcv_state = STATE_RX_IDLE;
            }
            break;

        case STATE_RX_RCV:
            if( UART0_RX_Manager.g_bufferPos < APP_UART0_RX_BUF_SIZE )
            {					
                UART0_RX_Manager.g_rx_buffer[UART0_RX_Manager.g_bufferPos++] = ucByte;
            }
            else
            {
				UART0_RX_Manager.g_bufferPos = 0U;	
                UART0_RX_Manager.e_rcv_state = STATE_RX_ERROR;
				_vComPortResetState();			
            }
            _vComPortTimersEnable();

            if (APP_GMSL_GetExpect() == GMSL_RESP_ACK)
            {
                if (ucByte == (uint8_t)GMSL_UART_ACK_BYTE)
                {
                    APP_GMSL_SetAckOk(1U);
                    APP_GMSL_SetRecvLen(1U);
                    UART0_RX_Manager.g_rcv_data_finish = 1U;
                    R_Config_UART0_Stop();
                    _vComPortTimersDisable();
                    UART0_RX_Manager.e_rcv_state = STATE_RX_IDLE;
                }
                else
                {
                    /* Ignore unexpected byte while waiting for ACK. */
                    UART0_RX_Manager.g_bufferPos = 0U;
                }
            }
            else if ((APP_GMSL_GetExpect() == GMSL_RESP_DATA) &&
                     (APP_GMSL_GetExpectedLen() != 0U) &&
                     (UART0_RX_Manager.g_bufferPos >= APP_GMSL_GetExpectedLen()))
            {
                APP_GMSL_SetRecvLen(UART0_RX_Manager.g_bufferPos);
                UART0_RX_Manager.g_rcv_data_finish = 1U;
                R_Config_UART0_Stop();
                _vComPortTimersDisable();
                UART0_RX_Manager.e_rcv_state = STATE_RX_IDLE;
            }
            break;
    }
}

void APP_UART0_RX_callback_receiveend(void)
{
    UART0_RX_Manager.g_uartrxerr = 0U;          /* Clear error flag */
    UART0_RX_Manager.g_packet_data = UART0_RX_Manager.g_uartrxbuf;
    _prvvUARTRxISR();
    R_Config_UART0_Receive((uint8_t *)&UART0_RX_Manager.g_uartrxbuf, 1U);   
}

void APP_UART0_RX_callback_error(uint32_t err_type)
{
    UART0_RX_Manager.g_uartrxerr = err_type;    /* Get error type */

    if (err_type)          /* Check reception error */
    {   
        /* Reception error */
        switch(err_type)
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

        if (APP_GMSL_IsWaiting() != 0U)
        {
            /* Treat RX errors during GMSL wait as a timeout to avoid silent drops. */
            APP_GMSL_SetTimeout(1U);
            APP_GMSL_SetError(1U);
            UART0_RX_Manager.g_rcv_data_finish = 1U;
            R_Config_UART0_Stop();
            _vComPortTimersDisable();
            UART0_RX_Manager.e_rcv_state = STATE_RX_IDLE;
        }
        UART0_RX_Manager.g_uartrxerr = 0U;
    }
}

void APP_UART0_RX_Init(void)
{
    reset_buffer((uint8_t*) UART0_RX_Manager.g_rx_buffer, 0x00U, sizeof(UART0_RX_Manager.g_rx_buffer));	

	_vComPortResetState();

    R_Config_UART0_Receive((uint8_t *)&UART0_RX_Manager.g_uartrxbuf, 1U);
    R_Config_UART0_Start();

    UART0_RX_Manager.e_rcv_state = STATE_RX_INIT;

    // set 217us interrupt
    _vComPortTimersEnable();
}

void APP_UART0_RX_TimerIsr(void)
{
    _prvvTIMERExpiredISR();
}

void APP_UART0_RX_Process(void)
{
	if (UART0_RX_Manager.g_rcv_data_finish)
	{
		UART0_RX_Manager.g_rcv_data_finish  = 0U; 

        if (UART0_RX_Manager.g_bufferPos == 0)
        {

            _vComPortResetState(); 	            
            R_Config_UART0_Start();	
            return;
        }

        if (APP_GMSL_GetExpect() != GMSL_RESP_NONE)
        {
            if (APP_GMSL_GetError() != 0U)
            {
                if (APP_GMSL_GetTimeout() != 0U)
                {
                    tiny_printf("[GMSL RX] TIMEOUT\r\n");
                }
                else
                {
                    tiny_printf("gmsl rx:error\r\n");
                }
            }
            else if (APP_GMSL_GetExpect() == GMSL_RESP_ACK)
            {
                if (APP_GMSL_GetAckOk() != 0U)
                {
                    tiny_printf("[GMSL RX] ACK\r\n");
                    tiny_printf("[GMSL RX] ---- end write ----\r\n");
                }
                else
                {
                    tiny_printf("gmsl rx:ack invalid\r\n");
                }
            }
            else if (APP_GMSL_GetExpect() == GMSL_RESP_DATA)
            {
                tiny_printf("[GMSL RX] DATA len=%u\r\n", UART0_RX_Manager.g_bufferPos);
                dump_buffer8((uint8_t *) UART0_RX_Manager.g_rx_buffer,
                             UART0_RX_Manager.g_bufferPos);
                tiny_printf("[GMSL RX] ---- end read ----\r\n");
            }
        }
        else
        {
            tiny_printf("uart rx:buffer\r\n");
            dump_buffer8((uint8_t *) UART0_RX_Manager.g_rx_buffer , UART0_RX_Manager.g_bufferPos);	
        }

        reset_buffer((uint8_t*) UART0_RX_Manager.g_rx_buffer, 0x00U , UART0_RX_Manager.g_bufferPos);   
        _vComPortResetState(); 	
        R_Config_UART0_Start();	
	}
}

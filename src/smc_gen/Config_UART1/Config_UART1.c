/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018, 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_UART1.c
* Component Version: 1.7.1
* Device(s)        : R7F701692
* Description      : This file implements device driver for Config_UART1.
***********************************************************************************************************************/
/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "Config_UART1.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t *  gp_uart1_tx_address;    /* uart1 transmit data address */
volatile uint16_t   g_uart1_tx_count;       /* uart1 transmit data number */
volatile uint8_t *  gp_uart1_rx_address;    /* uart1 receive data address */
volatile uint16_t   g_uart1_rx_count;       /* uart1 receive data number */
volatile uint16_t   g_uart1_rx_length;      /* uart1 receive data length */
extern volatile uint32_t g_cg_sync_read;
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_UART1_Create
* Description  : This function initializes the Config_UART1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART1_Create(void)
{
    uint32_t tmp_port;

    /* Set LIN reset mode */
    RLN31.LCUC = _UART_LIN_RESET_MODE_CAUSED;
    /* Disable ICRLIN31UR0 operation and clear request */
    INTC2.ICRLIN31UR0.BIT.MKRLIN31UR0 = _INT_PROCESSING_DISABLED;
    INTC2.ICRLIN31UR0.BIT.RFRLIN31UR0 = _INT_REQUEST_NOT_OCCUR;
    /* Disable ICRLIN31UR1 operation and clear request */
    INTC2.ICRLIN31UR1.BIT.MKRLIN31UR1 = _INT_PROCESSING_DISABLED;
    INTC2.ICRLIN31UR1.BIT.RFRLIN31UR1 = _INT_REQUEST_NOT_OCCUR;
    /* Disable ICRLIN31UR2 operation and clear request */
    INTC2.ICRLIN31UR2.BIT.MKRLIN31UR2 = _INT_PROCESSING_DISABLED;
    INTC2.ICRLIN31UR2.BIT.RFRLIN31UR2 = _INT_REQUEST_NOT_OCCUR;
    /* Set ICRLIN31UR0 table method */
    INTC2.ICRLIN31UR0.BIT.TBRLIN31UR0 = _INT_TABLE_VECTOR;
    /* Set ICRLIN31UR0 priority */
    INTC2.ICRLIN31UR0.UINT16 &= _INT_PRIORITY_LOWEST;
    /* Set ICRLIN31UR1 table method */
    INTC2.ICRLIN31UR1.BIT.TBRLIN31UR1 = _INT_TABLE_VECTOR;
    /* Set ICRLIN31UR1 priority */
    INTC2.ICRLIN31UR1.UINT16 &= _INT_PRIORITY_LOWEST;
    /* Set ICRLIN31UR2 table method */
    INTC2.ICRLIN31UR2.BIT.TBRLIN31UR2 = _INT_TABLE_VECTOR;
    /* Set ICRLIN31UR2 priority */
    INTC2.ICRLIN31UR2.UINT16 &= _INT_PRIORITY_LOWEST;
    /* Set UART1 setting */
    RLN31.LWBR = _UART_6_SAMPLING | _UART_PRESCALER_CLOCK_SELECT_2;
    RLN31.LBRP01.UINT16 = _UART1_BAUD_RATE_PRESCALER;
    RLN31.LMD = _UART_NOISE_FILTER_ENABLED | _UART_MODE_SELECT;
    RLN31.LEDE = _UART_FRAMING_ERROR_DETECTED | _UART_OVERRUN_ERROR_DETECTED;
    RLN31.LBFC = _UART_TRANSMISSION_NORMAL | _UART_RECEPTION_NORMAL | _UART_PARITY_PROHIBITED | _UART_STOP_BIT_1 | 
                 _UART_LSB | _UART_LENGTH_8;
    RLN31.LCUC = _UART_LIN_RESET_MODE_CANCELED;
    /* Synchronization processing */
    g_cg_sync_read = RLN31.LCUC;
    __syncp();
    /* Set RLIN31RX pin */
    PORT.PIBC10 &= _PORT_CLEAR_BIT11;
    PORT.PBDC10 &= _PORT_CLEAR_BIT11;
    PORT.PM10 |= _PORT_SET_BIT11;
    PORT.PMC10 &= _PORT_CLEAR_BIT11;
    PORT.PFC10 &= _PORT_CLEAR_BIT11;
    PORT.PFCE10 |= _PORT_SET_BIT11;
    PORT.PFCAE10 |= _PORT_SET_BIT11;
    PORT.PMC10 |= _PORT_SET_BIT11;
    /* Set RLIN31TX pin */
    PORT.PIBC10 &= _PORT_CLEAR_BIT12;
    PORT.PBDC10 &= _PORT_CLEAR_BIT12;
    PORT.PM10 |= _PORT_SET_BIT12;
    PORT.PMC10 &= _PORT_CLEAR_BIT12;
    tmp_port = PORT.PDSC10;
    PORT.PPCMD10 = _WRITE_PROTECT_COMMAND;
    PORT.PDSC10 = (tmp_port & _PORT_CLEAR_BIT12);
    PORT.PDSC10 = (uint32_t) ~(tmp_port & _PORT_CLEAR_BIT12);
    PORT.PDSC10 = (tmp_port & _PORT_CLEAR_BIT12);
    PORT.PFC10 |= _PORT_SET_BIT12;
    PORT.PFCE10 &= _PORT_CLEAR_BIT12;
    PORT.PMC10 |= _PORT_SET_BIT12;
    PORT.PM10 &= _PORT_CLEAR_BIT12;

    R_Config_UART1_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_UART1_Start
* Description  : This function starts the Config_UART1 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART1_Start(void)
{
    /* Enable UART1 operation */
    RLN31.LUOER |= _UART_RECEPTION_ENABLED | _UART_TRANSMISSION_ENABLED;
    /* Clear ICRLIN31UR0 interrupt request and enable operation */
    INTC2.ICRLIN31UR0.BIT.RFRLIN31UR0 = _INT_REQUEST_NOT_OCCUR;
    INTC2.ICRLIN31UR0.BIT.MKRLIN31UR0 = _INT_PROCESSING_ENABLED;
    /* Clear ICRLIN31UR1 interrupt request and enable operation */
    INTC2.ICRLIN31UR1.BIT.RFRLIN31UR1 = _INT_REQUEST_NOT_OCCUR;
    INTC2.ICRLIN31UR1.BIT.MKRLIN31UR1 = _INT_PROCESSING_ENABLED;
    /* Clear ICRLIN31UR2 interrupt request and enable operation */
    INTC2.ICRLIN31UR2.BIT.RFRLIN31UR2 = _INT_REQUEST_NOT_OCCUR;
    INTC2.ICRLIN31UR2.BIT.MKRLIN31UR2 = _INT_PROCESSING_ENABLED;
}

/***********************************************************************************************************************
* Function Name: R_Config_UART1_Stop
* Description  : This function stops the CSIH0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART1_Stop(void)
{
    /* Disable ICRLIN31UR0 operation */
    INTC2.ICRLIN31UR0.BIT.MKRLIN31UR0 = _INT_PROCESSING_DISABLED;
    /* Disable ICRLIN31UR1 operation */
    INTC2.ICRLIN31UR1.BIT.MKRLIN31UR1 = _INT_PROCESSING_DISABLED;
    /* Disable ICRLIN31UR2 operation */
    INTC2.ICRLIN31UR2.BIT.MKRLIN31UR2 = _INT_PROCESSING_DISABLED;
    /* Disable UART1 operation */
    RLN31.LUOER &= (uint8_t) ~(_UART_RECEPTION_ENABLED | _UART_TRANSMISSION_ENABLED);
    /* Synchronization processing */
    g_cg_sync_read = RLN31.LCUC;
    __syncp();
    /* Clear ICRLIN31UR0 request */
    INTC2.ICRLIN31UR0.BIT.RFRLIN31UR0 = _INT_REQUEST_NOT_OCCUR;
    /* Clear ICRLIN31UR1 request */
    INTC2.ICRLIN31UR1.BIT.RFRLIN31UR1 = _INT_REQUEST_NOT_OCCUR;
    /* Clear ICRLIN31UR2 request */
    INTC2.ICRLIN31UR2.BIT.RFRLIN31UR2 = _INT_REQUEST_NOT_OCCUR;
    /* Synchronization processing */
    g_cg_sync_read = RLN31.LCUC;
    __syncp();
}

/***********************************************************************************************************************
* Function Name: R_Config_UART1_Receive
* Description  : This function receives UART1 data.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
MD_STATUS R_Config_UART1_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;
    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        if (( RLN31.LST & _UART_RECEPTION_OPERATED ) == 0U)
        {
            g_uart1_rx_count = 0U;
            g_uart1_rx_length = rx_num;
            gp_uart1_rx_address = rx_buf;
        }
        else
        {
            status = MD_ERROR;
        }
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_Config_UART1_Send
* Description  : This function sends UART1 data.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
MD_STATUS R_Config_UART1_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;
    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart1_tx_address = tx_buf;
        g_uart1_tx_count = tx_num;
        if ((RLN31.LST & _UART_TRANSMISSION_OPERATED) == 0U)
        {
            /* Clear ICRLIN31UR0 request and Disable operation */
            INTC2.ICRLIN31UR0.BIT.MKRLIN31UR0 = _INT_PROCESSING_DISABLED;
            INTC2.ICRLIN31UR0.BIT.RFRLIN31UR0 = _INT_REQUEST_NOT_OCCUR;
            RLN31.LUTDR.UINT16 = *gp_uart1_tx_address;
            gp_uart1_tx_address++;
            g_uart1_tx_count--;
            /* Clear ICRLIN31UR0 requestt and enable operation */
            INTC2.ICRLIN31UR0.BIT.RFRLIN31UR0 = _INT_REQUEST_NOT_OCCUR;
            INTC2.ICRLIN31UR0.BIT.MKRLIN31UR0 = _INT_PROCESSING_ENABLED;
        }
        else
        {
            status = MD_ERROR;
        }
    }
    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

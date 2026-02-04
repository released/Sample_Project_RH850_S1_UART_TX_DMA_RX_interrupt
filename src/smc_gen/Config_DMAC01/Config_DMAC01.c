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
* File Name        : Config_DMAC01.c
* Component Version: 1.8.0
* Device(s)        : R7F701692
* Description      : This file implements device driver for Config_DMAC01.
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
#include "Config_DMAC01.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint32_t g_cg_sync_read;
void r_Config_DMAC01_dmac01_init(void);
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_DMAC01_Create
* Description  : This function initializes the DMAC01 chain.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC01_Create(void)
{
    r_Config_DMAC01_dmac01_init();

    R_Config_DMAC01_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: r_Config_DMAC01_dmac01_init
* Description  : This function initializes the DMAC01 channel.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_Config_DMAC01_dmac01_init(void)
{
    /* Disable DMAC01 operation */
    PDMA0.DCEN1 = _DMAC_CHANNEL_OPERATION_DISABLED;
    /* Disable DMAC01 complete interrupt (INTDMA1) operation and clear request */
    INTC2.ICDMA1.BIT.MKDMA1 = _INT_PROCESSING_DISABLED;
    INTC2.ICDMA1.BIT.RFDMA1 = _INT_REQUEST_NOT_OCCUR;
    /* Set DMAC01 complete interrupt (INTDMA1) setting */
    INTC2.ICDMA1.BIT.TBDMA1 = _INT_TABLE_VECTOR;
    INTC2.ICDMA1.UINT16 &= _INT_PRIORITY_LOWEST;
    /* Set DMAC01 global setting */
    PDMA0.DM01CM = _DMAC_PE1_SETTING | _DMAC_SPID1_SETTING | _DMAC_SUPERVISON_MODE;
    /* Set DMAC01 transfer setting */
    PDMA0.DSA1 = _DMAC01_SOURCE_ADDRESS;
    PDMA0.DDA1 = _DMAC01_DESTINATION_ADDRESS;
    PDMA0.DTC1 = _DMAC01_TRANSFER_COUNT;
    PDMA0.DTCT1 = _DMAC_CYCLES_EXECUTED | _DMAC_TRANSFER_HARDWARE | _DMAC_CHAIN_DISABLE | 
                  _DMAC_TRANSFER_COMPLETION_INTERRUPT_ENABLE | _DMAC_CONTINUOUS_TRANSFER_DISABLE | 
                  _DMAC_RELOAD2_DISABLE | _DMAC_RELOAD1_DISABLE | _DMAC_DESTINATION_INCREMENT | _DMAC_SOURCE_FIXED | 
                  _DMAC_TRANSFER_DATA_8BITS | _DMAC_SINGLE_TRANSFER;
    PDMA0.DTFR1 = _DMAC_TRIGGER_SOURCE_11 | _DMAC_HARDWARE_TRIGGER_ENABLE;
    /* DMAC Transfer Status Clear */
    PDMA0.DCSTC1 = _DMAC_TRANSFER_ERROR_FLAG_CLEAR | _DMAC_TRANSFER_COMPLETION_FLAG_CLEAR;
    /* Synchronization processing */
    g_cg_sync_read = PDMA0.DTCT1;
    __syncp();
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC01_Start
* Description  : This function enables DMAC01 activation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC01_Start(void)
{
    /* Clear DMAC01 complete interrupt (INTDMA1) request and enable operation */
    INTC2.ICDMA1.BIT.RFDMA1 = _INT_REQUEST_NOT_OCCUR;
    INTC2.ICDMA1.BIT.MKDMA1 = _INT_PROCESSING_ENABLED;
    /* Enable DMAC01 operation */
    PDMA0.DCEN1 = _DMAC_CHANNEL_OPERATION_ENABLED;
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC01_Stop
* Description  : This function disables DMAC01 activation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC01_Stop(void)
{
    /* Disable DMAC01 operation */
    PDMA0.DCEN1 = _DMAC_CHANNEL_OPERATION_DISABLED;
    /* Disable DMAC01 operation and clear request */
    INTC2.ICDMA1.BIT.MKDMA1 = _INT_PROCESSING_DISABLED;
    INTC2.ICDMA1.BIT.RFDMA1 = _INT_REQUEST_NOT_OCCUR;
    /* Synchronization processing */
    g_cg_sync_read = PDMA0.DCEN1;
    __syncp();
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC01_Set_SoftwareTrigger
* Description  : This function sets DMAC01 software trigger.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC01_Set_SoftwareTrigger(void)
{
    /* Set DMAC01 software trigger */
    PDMA0.DCSTS1 = _DMAC_SET_REQUEST_FLAG;
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC01_Suspend
* Description  : This function suspends DMAC01 channel operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC01_Suspend(void)
{
    /* Disable DMAC01 operation */
    PDMA0.DCEN1 = _DMAC_CHANNEL_OPERATION_DISABLED;
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC01_Resume
* Description  : This function resumes DMAC01 channel operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC01_Resume(void)
{
    /* Disable DMAC01 operation */
    PDMA0.DCEN1 = _DMAC_CHANNEL_OPERATION_ENABLED;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

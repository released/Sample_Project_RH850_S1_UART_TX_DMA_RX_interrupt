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
* File Name        : Config_DMAC00.c
* Component Version: 1.8.0
* Device(s)        : R7F701692
* Description      : This file implements device driver for Config_DMAC00.
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
#include "Config_DMAC00.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint32_t g_cg_sync_read;
void r_Config_DMAC00_dmac00_init(void);
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_DMAC00_Create
* Description  : This function initializes the DMAC00 chain.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC00_Create(void)
{
    r_Config_DMAC00_dmac00_init();

    R_Config_DMAC00_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: r_Config_DMAC00_dmac00_init
* Description  : This function initializes the DMAC00 channel.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void r_Config_DMAC00_dmac00_init(void)
{
    /* Disable DMAC00 operation */
    PDMA0.DCEN0 = _DMAC_CHANNEL_OPERATION_DISABLED;
    /* Disable DMAC00 complete interrupt (INTDMA0) operation and clear request */
    INTC2.ICDMA0.BIT.MKDMA0 = _INT_PROCESSING_DISABLED;
    INTC2.ICDMA0.BIT.RFDMA0 = _INT_REQUEST_NOT_OCCUR;
    /* Set DMAC00 complete interrupt (INTDMA0) setting */
    INTC2.ICDMA0.BIT.TBDMA0 = _INT_TABLE_VECTOR;
    INTC2.ICDMA0.UINT16 &= _INT_PRIORITY_LOWEST;
    /* Set DMAC00 global setting */
    PDMA0.DM00CM = _DMAC_PE1_SETTING | _DMAC_SPID1_SETTING | _DMAC_SUPERVISON_MODE;
    /* Set DMAC00 transfer setting */
    PDMA0.DSA0 = _DMAC00_SOURCE_ADDRESS;
    PDMA0.DDA0 = _DMAC00_DESTINATION_ADDRESS;
    PDMA0.DTC0 = _DMAC00_TRANSFER_COUNT;
    PDMA0.DTCT0 = _DMAC_CYCLES_EXECUTED | _DMAC_TRANSFER_HARDWARE | _DMAC_CHAIN_DISABLE | 
                  _DMAC_TRANSFER_COMPLETION_INTERRUPT_ENABLE | _DMAC_CONTINUOUS_TRANSFER_DISABLE | 
                  _DMAC_RELOAD2_DISABLE | _DMAC_RELOAD1_DISABLE | _DMAC_DESTINATION_FIXED | _DMAC_SOURCE_INCREMENT | 
                  _DMAC_TRANSFER_DATA_8BITS | _DMAC_SINGLE_TRANSFER;
    PDMA0.DTFR0 = _DMAC_TRIGGER_SOURCE_10 | _DMAC_HARDWARE_TRIGGER_ENABLE;
    /* DMAC Transfer Status Clear */
    PDMA0.DCSTC0 = _DMAC_TRANSFER_ERROR_FLAG_CLEAR | _DMAC_TRANSFER_COMPLETION_FLAG_CLEAR;
    /* Synchronization processing */
    g_cg_sync_read = PDMA0.DTCT0;
    __syncp();
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC00_Start
* Description  : This function enables DMAC00 activation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC00_Start(void)
{
    /* Clear DMAC00 complete interrupt (INTDMA0) request and enable operation */
    INTC2.ICDMA0.BIT.RFDMA0 = _INT_REQUEST_NOT_OCCUR;
    INTC2.ICDMA0.BIT.MKDMA0 = _INT_PROCESSING_ENABLED;
    /* Enable DMAC00 operation */
    PDMA0.DCEN0 = _DMAC_CHANNEL_OPERATION_ENABLED;
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC00_Stop
* Description  : This function disables DMAC00 activation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC00_Stop(void)
{
    /* Disable DMAC00 operation */
    PDMA0.DCEN0 = _DMAC_CHANNEL_OPERATION_DISABLED;
    /* Disable DMAC00 operation and clear request */
    INTC2.ICDMA0.BIT.MKDMA0 = _INT_PROCESSING_DISABLED;
    INTC2.ICDMA0.BIT.RFDMA0 = _INT_REQUEST_NOT_OCCUR;
    /* Synchronization processing */
    g_cg_sync_read = PDMA0.DCEN0;
    __syncp();
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC00_Set_SoftwareTrigger
* Description  : This function sets DMAC00 software trigger.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC00_Set_SoftwareTrigger(void)
{
    /* Set DMAC00 software trigger */
    PDMA0.DCSTS0 = _DMAC_SET_REQUEST_FLAG;
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC00_Suspend
* Description  : This function suspends DMAC00 channel operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC00_Suspend(void)
{
    /* Disable DMAC00 operation */
    PDMA0.DCEN0 = _DMAC_CHANNEL_OPERATION_DISABLED;
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC00_Resume
* Description  : This function resumes DMAC00 channel operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_DMAC00_Resume(void)
{
    /* Disable DMAC00 operation */
    PDMA0.DCEN0 = _DMAC_CHANNEL_OPERATION_ENABLED;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

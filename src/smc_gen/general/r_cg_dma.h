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
* File Name        : r_cg_dma.h
* Version          : 1.0.151
* Device(s)        : R7F701692
* Description      : General header file for DMA controller peripheral.
***********************************************************************************************************************/

#ifndef DMAC_H
#define DMAC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    DMA Control Register (DMACTL)
*/
/* DMA suspension (DMASPD) */
#define _DMAC_SUSPEND_CLEARED                               (0x00000000UL) /* DMA suspension cleared */
#define _DMAC_SUSPEND_REQUEST                               (0x00000001UL) /* DMA suspension request/ongoing */

/*
    DMAC Channel Master Setting m  (DMyiCM)
*/
/* Channel master UM setting (UM) (UM) */
#define _DMAC_SUPERVISON_MODE                               (0x00000000UL) /* Supervison mode */
#define _DMAC_USER_MODE                                     (0x00000002UL) /* User mode */
/* Channel master SPID setting  (SPID[1:0]) */
#define _DMAC_SPID0_SETTING                                 (0x00000000UL) /* SPID0 */
#define _DMAC_SPID1_SETTING                                 (0x00000004UL) /* SPID1 */
#define _DMAC_SPID2_SETTING                                 (0x00000008UL) /* SPID2 */
#define _DMAC_SPID3_SETTING                                 (0x0000000CUL) /* SPID3 */
/* Channel master PEID setting  (PEID[2:0]) */
#define _DMAC_PE1_SETTING                                   (0x00000010UL) /* PE1 */
#define _DMAC_PE2_SETTING                                   (0x00000020UL) /* PE2 */

/*
    DMAC channel operation enable setting register n (DCENn)
*/
/* Channel operation enable (DTE) */
#define _DMAC_CHANNEL_OPERATION_DISABLED                    (0x00000000UL) /* Channel is disabled/suspended */
#define _DMAC_CHANNEL_OPERATION_ENABLED                     (0x00000001UL) /* Channel is enabled/suspension cleared */

/*
    DMAC Transfer Control Register m (DTCTm)
*/
/* DMA transfer disable on transfer error setting (ESE) */
#define _DMAC_CYCLES_EXECUTED                               (0x00000000UL) /* DMA cycles are executed */
#define _DMAC_CYCLES_NOT_EXECUTED                           (0x08000000UL) /* DMA cycles are not executed */
/* DMA transfer request selection assignment (DRS) */
#define _DMAC_TRANSFER_SOFTWARE                             (0x00000000UL) /* Software DMA transfer request */
#define _DMAC_TRANSFER_HARDWARE                             (0x04000000UL) /* Hardware DMA transfer request */
/* Selection of next channel in the chain (CHNSEL[2:0]) */
#define _DMAC_SELECT_CHAIN_0                                (0x00000000UL) /* chain 0 */
#define _DMAC_SELECT_CHAIN_1                                (0x00040000UL) /* chain 1 */
#define _DMAC_SELECT_CHAIN_2                                (0x00080000UL) /* chain 2 */
#define _DMAC_SELECT_CHAIN_3                                (0x000C0000UL) /* chain 3 */
#define _DMAC_SELECT_CHAIN_4                                (0x00100000UL) /* chain 4 */
#define _DMAC_SELECT_CHAIN_5                                (0x00140000UL) /* chain 5 */
#define _DMAC_SELECT_CHAIN_6                                (0x00180000UL) /* chain 6 */
#define _DMAC_SELECT_CHAIN_7                                (0x001C0000UL) /* chain 7 */
/* Chain enable (CHNE[1:0]) */
#define _DMAC_CHAIN_DISABLE                                 (0x00000000UL) /* chain function disabled */
#define _DMAC_CHAIN_LAST_TRANFER                            (0x00010000UL) /* Chain at the last transfer */
#define _DMAC_ALWAYS_CHAIN                                  (0x00030000UL) /* Always chain */
/* Transfer completion interrupt enable (TCE) */
#define _DMAC_TRANSFER_COMPLETION_INTERRUPT_DISABLE         (0x00000000UL) /* Transfer completion interrupt disable */
#define _DMAC_TRANSFER_COMPLETION_INTERRUPT_ENABLE          (0x00004000UL) /* Transfer completion interrupt enable */
/* Continuous transfer enable (MLE) */
#define _DMAC_CONTINUOUS_TRANSFER_ENABLE                    (0x00002000UL) /* DTE bit is cleared */
#define _DMAC_CONTINUOUS_TRANSFER_DISABLE                   (0x00000000UL) /* DTE bit is not cleared */
/* Reload function 2 setting (RLD2M[1:0]) */
#define _DMAC_RELOAD2_DISABLE                               (0x00000000UL) /* Reload function 2 is disabled */
#define _DMAC_RELOAD2_SOURCE_CONTROL                        (0x00000800UL) /* Reload function 2 is enabled source */
#define _DMAC_RELOAD2_DESTINATION_CONTROL                   (0x00001000UL) /* Reload function 2 is enabled
                                                                                              destination */
#define _DMAC_RELOAD2_ALL_CONTROL                           (0x00001800UL) /* Reload function 2 is enabled all */
/* Reload function 1 setting (RLD1M[1:0]) */
#define _DMAC_RELOAD1_DISABLE                               (0x00000000UL) /* Reload function 1 is disabled */
#define _DMAC_RELOAD1_SOURCE_CONTROL                        (0x00000200UL) /* Reload function 1 is enabled source */
#define _DMAC_RELOAD1_DESTINATION_CONTROL                   (0x00000400UL) /* Reload function 1 is enabled
                                                                                              destination */
#define _DMAC_RELOAD1_ALL_CONTROL                           (0x00000600UL) /* Reload function 1 is enabled all */
/* Destination address count direction (DACM[1:0]) */
#define _DMAC_DESTINATION_INCREMENT                         (0x00000000UL) /* Increment mode */
#define _DMAC_DESTINATION_DECREMENT                         (0x00000080UL) /* Decrement mode */
#define _DMAC_DESTINATION_FIXED                             (0x00000100UL) /* fixed */
/* Source address count direction (SACM[1:0]) */
#define _DMAC_SOURCE_INCREMENT                              (0x00000000UL) /* Increment mode */
#define _DMAC_SOURCE_DECREMENT                              (0x00000020UL) /* Decrement mode */
#define _DMAC_SOURCE_FIXED                                  (0x00000040UL) /* fixed */
/* Transfer data size (DS[2:0]) */
#define _DMAC_TRANSFER_DATA_8BITS                           (0x00000000UL) /* transfer data size 8 bits */
#define _DMAC_TRANSFER_DATA_16BITS                          (0x00000004UL) /* transfer data size 16 bits */
#define _DMAC_TRANSFER_DATA_32BITS                          (0x00000008UL) /* transfer data size 32 bits */
#define _DMAC_TRANSFER_DATA_64BITS                          (0x0000000CUL) /* transfer data size 64 bits */
#define _DMAC_TRANSFER_DATA_128BITS                         (0x00000010UL) /* transfer data size 128 bits */
/* Transfer mode (TRM[1:0]) */
#define _DMAC_SINGLE_TRANSFER                               (0x00000000UL) /* Single transfer */
#define _DMAC_BOLCK_TRANSFER_1                              (0x00000001UL) /* Block transfer 1 */
#define _DMAC_BOLCK_TRANSFER_2                              (0x00000002UL) /* Block transfer 2 */

/*
    DMAC Transfer Status Set Register m (DCSTSm)
*/
/* Software DMA transfer request flag (SRS) */
#define _DMAC_READ_THIS_BIT                                 (0x00000000UL) /* always read from this bit */
#define _DMAC_SET_REQUEST_FLAG                              (0x00000001UL) /* Set DMACnm software trigger */

/*
    DMAC Transfer Status Clear Register m (DCSTCm)
*/
/* Software DMA transfer request flag clear (SRC) */
#define _DMAC_SOFTWARE_TRANSFER_REQUEST_FLAG_CLEAR          (0x00000001UL) /* Software DMA transfer request flag clear */
/* Transfer completion flag clear (TCC) */
#define _DMAC_TRANSFER_COMPLETION_FLAG_CLEAR                (0x00000010UL) /* Transfer completion flag clear */
/* Transfer error flag clear (ERC) */
#define _DMAC_TRANSFER_ERROR_FLAG_CLEAR                     (0x00000080UL) /* Transfer error flag clear */

/*
    DTFR Setting Register m (DTFRm)
*/
/* Hardware DMA transfer source selection enable (REQEN) */
#define _DMAC_HARDWARE_TRIGGER_DISABLE                      (0x00000000UL) /* Hardware transfer source is disabled */
#define _DMAC_HARDWARE_TRIGGER_ENABLE                       (0x00000001UL) /* Hardware transfer source is enabled */
/* Hardware DMA transfer source selection (REQSEL[6:0]) */
#define _DMAC_TRIGGER_SOURCE_0                              (0x00000000UL) /* TAUD0REQSEL0 */
#define _DMAC_TRIGGER_SOURCE_1                              (0x00000002UL) /* TAUD0REQSEL1 */
#define _DMAC_TRIGGER_SOURCE_2                              (0x00000004UL) /* TAUD0REQSEL2 */
#define _DMAC_TRIGGER_SOURCE_3                              (0x00000006UL) /* TAUD0REQSEL3 */
#define _DMAC_TRIGGER_SOURCE_4                              (0x00000008UL) /* INTADCA0I0 */
#define _DMAC_TRIGGER_SOURCE_5                              (0x0000000AUL) /* INTADCA0I1 */
#define _DMAC_TRIGGER_SOURCE_6                              (0x0000000CUL) /* INTADCA0I2 */
#define _DMAC_TRIGGER_SOURCE_7                              (0x0000000EUL) /* ADC_CONV_END0 */
#define _DMAC_TRIGGER_SOURCE_8                              (0x00000010UL) /* INTCSIG0IC */
#define _DMAC_TRIGGER_SOURCE_9                              (0x00000012UL) /* INTCSIG0IR */
#define _DMAC_TRIGGER_SOURCE_10                             (0x00000014UL) /* INTRLIN30UR0 */
#define _DMAC_TRIGGER_SOURCE_11                             (0x00000016UL) /* INTRLIN30UR1 */
#define _DMAC_TRIGGER_SOURCE_12                             (0x00000018UL) /* INTP0 */
#define _DMAC_TRIGGER_SOURCE_13                             (0x0000001AUL) /* INTP2 */
#define _DMAC_TRIGGER_SOURCE_14                             (0x0000001CUL) /* INTP4 */
#define _DMAC_TRIGGER_SOURCE_16                             (0x00000020UL) /* INTRLIN37UR0 */
#define _DMAC_TRIGGER_SOURCE_17                             (0x00000022UL) /* TAUD0REQSEL4 */
#define _DMAC_TRIGGER_SOURCE_18                             (0x00000024UL) /* TAUD0REQSEL5 */
#define _DMAC_TRIGGER_SOURCE_19                             (0x00000026UL) /* INTRIIC0TI */
#define _DMAC_TRIGGER_SOURCE_20                             (0x00000028UL) /* INTRIIC0RI */
#define _DMAC_TRIGGER_SOURCE_21                             (0x0000002AUL) /* INTTAUJ0I0 */
#define _DMAC_TRIGGER_SOURCE_22                             (0x0000002CUL) /* INTTAUJ0I3 */
#define _DMAC_TRIGGER_SOURCE_23                             (0x0000002EUL) /* RSCANFDCF0 */
#define _DMAC_TRIGGER_SOURCE_24                             (0x00000030UL) /* RSCANFDCF1 */
#define _DMAC_TRIGGER_SOURCE_26                             (0x00000034UL) /* RSCANFDCF2 */
#define _DMAC_TRIGGER_SOURCE_27                             (0x00000036UL) /* RSCANFDCF3 */
#define _DMAC_TRIGGER_SOURCE_28                             (0x00000038UL) /* INTCSIH1IC */
#define _DMAC_TRIGGER_SOURCE_29                             (0x0000003AUL) /* INTCSIH1IR */
#define _DMAC_TRIGGER_SOURCE_30                             (0x0000003CUL) /* INTCSIH1IJC */
#define _DMAC_TRIGGER_SOURCE_31                             (0x0000003EUL) /* INTP6 */
#define _DMAC_TRIGGER_SOURCE_32                             (0x00000040UL) /* INTP8 */
#define _DMAC_TRIGGER_SOURCE_33                             (0x00000042UL) /* TAUB0REQSEL0 */
#define _DMAC_TRIGGER_SOURCE_34                             (0x00000044UL) /* TAUB0REQSEL1 */
#define _DMAC_TRIGGER_SOURCE_35                             (0x00000046UL) /* TAUB0REQSEL2 */
#define _DMAC_TRIGGER_SOURCE_36                             (0x00000048UL) /* TAUB0REQSEL3 */
#define _DMAC_TRIGGER_SOURCE_37                             (0x0000004AUL) /* TAUB0REQSEL4 */
#define _DMAC_TRIGGER_SOURCE_38                             (0x0000004CUL) /* TAUB0REQSEL5 */
#define _DMAC_TRIGGER_SOURCE_41                             (0x00000052UL) /* INTCSIH3IC */
#define _DMAC_TRIGGER_SOURCE_42                             (0x00000054UL) /* INTCSIH3IR */
#define _DMAC_TRIGGER_SOURCE_43                             (0x00000056UL) /* INTCSIH3IJC */
#define _DMAC_TRIGGER_SOURCE_44                             (0x00000058UL) /* INTRLIN32UR0 */
#define _DMAC_TRIGGER_SOURCE_45                             (0x0000005AUL) /* INTRLIN32UR1 */
#define _DMAC_TRIGGER_SOURCE_46                             (0x0000005CUL) /* INTTAUJ1I0 */
#define _DMAC_TRIGGER_SOURCE_47                             (0x0000005EUL) /* INTTAUJ1I2 */
#define _DMAC_TRIGGER_SOURCE_48                             (0x00000060UL) /* RSCANFDCF4 */
#define _DMAC_TRIGGER_SOURCE_49                             (0x00000062UL) /* RSCANFDCF5 */
#define _DMAC_TRIGGER_SOURCE_50                             (0x00000064UL) /* INTRLIN34UR0 */
#define _DMAC_TRIGGER_SOURCE_51                             (0x00000066UL) /* INTRLIN34UR1 */
#define _DMAC_TRIGGER_SOURCE_52                             (0x00000068UL) /* TAUB1REQSEL0 */
#define _DMAC_TRIGGER_SOURCE_53                             (0x0000006AUL) /* TAUB1REQSEL1 */
#define _DMAC_TRIGGER_SOURCE_54                             (0x0000006CUL) /* TAUB1REQSEL2 */
#define _DMAC_TRIGGER_SOURCE_55                             (0x0000006EUL) /* TAUB1REQSEL3 */
#define _DMAC_TRIGGER_SOURCE_56                             (0x00000070UL) /* TAUB1REQSEL4 */
#define _DMAC_TRIGGER_SOURCE_57                             (0x00000072UL) /* TAUB1REQSEL5 */
#define _DMAC_TRIGGER_SOURCE_60                             (0x00000078UL) /* RSCANFDRF0 */
#define _DMAC_TRIGGER_SOURCE_61                             (0x0000007AUL) /* RSCANFDRF1 */
#define _DMAC_TRIGGER_SOURCE_62                             (0x0000007CUL) /* RSCANFDRF2 */
#define _DMAC_TRIGGER_SOURCE_63                             (0x0000007EUL) /* RSCANFDRF3 */
#define _DMAC_TRIGGER_SOURCE_64                             (0x00000080UL) /* RSCANFDCF6 */
#define _DMAC_TRIGGER_SOURCE_65                             (0x00000082UL) /* RSCANFDCF7 */
#define _DMAC_TRIGGER_SOURCE_66                             (0x00000084UL) /* INTCSIG1IC */
#define _DMAC_TRIGGER_SOURCE_67                             (0x00000086UL) /* INTCSIG1IR */
#define _DMAC_TRIGGER_SOURCE_68                             (0x00000088UL) /* RSCANFDRF4 */
#define _DMAC_TRIGGER_SOURCE_69                             (0x0000008AUL) /* RSCANFDRF5 */
#define _DMAC_TRIGGER_SOURCE_70                             (0x0000008CUL) /* INTCSIH0IC */
#define _DMAC_TRIGGER_SOURCE_71                             (0x0000008EUL) /* INTCSIH0IR */
#define _DMAC_TRIGGER_SOURCE_72                             (0x00000090UL) /* INTCSIH0IJC */
#define _DMAC_TRIGGER_SOURCE_73                             (0x00000092UL) /* INTP1 */
#define _DMAC_TRIGGER_SOURCE_74                             (0x00000094UL) /* INTP3 */
#define _DMAC_TRIGGER_SOURCE_75                             (0x00000096UL) /* INTP5 */
#define _DMAC_TRIGGER_SOURCE_77                             (0x0000009AUL) /* INTRLIN37UR1 */
#define _DMAC_TRIGGER_SOURCE_78                             (0x0000009CUL) /* INTCSIG2IC */
#define _DMAC_TRIGGER_SOURCE_79                             (0x0000009EUL) /* INTCSIG2IR */
#define _DMAC_TRIGGER_SOURCE_80                             (0x000000A0UL) /* INTTAUJ0I1 */
#define _DMAC_TRIGGER_SOURCE_81                             (0x000000A2UL) /* INTTAUJ0I2 */
#define _DMAC_TRIGGER_SOURCE_82                             (0x000000A4UL) /* RSCANFDRF6 */
#define _DMAC_TRIGGER_SOURCE_83                             (0x000000A6UL) /* RSCANFDRF7 */
#define _DMAC_TRIGGER_SOURCE_85                             (0x000000AAUL) /* INTDMAFL */
#define _DMAC_TRIGGER_SOURCE_86                             (0x000000ACUL) /* INTRLIN31UR0 */
#define _DMAC_TRIGGER_SOURCE_87                             (0x000000AEUL) /* INTRLIN31UR1 */
#define _DMAC_TRIGGER_SOURCE_88                             (0x000000B0UL) /* INTP7 */
#define _DMAC_TRIGGER_SOURCE_89                             (0x000000B2UL) /* INTCSIH2IC */
#define _DMAC_TRIGGER_SOURCE_90                             (0x000000B4UL) /* INTCSIH2IR */
#define _DMAC_TRIGGER_SOURCE_91                             (0x000000B6UL) /* INTCSIH2IJC */
#define _DMAC_TRIGGER_SOURCE_100                            (0x000000C8UL) /* INTTAUJ1I1 */
#define _DMAC_TRIGGER_SOURCE_101                            (0x000000CAUL) /* INTTAUJ1I3 */
#define _DMAC_TRIGGER_SOURCE_102                            (0x000000CCUL) /* INTP9 */
#define _DMAC_TRIGGER_SOURCE_103                            (0x000000CEUL) /* INTADCA1I0 */
#define _DMAC_TRIGGER_SOURCE_104                            (0x000000D0UL) /* INTADCA1I1 */
#define _DMAC_TRIGGER_SOURCE_105                            (0x000000D2UL) /* INTADCA1I2 */
#define _DMAC_TRIGGER_SOURCE_106                            (0x000000D4UL) /* INTTAUJ2I3 */
#define _DMAC_TRIGGER_SOURCE_107                            (0x000000D6UL) /* INTTAUJ3I0 */
#define _DMAC_TRIGGER_SOURCE_108                            (0x000000D8UL) /* INTTAUJ3I1 */
#define _DMAC_TRIGGER_SOURCE_109                            (0x000000DAUL) /* INTTAUJ3I2 */
#define _DMAC_TRIGGER_SOURCE_110                            (0x000000DCUL) /* INTTAUJ3I3 */
#define _DMAC_TRIGGER_SOURCE_111                            (0x000000DEUL) /* INTRLIN33UR0 */
#define _DMAC_TRIGGER_SOURCE_112                            (0x000000E0UL) /* INTRLIN33UR1 */
#define _DMAC_TRIGGER_SOURCE_113                            (0x000000E2UL) /* INTRIIC1TI */
#define _DMAC_TRIGGER_SOURCE_114                            (0x000000E4UL) /* INTRIIC1RI */
#define _DMAC_TRIGGER_SOURCE_115                            (0x000000E6UL) /* INTADCA1I0 */
#define _DMAC_TRIGGER_SOURCE_116                            (0x000000E8UL) /* INTADCA1I1 */
#define _DMAC_TRIGGER_SOURCE_117                            (0x000000EAUL) /* INTADCA1I2 */
#define _DMAC_TRIGGER_SOURCE_118                            (0x000000ECUL) /* ADC_CONV_END1 */
#define _DMAC_TRIGGER_SOURCE_119                            (0x000000EEUL) /* INTRLIN36UR0 */
#define _DMAC_TRIGGER_SOURCE_120                            (0x000000F0UL) /* INTRLIN36UR1 */
#define _DMAC_TRIGGER_SOURCE_121                            (0x000000F2UL) /* INTRLIN35UR0 */
#define _DMAC_TRIGGER_SOURCE_122                            (0x000000F4UL) /* INTRLIN35UR1 */
#define _DMAC_TRIGGER_SOURCE_123                            (0x000000F6UL) /* INTSENT0RI */
#define _DMAC_TRIGGER_SOURCE_124                            (0x000000F8UL) /* INTSENT1RI */
#define _DMAC_TRIGGER_SOURCE_125                            (0x000000FAUL) /* INTCSIG3IC */
#define _DMAC_TRIGGER_SOURCE_126                            (0x000000FCUL) /* INTCSIG3IR */

/*
    DMA trigger factor select register of TAUD0 (DTFSEL_TAUD0)
*/
/* DMA Trigger Factor selection of TAUD0REQSEL5 (REQSEL5[3:0]) */
#define _DMAC_REQSEL5_TAUD0_CLEAR                           (0xFF0FFFFFUL) /* Clear TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I0                            (0x00000000UL) /* INTTAUD0I0 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I1                            (0x00100000UL) /* INTTAUD0I1 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I2                            (0x00200000UL) /* INTTAUD0I2 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I3                            (0x00300000UL) /* INTTAUD0I3 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I4                            (0x00400000UL) /* INTTAUD0I4 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I5                            (0x00500000UL) /* INTTAUD0I5 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I6                            (0x00600000UL) /* INTTAUD0I6 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I7                            (0x00700000UL) /* INTTAUD0I7 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I8                            (0x00800000UL) /* INTTAUD0I8 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I9                            (0x00900000UL) /* INTTAUD0I9 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I10                           (0x00A00000UL) /* INTTAUD0I10 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I11                           (0x00B00000UL) /* INTTAUD0I11 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I12                           (0x00C00000UL) /* INTTAUD0I12 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I13                           (0x00D00000UL) /* INTTAUD0I13 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I14                           (0x00E00000UL) /* INTTAUD0I14 valid in TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUD0I15                           (0x00F00000UL) /* INTTAUD0I15 valid in TAUD0REQSEL5 */
/* DMA Trigger Factor selection of TAUD0REQSEL4 (REQSEL4[3:0]) */
#define _DMAC_REQSEL4_TAUD0_CLEAR                           (0xFFF0FFFFUL) /* Clear TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I0                            (0x00000000UL) /* INTTAUD0I0 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I1                            (0x00010000UL) /* INTTAUD0I1 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I2                            (0x00020000UL) /* INTTAUD0I2 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I3                            (0x00030000UL) /* INTTAUD0I3 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I4                            (0x00040000UL) /* INTTAUD0I4 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I5                            (0x00050000UL) /* INTTAUD0I5 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I6                            (0x00060000UL) /* INTTAUD0I6 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I7                            (0x00070000UL) /* INTTAUD0I7 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I8                            (0x00080000UL) /* INTTAUD0I8 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I9                            (0x00090000UL) /* INTTAUD0I9 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I10                           (0x000A0000UL) /* INTTAUD0I10 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I11                           (0x000B0000UL) /* INTTAUD0I11 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I12                           (0x000C0000UL) /* INTTAUD0I12 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I13                           (0x000D0000UL) /* INTTAUD0I13 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I14                           (0x000E0000UL) /* INTTAUD0I14 valid in TAUD0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUD0I15                           (0x000F0000UL) /* INTTAUD0I15 valid in TAUD0REQSEL4 */
/* DMA Trigger Factor selection of TAUD0REQSEL3 (REQSEL3[3:0]) */
#define _DMAC_REQSEL3_TAUD0_CLEAR                           (0xFFFF0FFFUL) /* Clear TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I0                            (0x00000000UL) /* INTTAUD0I0 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I1                            (0x00001000UL) /* INTTAUD0I1 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I2                            (0x00002000UL) /* INTTAUD0I2 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I3                            (0x00003000UL) /* INTTAUD0I3 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I4                            (0x00004000UL) /* INTTAUD0I4 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I5                            (0x00005000UL) /* INTTAUD0I5 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I6                            (0x00006000UL) /* INTTAUD0I6 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I7                            (0x00007000UL) /* INTTAUD0I7 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I8                            (0x00008000UL) /* INTTAUD0I8 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I9                            (0x00009000UL) /* INTTAUD0I9 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I10                           (0x0000A000UL) /* INTTAUD0I10 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I11                           (0x0000B000UL) /* INTTAUD0I11 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I12                           (0x0000C000UL) /* INTTAUD0I12 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I13                           (0x0000D000UL) /* INTTAUD0I13 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I14                           (0x0000E000UL) /* INTTAUD0I14 valid in TAUD0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUD0I15                           (0x0000F000UL) /* INTTAUD0I15 valid in TAUD0REQSEL3 */
/* DMA Trigger Factor selection of TAUD0REQSEL2 (REQSEL2[3:0]) */
#define _DMAC_REQSEL2_TAUD0_CLEAR                           (0xFFFFF0FFUL) /* Clear TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I0                            (0x00000000UL) /* INTTAUD0I0 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I1                            (0x00000100UL) /* INTTAUD0I1 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I2                            (0x00000200UL) /* INTTAUD0I2 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I3                            (0x00000300UL) /* INTTAUD0I3 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I4                            (0x00000400UL) /* INTTAUD0I4 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I5                            (0x00000500UL) /* INTTAUD0I5 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I6                            (0x00000600UL) /* INTTAUD0I6 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I7                            (0x00000700UL) /* INTTAUD0I7 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I8                            (0x00000800UL) /* INTTAUD0I8 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I9                            (0x00000900UL) /* INTTAUD0I9 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I10                           (0x00000A00UL) /* INTTAUD0I10 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I11                           (0x00000B00UL) /* INTTAUD0I11 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I12                           (0x00000C00UL) /* INTTAUD0I12 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I13                           (0x00000D00UL) /* INTTAUD0I13 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I14                           (0x00000E00UL) /* INTTAUD0I14 valid in TAUD0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUD0I15                           (0x00000F00UL) /* INTTAUD0I15 valid in TAUD0REQSEL2 */
/* DMA Trigger Factor selection of TAUD0REQSEL1 (REQSEL1[3:0]) */
#define _DMAC_REQSEL1_TAUD0_CLEAR                           (0xFFFFFF0FUL) /* Clear TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I0                            (0x00000000UL) /* INTTAUD0I0 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I1                            (0x00000010UL) /* INTTAUD0I1 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I2                            (0x00000020UL) /* INTTAUD0I2 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I3                            (0x00000030UL) /* INTTAUD0I3 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I4                            (0x00000040UL) /* INTTAUD0I4 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I5                            (0x00000050UL) /* INTTAUD0I5 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I6                            (0x00000060UL) /* INTTAUD0I6 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I7                            (0x00000070UL) /* INTTAUD0I7 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I8                            (0x00000080UL) /* INTTAUD0I8 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I9                            (0x00000090UL) /* INTTAUD0I9 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I10                           (0x000000A0UL) /* INTTAUD0I10 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I11                           (0x000000B0UL) /* INTTAUD0I11 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I12                           (0x000000C0UL) /* INTTAUD0I12 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I13                           (0x000000D0UL) /* INTTAUD0I13 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I14                           (0x000000E0UL) /* INTTAUD0I14 valid in TAUD0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUD0I15                           (0x000000F0UL) /* INTTAUD0I15 valid in TAUD0REQSEL1 */
/* DMA Trigger Factor selection of TAUD0REQSEL0 (REQSEL0[3:0]) */
#define _DMAC_REQSEL0_TAUD0_CLEAR                           (0xFFFFFFF0UL) /* Clear TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I0                            (0x00000000UL) /* INTTAUD0I0 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I1                            (0x00000001UL) /* INTTAUD0I1 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I2                            (0x00000002UL) /* INTTAUD0I2 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I3                            (0x00000003UL) /* INTTAUD0I3 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I4                            (0x00000004UL) /* INTTAUD0I4 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I5                            (0x00000005UL) /* INTTAUD0I5 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I6                            (0x00000006UL) /* INTTAUD0I6 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I7                            (0x00000007UL) /* INTTAUD0I7 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I8                            (0x00000008UL) /* INTTAUD0I8 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I9                            (0x00000009UL) /* INTTAUD0I9 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I10                           (0x0000000AUL) /* INTTAUD0I10 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I11                           (0x0000000BUL) /* INTTAUD0I11 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I12                           (0x0000000CUL) /* INTTAUD0I12 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I13                           (0x0000000DUL) /* INTTAUD0I13 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I14                           (0x0000000EUL) /* INTTAUD0I14 valid in TAUD0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUD0I15                           (0x0000000FUL) /* INTTAUD0I15 valid in TAUD0REQSEL0 */

/*
    DMA trigger factor select register of TAUB0 (DTFSEL_TAUB0)
*/
/* DMA Trigger Factor selection of TAUB0REQSEL5 (REQSEL5[3:0]) */
#define _DMAC_REQSEL5_TAUB0_CLEAR                           (0xFF0FFFFFUL) /* Clear TAUD0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I0                            (0x00000000UL) /* INTTAUB0I0 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I1                            (0x00100000UL) /* INTTAUB0I1 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I2                            (0x00200000UL) /* INTTAUB0I2 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I3                            (0x00300000UL) /* INTTAUB0I3 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I4                            (0x00400000UL) /* INTTAUB0I4 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I5                            (0x00500000UL) /* INTTAUB0I5 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I6                            (0x00600000UL) /* INTTAUB0I6 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I7                            (0x00700000UL) /* INTTAUB0I7 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I8                            (0x00800000UL) /* INTTAUB0I8 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I9                            (0x00900000UL) /* INTTAUB0I9 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I10                           (0x00A00000UL) /* INTTAUB0I10 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I11                           (0x00B00000UL) /* INTTAUB0I11 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I12                           (0x00C00000UL) /* INTTAUB0I12 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I13                           (0x00D00000UL) /* INTTAUB0I13 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I14                           (0x00E00000UL) /* INTTAUB0I14 valid in TAUB0REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB0I15                           (0x00F00000UL) /* INTTAUB0I15 valid in TAUB0REQSEL5 */
/* DMA Trigger Factor selection of TAUB0REQSEL4 (REQSEL4[3:0]) */
#define _DMAC_REQSEL4_TAUB0_CLEAR                           (0xFFF0FFFFUL) /* Clear TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I0                            (0x00000000UL) /* INTTAUB0I0 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I1                            (0x00010000UL) /* INTTAUB0I1 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I2                            (0x00020000UL) /* INTTAUB0I2 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I3                            (0x00030000UL) /* INTTAUB0I3 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I4                            (0x00040000UL) /* INTTAUB0I4 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I5                            (0x00050000UL) /* INTTAUB0I5 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I6                            (0x00060000UL) /* INTTAUB0I6 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I7                            (0x00070000UL) /* INTTAUB0I7 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I8                            (0x00080000UL) /* INTTAUB0I8 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I9                            (0x00090000UL) /* INTTAUB0I9 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I10                           (0x000A0000UL) /* INTTAUB0I10 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I11                           (0x000B0000UL) /* INTTAUB0I11 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I12                           (0x000C0000UL) /* INTTAUB0I12 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I13                           (0x000D0000UL) /* INTTAUB0I13 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I14                           (0x000E0000UL) /* INTTAUB0I14 valid in TAUB0REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB0I15                           (0x000F0000UL) /* INTTAUB0I15 valid in TAUB0REQSEL4 */
/* DMA Trigger Factor selection of TAUB0REQSEL3 (REQSEL3[3:0]) */
#define _DMAC_REQSEL3_TAUB0_CLEAR                           (0xFFFF0FFFUL) /* Clear TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I0                            (0x00000000UL) /* INTTAUB0I0 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I1                            (0x00001000UL) /* INTTAUB0I1 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I2                            (0x00002000UL) /* INTTAUB0I2 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I3                            (0x00003000UL) /* INTTAUB0I3 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I4                            (0x00004000UL) /* INTTAUB0I4 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I5                            (0x00005000UL) /* INTTAUB0I5 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I6                            (0x00006000UL) /* INTTAUB0I6 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I7                            (0x00007000UL) /* INTTAUB0I7 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I8                            (0x00008000UL) /* INTTAUB0I8 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I9                            (0x00009000UL) /* INTTAUB0I9 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I10                           (0x0000A000UL) /* INTTAUB0I10 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I11                           (0x0000B000UL) /* INTTAUB0I11 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I12                           (0x0000C000UL) /* INTTAUB0I12 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I13                           (0x0000D000UL) /* INTTAUB0I13 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I14                           (0x0000E000UL) /* INTTAUB0I14 valid in TAUB0REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB0I15                           (0x0000F000UL) /* INTTAUB0I15 valid in TAUB0REQSEL3 */
/* DMA Trigger Factor selection of TAUB0REQSEL2 (REQSEL2[3:0]) */
#define _DMAC_REQSEL2_TAUB0_CLEAR                           (0xFFFFF0FFUL) /* Clear TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I0                            (0x00000000UL) /* INTTAUB0I0 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I1                            (0x00000100UL) /* INTTAUB0I1 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I2                            (0x00000200UL) /* INTTAUB0I2 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I3                            (0x00000300UL) /* INTTAUB0I3 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I4                            (0x00000400UL) /* INTTAUB0I4 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I5                            (0x00000500UL) /* INTTAUB0I5 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I6                            (0x00000600UL) /* INTTAUB0I6 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I7                            (0x00000700UL) /* INTTAUB0I7 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I8                            (0x00000800UL) /* INTTAUB0I8 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I9                            (0x00000900UL) /* INTTAUB0I9 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I10                           (0x00000A00UL) /* INTTAUB0I10 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I11                           (0x00000B00UL) /* INTTAUB0I11 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I12                           (0x00000C00UL) /* INTTAUB0I12 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I13                           (0x00000D00UL) /* INTTAUB0I13 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I14                           (0x00000E00UL) /* INTTAUB0I14 valid in TAUB0REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB0I15                           (0x00000F00UL) /* INTTAUB0I15 valid in TAUB0REQSEL2 */
/* DMA Trigger Factor selection of TAUB0REQSEL1 (REQSEL1[3:0]) */
#define _DMAC_REQSEL1_TAUB0_CLEAR                           (0xFFFFFF0FUL) /* Clear TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I0                            (0x00000000UL) /* INTTAUB0I0 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I1                            (0x00000010UL) /* INTTAUB0I1 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I2                            (0x00000020UL) /* INTTAUB0I2 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I3                            (0x00000030UL) /* INTTAUB0I3 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I4                            (0x00000040UL) /* INTTAUB0I4 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I5                            (0x00000050UL) /* INTTAUB0I5 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I6                            (0x00000060UL) /* INTTAUB0I6 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I7                            (0x00000070UL) /* INTTAUB0I7 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I8                            (0x00000080UL) /* INTTAUB0I8 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I9                            (0x00000090UL) /* INTTAUB0I9 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I10                           (0x000000A0UL) /* INTTAUB0I10 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I11                           (0x000000B0UL) /* INTTAUB0I11 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I12                           (0x000000C0UL) /* INTTAUB0I12 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I13                           (0x000000D0UL) /* INTTAUB0I13 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I14                           (0x000000E0UL) /* INTTAUB0I14 valid in TAUB0REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB0I15                           (0x000000F0UL) /* INTTAUB0I15 valid in TAUB0REQSEL1 */
/* DMA Trigger Factor selection of TAUB0REQSEL0 (REQSEL0[3:0]) */
#define _DMAC_REQSEL0_TAUB0_CLEAR                           (0xFFFFFFF0UL) /* Clear TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I0                            (0x00000000UL) /* INTTAUB0I0 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I1                            (0x00000001UL) /* INTTAUB0I1 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I2                            (0x00000002UL) /* INTTAUB0I2 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I3                            (0x00000003UL) /* INTTAUB0I3 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I4                            (0x00000004UL) /* INTTAUB0I4 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I5                            (0x00000005UL) /* INTTAUB0I5 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I6                            (0x00000006UL) /* INTTAUB0I6 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I7                            (0x00000007UL) /* INTTAUB0I7 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I8                            (0x00000008UL) /* INTTAUB0I8 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I9                            (0x00000009UL) /* INTTAUB0I9 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I10                           (0x0000000AUL) /* INTTAUB0I10 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I11                           (0x0000000BUL) /* INTTAUB0I11 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I12                           (0x0000000CUL) /* INTTAUB0I12 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I13                           (0x0000000DUL) /* INTTAUB0I13 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I14                           (0x0000000EUL) /* INTTAUB0I14 valid in TAUB0REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB0I15                           (0x0000000FUL) /* INTTAUB0I15 valid in TAUB0REQSEL0 */

/*
    DMA trigger factor select register of TAUB1 (DTFSEL_TAUB1)
*/
/* DMA Trigger Factor selection of TAUB1REQSEL5 (REQSEL5[3:0]) */
#define _DMAC_REQSEL5_TAUB1_CLEAR                           (0xFF0FFFFFUL) /* Clear TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I0                            (0x00000000UL) /* INTTAUB1I0 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I1                            (0x00100000UL) /* INTTAUB1I1 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I2                            (0x00200000UL) /* INTTAUB1I2 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I3                            (0x00300000UL) /* INTTAUB1I3 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I4                            (0x00400000UL) /* INTTAUB1I4 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I5                            (0x00500000UL) /* INTTAUB1I5 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I6                            (0x00600000UL) /* INTTAUB1I6 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I7                            (0x00700000UL) /* INTTAUB1I7 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I8                            (0x00800000UL) /* INTTAUB1I8 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I9                            (0x00900000UL) /* INTTAUB1I9 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I10                           (0x00A00000UL) /* INTTAUB1I10 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I11                           (0x00B00000UL) /* INTTAUB1I11 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I12                           (0x00C00000UL) /* INTTAUB1I12 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I13                           (0x00D00000UL) /* INTTAUB1I13 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I14                           (0x00E00000UL) /* INTTAUB1I14 valid in TAUB1REQSEL5 */
#define _DMAC_REQSEL5_INTTAUB1I15                           (0x00F00000UL) /* INTTAUB1I15 valid in TAUB1REQSEL5 */
/* DMA Trigger Factor selection of TAUB1REQSEL4 (REQSEL4[3:0]) */
#define _DMAC_REQSEL4_TAUB1_CLEAR                           (0xFFF0FFFFUL) /* Clear TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I0                            (0x00000000UL) /* INTTAUB1I0 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I1                            (0x00010000UL) /* INTTAUB1I1 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I2                            (0x00020000UL) /* INTTAUB1I2 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I3                            (0x00030000UL) /* INTTAUB1I3 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I4                            (0x00040000UL) /* INTTAUB1I4 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I5                            (0x00050000UL) /* INTTAUB1I5 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I6                            (0x00060000UL) /* INTTAUB1I6 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I7                            (0x00070000UL) /* INTTAUB1I7 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I8                            (0x00080000UL) /* INTTAUB1I8 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I9                            (0x00090000UL) /* INTTAUB1I9 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I10                           (0x000A0000UL) /* INTTAUB1I10 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I11                           (0x000B0000UL) /* INTTAUB1I11 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I12                           (0x000C0000UL) /* INTTAUB1I12 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I13                           (0x000D0000UL) /* INTTAUB1I13 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I14                           (0x000E0000UL) /* INTTAUB1I14 valid in TAUB1REQSEL4 */
#define _DMAC_REQSEL4_INTTAUB1I15                           (0x000F0000UL) /* INTTAUB1I15 valid in TAUB1REQSEL4 */
/* DMA Trigger Factor selection of TAUB1REQSEL3 (REQSEL3[3:0]) */
#define _DMAC_REQSEL3_TAUB1_CLEAR                           (0xFFFF0FFFUL) /* Clear TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I0                            (0x00000000UL) /* INTTAUB1I0 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I1                            (0x00001000UL) /* INTTAUB1I1 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I2                            (0x00002000UL) /* INTTAUB1I2 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I3                            (0x00003000UL) /* INTTAUB1I3 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I4                            (0x00004000UL) /* INTTAUB1I4 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I5                            (0x00005000UL) /* INTTAUB1I5 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I6                            (0x00006000UL) /* INTTAUB1I6 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I7                            (0x00007000UL) /* INTTAUB1I7 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I8                            (0x00008000UL) /* INTTAUB1I8 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I9                            (0x00009000UL) /* INTTAUB1I9 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I10                           (0x0000A000UL) /* INTTAUB1I10 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I11                           (0x0000B000UL) /* INTTAUB1I11 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I12                           (0x0000C000UL) /* INTTAUB1I12 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I13                           (0x0000D000UL) /* INTTAUB1I13 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I14                           (0x0000E000UL) /* INTTAUB1I14 valid in TAUB1REQSEL3 */
#define _DMAC_REQSEL3_INTTAUB1I15                           (0x0000F000UL) /* INTTAUB1I15 valid in TAUB1REQSEL3 */
/* DMA Trigger Factor selection of TAUB1REQSEL2 (REQSEL2[3:0]) */
#define _DMAC_REQSEL2_TAUB1_CLEAR                           (0xFFFFF0FFUL) /* Clear TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I0                            (0x00000000UL) /* INTTAUB1I0 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I1                            (0x00000100UL) /* INTTAUB1I1 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I2                            (0x00000200UL) /* INTTAUB1I2 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I3                            (0x00000300UL) /* INTTAUB1I3 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I4                            (0x00000400UL) /* INTTAUB1I4 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I5                            (0x00000500UL) /* INTTAUB1I5 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I6                            (0x00000600UL) /* INTTAUB1I6 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I7                            (0x00000700UL) /* INTTAUB1I7 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I8                            (0x00000800UL) /* INTTAUB1I8 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I9                            (0x00000900UL) /* INTTAUB1I9 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I10                           (0x00000A00UL) /* INTTAUB1I10 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I11                           (0x00000B00UL) /* INTTAUB1I11 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I12                           (0x00000C00UL) /* INTTAUB1I12 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I13                           (0x00000D00UL) /* INTTAUB1I13 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I14                           (0x00000E00UL) /* INTTAUB1I14 valid in TAUB1REQSEL2 */
#define _DMAC_REQSEL2_INTTAUB1I15                           (0x00000F00UL) /* INTTAUB1I15 valid in TAUB1REQSEL2 */
/* DMA Trigger Factor selection of TAUB1REQSEL1 (REQSEL1[3:0]) */
#define _DMAC_REQSEL1_TAUB1_CLEAR                           (0xFFFFFF0FUL) /* Clear TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I0                            (0x00000000UL) /* INTTAUB1I0 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I1                            (0x00000010UL) /* INTTAUB1I1 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I2                            (0x00000020UL) /* INTTAUB1I2 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I3                            (0x00000030UL) /* INTTAUB1I3 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I4                            (0x00000040UL) /* INTTAUB1I4 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I5                            (0x00000050UL) /* INTTAUB1I5 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I6                            (0x00000060UL) /* INTTAUB1I6 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I7                            (0x00000070UL) /* INTTAUB1I7 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I8                            (0x00000080UL) /* INTTAUB1I8 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I9                            (0x00000090UL) /* INTTAUB1I9 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I10                           (0x000000A0UL) /* INTTAUB1I10 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I11                           (0x000000B0UL) /* INTTAUB1I11 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I12                           (0x000000C0UL) /* INTTAUB1I12 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I13                           (0x000000D0UL) /* INTTAUB1I13 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I14                           (0x000000E0UL) /* INTTAUB1I14 valid in TAUB1REQSEL1 */
#define _DMAC_REQSEL1_INTTAUB1I15                           (0x000000F0UL) /* INTTAUB1I15 valid in TAUB1REQSEL1 */
/* DMA Trigger Factor selection of TAUB1REQSEL0 (REQSEL0[3:0]) */
#define _DMAC_REQSEL0_TAUB1_CLEAR                           (0xFFFFFFF0UL) /* Clear TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I0                            (0x00000000UL) /* INTTAUB1I0 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I1                            (0x00000001UL) /* INTTAUB1I1 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I2                            (0x00000002UL) /* INTTAUB1I2 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I3                            (0x00000003UL) /* INTTAUB1I3 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I4                            (0x00000004UL) /* INTTAUB1I4 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I5                            (0x00000005UL) /* INTTAUB1I5 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I6                            (0x00000006UL) /* INTTAUB1I6 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I7                            (0x00000007UL) /* INTTAUB1I7 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I8                            (0x00000008UL) /* INTTAUB1I8 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I9                            (0x00000009UL) /* INTTAUB1I9 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I10                           (0x0000000AUL) /* INTTAUB1I10 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I11                           (0x0000000BUL) /* INTTAUB1I11 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I12                           (0x0000000CUL) /* INTTAUB1I12 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I13                           (0x0000000DUL) /* INTTAUB1I13 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I14                           (0x0000000EUL) /* INTTAUB1I14 valid in TAUB1REQSEL0 */
#define _DMAC_REQSEL0_INTTAUB1I15                           (0x0000000FUL) /* INTTAUB1I15 valid in TAUB1REQSEL0 */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif

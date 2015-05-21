/*
 * hal_edma.h
 *
 *  Created on: 21.05.2015
 *      Author: Kevin
 */

#ifndef HAL_EDMA_HAL_EDMA_H_
#define HAL_EDMA_HAL_EDMA_H_

#define		EDMA_REVID_AM335X                  (0x02u)
#define 	SOC_EDMA3_NUM_DMACH                (64)
#define 	SOC_EDMA3_NUM_QDMACH 			   (8)
#define 	EDMA3_SET_ALL_BITS 				   (0xFFFFFFFFu)
#define 	SYS_INT_EDMACOMPINT  			   (12)
#define 	SYS_INT_EDMAERRINT 				   (14)
#define 	SYS_INT_MMCSD0INT 				   (64)
#define 	EDMA3_CHANNEL_TYPE_DMA 			   (0u)
#define 	EDMA3_CHANNEL_TYPE_QDMA 		   (1u)

/* MMCSD0 receive event. */
#define 	EDMA3_CHA_MMCSD0_RX                (25u)
#define 	MMCSD_RX_EDMA_CHAN             	   (EDMA3_CHA_MMCSD0_RX)


extern void EDMAModuleClkConfig(void);
extern void EDMA3Init(unsigned int baseAdd, unsigned int queNum);
extern void EDMA3AINTCConfigure(void);
extern unsigned int EDMA3RequestChannel(unsigned int baseAdd, unsigned int chType, unsigned int chNum, unsigned int tccNum, unsigned int evtQNum);

#endif /* HAL_EDMA_HAL_EDMA_H_ */

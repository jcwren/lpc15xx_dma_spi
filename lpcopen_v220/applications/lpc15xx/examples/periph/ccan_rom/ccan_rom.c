/*
 * @brief CCAN on-chip driver example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Memory for CAN API */
#define MAX_CAN_PARAM_SIZE          512
static uint32_t gCANapiMem[MAX_CAN_PARAM_SIZE];

/* CAN handle */
static CAN_HANDLE_T pCanHandle;

/* CAN initialization structure / message structure */
static CAN_CFG gCANConfig;
static CAN_MSG_OBJ msg_obj;

/* Callback function prototypes */
static void CAN_rx(uint8_t msg_obj_num);

static void CAN_tx(uint8_t msg_obj_num);

static void CAN_error(uint32_t error_info);

/* Publish CAN Callback Functions */
CAN_CALLBACKS callbacks = {
	CAN_rx,
	CAN_tx,
	CAN_error
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void CAN_rx(uint8_t msgNumber)
{
	/* Determine which CAN message has been received */
	msg_obj.msgobj = msgNumber;

	/* Now load up the msg_obj structure with the CAN message */
	LPC_CAND_API->hwCAN_MsgReceive(pCanHandle, &msg_obj);
	if (msgNumber == 1) {
		/* Simply transmit CAN frame (echo) with with ID +0x100 via buffer 2 */
		msg_obj.msgobj = 2;
		msg_obj.mode_id += 0x100;
		LPC_CAND_API->hwCAN_MsgTransmit(pCanHandle, &msg_obj);
	}
}

static void CAN_tx(uint8_t msg_obj)
{}

static void CAN_error(uint32_t error_info)
{}

void CAN_IRQHandler(void)
{
	LPC_CAND_API->hwCAN_Isr(pCanHandle);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Main routine for CCAN_ROM example
 * @return	Nothing
 */
int main(void)
{
	uint32_t maxParamSize;
	uint32_t status;
	CAN_API_INIT_PARAM_T myCANConfig = {
		0,
		LPC_C_CAN0_BASE,
		&gCANConfig,
		&callbacks,
		NULL,
		NULL,
	};

    /* For data rates above 100kHz DO NOT use RC as oscillator source. 
       I.e use default Xtal clock as main source */
	SystemCoreClockUpdate();
	Board_Init();

	/* Enable clocking for CAN and reset the controller */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CAN);
	Chip_SYSCTL_PeriphReset(RESET_CAN);

	/* Assign the pins rx 0[11] and tx 0[31] */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 31, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_SWM_MovablePinAssign(SWM_CAN_RD1_I, 11);	/* Rx P0.11 */
	Chip_SWM_MovablePinAssign(SWM_CAN_TD1_O, 31);	/* Tx P0.31 */

	myCANConfig.mem_base = (uint32_t) &gCANapiMem[0];
    
    /* Set the Can peripheral to run at 12Mhz. Was used to calculate the btr value in next line. */
    /* CPU runs from Xtal at 72Mhz so the divisor should be 6 but clkdiv is divisor - 1 */
	gCANConfig.clkdiv = 5;
	
	/* btr values can be obtained from www.bittiming.can-wiki.info  (use Bosch C_CAN / D_CAN) */
	gCANConfig.btr = 0x0502;	/* 500Khz bit rate  (%87.5 w 12Mhz base clock ) */
	gCANConfig.isr_ena = 1;

	/* Validate that we reserved enough memory */
	maxParamSize = LPC_CAND_API->hwCAN_GetMemSize(&myCANConfig);
	if (maxParamSize > MAX_CAN_PARAM_SIZE / 4) {
		while ( 1 ) {}
	}

	/* Initialize the ROM with specific configuration */
	status = LPC_CAND_API->hwCAN_Init(&pCanHandle, &myCANConfig);
	if (status != CAN_ERROR_NONE) {
		while (1) {
			__WFI();/* Go to Sleep */
		}
	}

	if (gCANConfig.isr_ena == TRUE) {
		NVIC_EnableIRQ(CAN_IRQn);
	}

	/* Send a simple one time CAN message */
	msg_obj.msgobj  = 0;
	msg_obj.mode_id = 0x345;
	msg_obj.mask    = 0x0;
	msg_obj.dlc     = 4 ;
	msg_obj.data[0] = 'T';	/* 0x54 */
	msg_obj.data[1] = 'E';	/* 0x45 */
	msg_obj.data[2] = 'S';	/* 0x53 */
	msg_obj.data[3] = 'T';	/* 0x54 */
	LPC_CAND_API->hwCAN_MsgTransmit(pCanHandle, &msg_obj);

	/* Configure message object 1 to receive all 11-bit messages 0x400-0x4FF */
	msg_obj.msgobj = 1;
	msg_obj.mode_id = 0x400;
	msg_obj.mask = 0x700;
	LPC_CAND_API->hwCAN_ConfigRxmsgobj(pCanHandle, &msg_obj);

	while (1) {
		__WFI();	/* Go to Sleep */
	}
}

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
#define OD_NONE 0x00 // Object Dictionary entry doesn't exist
#define OD_EXP_RO 0x10 // Object Dictionary entry expedited, read-only 
#define OD_EXP_WO 0x20 // Object Dictionary entry expedited, write-only
#define OD_EXP_RW 0x30 // Object Dictionary entry expedited, read-write 
#define OD_SEG_RO 0x40 // Object Dictionary entry segmented, read-only 
#define OD_SEG_WO 0x50 // Object Dictionary entry segmented, write-only 
#define OD_SEG_RW 0x60 // Object Dictionary entry segmented, read-write 

/**************************************************************************
SDO Abort Codes
**************************************************************************/ 
#define SDO_ABORT_TOGGLE          0x05030000UL  // Toggle bit not alternated
#define SDO_ABORT_SDOTIMEOUT      0x05040000UL  // SDO protocol timed out
#define SDO_ABORT_UNKNOWN_COMMAND 0x05040001UL  // Client/server command specifier not valid or unknown
#define SDO_ABORT_UNSUPPORTED     0x06010000UL  // Unsupported access to an object
#define SDO_ABORT_WRITEONLY       0x06010001UL  // Attempt to read a write only object
#define SDO_ABORT_READONLY        0x06010002UL  // Attempt to write a read only object
#define SDO_ABORT_NOT_EXISTS      0x06020000UL  // Object does not exist in the object dictionary
#define SDO_ABORT_PARAINCOMP      0x06040043UL  // General parameter incompatibility reason
#define SDO_ABORT_ACCINCOMP       0x06040047UL  // General internal incompatibility in the device
#define SDO_ABORT_TYPEMISMATCH    0x06070010UL  // Data type does not match, length of service parameter does not match
#define SDO_ABORT_UNKNOWNSUB      0x06090011UL  // Sub-index does not exist
#define SDO_ABORT_VALUE_RANGE     0x06090030UL  // Value range of parameter exceeded (only for write access)
#define SDO_ABORT_TRANSFER        0x08000020UL  // Data cannot be transferred or stored to the application
#define SDO_ABORT_LOCAL           0x08000021UL  // Data cannot be transferred or stored to the application because of local control
#define SDO_ABORT_DEVSTAT         0x08000022UL  // Data cannot be transferred or stored to the application because of the present device state

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

/* CANopen Callback function prototypes */
uint32_t CANOPEN_sdo_exp_read (uint16_t index, uint8_t subindex);
uint32_t CANOPEN_sdo_exp_write(uint16_t index, uint8_t subindex, uint8_t *dat_ptr);

/* Publish CAN Callback Functions */
CAN_CALLBACKS callbacks = {
	CAN_rx,
	CAN_tx,
	CAN_error
};

/*	CANopen read-only (constant) Object Dictionary (OD) entries
	Used with Expidited SDO only. Lengths = 1/2/4 bytes */
CAN_ODCONSTENTRY myConstOD [] = {
	/* index, subindex,	length,	value */
	{ 0x1000, 0x00, 	4, 		0x54534554UL },  // "TEST"
	{ 0x1018, 0x00, 	1, 		0x00000003UL },
	{ 0x1018, 0x01, 	4, 		0x00000003UL },
	{ 0x2000, 0x00, 	1, 		(uint32_t)'M' },
};

/* Application variables used in variable OD */
uint8_t  error_register;
uint32_t device_id;
uint32_t fw_ver;
uint16_t param;

/*	CANopen list of variable Object Dictionary (OD) entries
	Expedited SDO with length=1/2/4 bytes */
CAN_ODENTRY myOD [] = {
	/* index, subindex,	access_type | length,	value_pointer */
	{ 0x1001, 0x00, 	OD_EXP_RO | 1,			(uint8_t *)&error_register },
	{ 0x1018, 0x02, 	OD_EXP_RO | 4,			(uint8_t *)&device_id },
	{ 0x1018, 0x03, 	OD_EXP_RO | 4,			(uint8_t *)&fw_ver },
	{ 0x2001, 0x00, 	OD_EXP_RW | 2,			(uint8_t *)&param },
};

const CANOPEN_CALLBACKS co_callbacks = {
	CANOPEN_sdo_exp_read,
	CANOPEN_sdo_exp_write,
	NULL,
	NULL,
	NULL
};

/* CANopen configuration structure */
static const CAN_CANOPENCFG myCANopen = {
	0x14,									// node_id
	5,										// msgobj_rx
	6,										// msgobj_tx
	1,										// isr_handled
	sizeof(myConstOD)/sizeof(myConstOD[0]),	// od_const_num 
	(CAN_ODCONSTENTRY *)myConstOD,			// od_const_table
	sizeof(myOD)/sizeof(myOD[0]),			// od_num
	(CAN_ODENTRY *)myOD,					// od_table
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
{
	return;
}

static void CAN_error(uint32_t error_info)
{
	return;
}

void CAN_IRQHandler(void)
{
	LPC_CAND_API->hwCAN_Isr(pCanHandle);
}

/* CANopen Callback for expedited read accesses */
uint32_t CANOPEN_sdo_exp_read(uint16_t index, uint8_t subindex){
	if (index == 0x2001) {
		param++;
	}

	return 0;  // Return 0 for successs, SDO Abort code for error
}

/* CANopen Callback for expedited write accesses */
uint32_t CANOPEN_sdo_exp_write(uint16_t index, uint8_t subindex, uint8_t *dat_ptr){
  if ((index == 0x2001) && (*(uint16_t *)dat_ptr != 0)) {
    return SDO_ABORT_VALUE_RANGE;
  }
  else {
    return 0;  // Return 0 for successs, SDO Abort code for error
  }
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
		(CAN_CANOPENCFG *)&myCANopen,
		(CANOPEN_CALLBACKS *)&co_callbacks,
	};

     /* For data rates above 100kHz DO NOT use RC as oscillator source. 
       I.e use default Xtal clock as main source */
	SystemCoreClockUpdate();
	Board_Init();

	/* Enable clocking for CAN and reset the controller */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_CAN);
	Chip_SYSCTL_PeriphReset(RESET_CAN);

	/* Assign the pins rx 0.[11] and tx 0.[31] for LPC 1549 Xpresso V2 board*/
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 31, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_SWM_MovablePinAssign(SWM_CAN_RD1_I, 11);	/* Rx P0.11 */
	Chip_SWM_MovablePinAssign(SWM_CAN_TD1_O, 31);	/* Tx P0.31 */

	myCANConfig.mem_base = (uint32_t) &gCANapiMem[0];
    
    /* Set the Can peripheral to run at 12Mhz. Was used to calculate the btr value in next line. */
    /* CPU runs from Xtal at 72Mhz so target divisor = 6 (clkdiv + 1 = target divisor) */
	gCANConfig.clkdiv = 5;
	
	/* btr values can be obtained from www.bittiming.can-wiki.info  (use Bosch C_CAN / D_CAN) */
	gCANConfig.btr = 0x0502;	/* 500Khz bit rate  (%87.5 w 12Mhz base clock ) */
	gCANConfig.isr_ena = 1;

	/* Initialize CANopen handler and variables */
	error_register = 0x00;
	device_id      = 0xAA55AA55;
	fw_ver         = 0x00010002;
	param          = 33;
	
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

	/* Enable IRQ if applicable */
	if (gCANConfig.isr_ena == TRUE) {
		NVIC_EnableIRQ(CAN_IRQn);
	}

	/* Send a simple one time CAN message */
	msg_obj.msgobj  = 0;
	msg_obj.mode_id = 0x700+0x20;
	msg_obj.mask    = 0x0;
	msg_obj.dlc     = 4;
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

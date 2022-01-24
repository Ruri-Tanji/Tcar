﻿/*
 *----------------------------------------------------------------------
 *    Device Driver for micro T-Kernel for μT-Kernel 3.00.03
 *
 *    Copyright (C) 2020-2021 by Ken Sakamura.
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2021/03/31.
 *
 *----------------------------------------------------------------------
 */


#include <sys/machine.h>
#ifdef CPU_STM32L4
#include "../../../config/devconf.h"
#if DEVCNF_DEV_SER
/*
 *	ser_stm32l4.c
 *	Serial communication device driver
 *	System dependent processing for STM32L4
 */
#include <tk/tkernel.h>
#include "../../ser.h"

/*----------------------------------------------------------------------
 * Device register base address
 */
const LOCAL UW ba[DEV_SER_UNITNM] = { USART1_BASE, USART2_BASE, USART3_BASE};

#define	USART_CR1(x)	(ba[x] + USARTx_CR1)	/* Control register 1 */
#define	USART_CR2(x)	(ba[x] + USARTx_CR2)	/* Control register 2 */
#define	USART_CR3(x)	(ba[x] + USARTx_CR3)	/* Control register 3 */
#define	USART_BRR(x)	(ba[x] + USARTx_BRR)	/* baud rate register */
#define	USART_GTPR(x)	(ba[x] + USARTx_GTPR)	/* Guard time and Priscaler register */
#define	USART_RTOR(x)	(ba[x] + USARTx_RTOR)	/* Receiver timeout register */
#define	USART_RQR(x)	(ba[x] + USARTx_RQR)	/* Request register */
#define	USART_ISR(x)	(ba[x] + USARTx_ISR)	/* Interrupts and status register */
#define	USART_ICR(x)	(ba[x] + USARTx_ICR)	/* Interrupt flag clear register */
#define	USART_RDR(x)	(ba[x] + USARTx_RDR)	/* Received data register */
#define	USART_TDR(x)	(ba[x] + USARTx_TDR)	/* Transmission data register */

/*----------------------------------------------------------------------
 * Device low-level control data
*/
typedef struct {
	UW	mode;		// Serial mode
	UW	speed;		// Spped (bit rate)
} T_DEV_SER_LLDEVCB;

LOCAL T_DEV_SER_LLDEVCB		ll_devcb[DEV_SER_UNITNM];

/*----------------------------------------------------------------------
 * Interrupt handler
 */
void usart_inthdr( UINT intno)
{
	UW	data, err;
	UW	isr;
	W	unit;

	unit = intno -INTNO_USART1;
	
	isr = in_w(USART_ISR(unit));			// Get interrupt factor

	out_w(USART_ICR(unit), USART_ICR_ALL);		// Clear Interrupt
	ClearInt(intno);

	/* Reception process */
	if( isr & USART_ISR_RXNE) {
		data = in_w(USART_RDR(unit));
		dev_ser_notify_rcv(unit, data);	/* Notify the main process of data reception. */
	}

	/* Transmission process */
	if( isr & USART_ISR_TXE ) {
		if( dev_ser_get_snddat(unit, &data)) {
			out_w(USART_TDR(unit), data);
		} else {
			*(_UW*)( USART_CR1(unit)) &= ~USART_CR1_TXEIE;	// Disable TXE interrupt
		}
	}

	/* Error handling */
	err = isr & USART_ISR_ERR;
	if(err) {
		dev_ser_notify_err(unit, err);	/* Notify the main process of this error. */
	}
}

/*----------------------------------------------------------------------
 * Set mode & Start communication
 */
#define	MASK_MODE_CR1	(USART_CR1_M1| USART_CR1_M0|USART_CR1_PS|USART_CR1_PCE)

LOCAL void start_com(UW unit, UW mode, UW speed)
{


	out_w(USART_CR2(unit), mode & USART_CR2_STOP);		// Set stop-bit
	out_w(USART_CR3(unit), mode & (USART_CR3_RTSE | USART_CR3_CTSE)); // Set RTS/CTS
	out_w(USART_BRR(unit), speed);				// Set communication Speed

	/* Set mode & Start communication */
	out_w(USART_CR1(unit),
		USART_CR1_RXNEIE | USART_CR1_PEIE		// Unmask Receive & Parity error interrupt
		| (mode & MASK_MODE_CR1 )			// Set word length & parity
		| USART_CR1_UE | USART_CR1_RE | USART_CR1_TE	// USART enable
	);
}

/*----------------------------------------------------------------------
 * Stop communication
 */
LOCAL void stop_com(UW unit)
{
	if(unit != DEVCNF_SER_DBGUN) {
		out_w(USART_CR1(unit), 0);
	} else {	/* Used by T-Monitor */
		out_w(USART_CR1(unit), USART_CR1_DEBUG);
	}
}

/*----------------------------------------------------------------------
 * Low level device control
 */
EXPORT ER dev_ser_llctl( UW unit, INT cmd, UW parm)
{
	ER	err	= E_OK;

	switch(cmd) {
	case LLD_SER_MODE:	/* Set Communication mode */
		ll_devcb[unit].mode = parm;
		break;
	
	case LLD_SER_SPEED:	/* Set Communication Speed */
		ll_devcb[unit].speed = (TMCLK*1000*1000)/parm;
		break;
	
	case LLD_SER_START:	/* Start communication */
		out_w(USART_CR1(unit), 0);
		out_w(USART_ICR(unit), USART_ICR_ALL);			// Clear interrupt
		ClearInt(INTNO_USART1 + unit);
		EnableInt(INTNO_USART1 + unit, DEVCNF_SER_INTPRI);	// Enable Interrupt
		start_com( unit, ll_devcb[unit].mode, ll_devcb[unit].speed);
		break;
	
	case LLD_SER_STOP:
		DisableInt(INTNO_USART1 + unit);
		stop_com(unit);
		break;

	case LLD_SER_SEND:
		if(in_w(USART_ISR(unit)) & USART_ISR_TXE) {
			out_w(USART_TDR(unit), parm);			// Set Transmission data
			*(_UW*)( USART_CR1(unit)) |= USART_CR1_TXEIE;	// Enable TXE interrupt
			err = E_OK;
		} else {
			err = E_BUSY;
		}
		break;

	case LLD_SER_BREAK:	/* Send Break */
		if(parm) {
			out_w(USART_RQR(unit), USART_RQR_SBKRQ);
		}
		break;
	}

	return err;
}

/*----------------------------------------------------------------------
 * Device initialization
 */
EXPORT ER dev_ser_llinit( T_SER_DCB *p_dcb)
{
	const T_DINT	dint = {
		.intatr	= TA_HLNG,
		.inthdr	= usart_inthdr,
	};
	UW	unit;
	ER	err;

	unit = p_dcb->unit;

#if DEVCONF_SER_INIT_MCLK
	switch(unit) {
	case 0:	// USART1
		*(_UW*)RCC_APB2ENR |= RCC_APB2ENR_USART1EN;
		break;
	case 1:	// USART2
		*(_UW*)RCC_APB1ENR1 |= RCC_APB1ENR1_USART2EN;
		break;
	case 2:	// USART3
		*(_UW*)RCC_APB1ENR1 |= RCC_APB1ENR1_USART3EN;
		break;
	}
#endif

	/* USART device initialize (Disable USART & Disable all interrupt) */
	stop_com(unit);

	/* Device Control block Initizlize */
	p_dcb->intno_rcv = p_dcb->intno_snd = INTNO_USART1 + unit;

	/* Interrupt handler definition */
	err = tk_def_int((INTNO_USART1 + unit), &dint);

	return err;
}

#endif		/* DEVCNF_DEV_SER */
#endif		/* CPU_STM32L4 */
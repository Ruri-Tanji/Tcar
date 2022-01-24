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


/*
 *	ser_stm32l4.h
 *	Serial communication device driver
 *	System-dependent definition for STM32L4
 */

#ifndef	__DEV_SER_STM32L4_H__
#define	__DEV_SER_STM32L4_H__

#define	DEV_SER_UNITNM	(3)	/* Number of devive channels */
#define DEV_SER_UNIT0	(0)	/* Ch.0 - USART1 */
#define DEV_SER_UNIT1	(1)	/* Ch.1 - USART2 */
#define DEV_SER_UNIT2	(2)	/* Ch.2 - USART3 */

/*
 * USART registers
 */

/* Base address for each channel */
#define	USART1_BASE	(0x40013800UL)
#define	USART2_BASE	(0x40004400UL)
#define	USART3_BASE	(0x40004800UL)

/* Register offset */
#define	USARTx_CR1	(0x0000)	/* Control register 1 */
#define	USARTx_CR2	(0x0004)	/* Control register 2 */
#define	USARTx_CR3	(0x0008)	/* Control register 3 */
#define	USARTx_BRR	(0x000C)	/* baud rate register */
#define	USARTx_GTPR	(0x0010)	/* Guard time and Priscaler register */
#define	USARTx_RTOR	(0x0014)	/* Receiver timeout register */
#define	USARTx_RQR	(0x0018)	/* Request register */
#define	USARTx_ISR	(0x001C)	/* Interrupts and status register */
#define	USARTx_ICR	(0x0020)	/* Interrupt flag clear register */
#define	USARTx_RDR	(0x0024)	/* Received data register */
#define	USARTx_TDR	(0x0028)	/* Transmission data register */

/* Register bit definition */
#define	USART_CR1_UE		(1<<0)	/* RW USART enable */
#define	USART_CR1_UESM		(1<<1)	/* RW USART enable in STOP mode */
#define	USART_CR1_RE		(1<<2)	/* RW Receiver enable */
#define	USART_CR1_TE		(1<<3)	/* RW Transmitter enable */
#define	USART_CR1_IDLEIE	(1<<4)	/* RW IDLE interrupt enable */
#define	USART_CR1_RXNEIE	(1<<5)	/* RW RXNE interrupt enable */
#define	USART_CR1_TCIE		(1<<6)	/* RW TCOE interrupt enable */
#define	USART_CR1_TXEIE		(1<<7)	/* RW TXE interrupt enable */
#define	USART_CR1_PEIE		(1<<8)	/* RW PE interrupt enable */
#define	USART_CR1_PS		(1<<9)	/* RW Parity selection */
#define	USART_CR1_PCE		(1<<10)	/* RW Parity control enable */
#define	USART_CR1_WAKE		(1<<11)	/* RW Receive wakeup method */
#define	USART_CR1_M0		(1<<12)	/* RW Word length 0 */
#define	USART_CR1_MME		(1<<13)	/* RW Mute mode enable */
#define	USART_CR1_CMIE		(1<<14)	/* RW Character match interrupt enable */
#define	USART_CR1_OVER8		(1<<15)	/* RW Oversampling mode */
#define	USART_CR1_RTOIE		(1<<26)	/* RW Receiver timeout interrupt enable */
#define	USART_CR1_EOBIE		(1<<27)	/* RW End of Block interrupt enabled */
#define	USART_CR1_M1		(1<<28)	/* RW Word length 1 */

#define	USART_CR2_STOP		(3<<12)	/* RW Stop-bit */

#define	USART_CR3_RTSE		(1<<8)	/* RTS enable */
#define	USART_CR3_CTSE		(1<<9)	/* CTS enable */

#define	USART_RQR_SBKRQ		(1<<1)	/* Break send request */

#define	USART_ISR_PE		(1<<0)	/* R Parity error */
#define	USART_ISR_FE		(1<<1)	/* R Framing error */
#define	USART_ISR_NF		(1<<2)	/* R Start bit noise detection */
#define	USART_ISR_ORE		(1<<3)	/* R Overrun error */
#define	USART_ISR_IDLE		(1<<4)	/* R Idle line detection */
#define	USART_ISR_RXNE		(1<<5)	/* R Received data register not empty */
#define	USART_ISR_TC		(1<<6)	/* R Transmission completely */
#define	USART_ISR_TXE		(1<<7)	/* R Transmission data register empty */
#define	USART_ISR_LBDF		(1<<8)	/* R LIN break detection */
#define	USART_ISR_CTSIF		(1<<9)	/* R CTS interrupt */
#define	USART_ISR_CTS		(1<<10)	/* R CTS flag */
#define	USART_ISR_RTOF		(1<<11)	/* R Receiver timeout */
#define	USART_ISR_EOBF		(1<<12)	/* R End of block */
#define	USART_ISR_ABRE		(1<<14)	/* R Automatic baud rate error */
#define	USART_ISR_ABRF		(1<<15)	/* R Automatic baud rate flag */
#define	USART_ISR_BUSY		(1<<16)	/* R Busy flag */
#define	USART_ISR_CMF		(1<<17)	/* R Character match flag */
#define	USART_ISR_SBKF		(1<<18)	/* R Break transmission flag */
#define	USART_ISR_RWU		(1<<6)	/* R Receiver mute mode flag */
#define	USART_ISR_WUF		(1<<6)	/* R Wakeup flag */
#define	USART_ISR_TEACK		(1<<6)	/* R Transmission ACK flag */
#define	USART_ISR_REACK		(1<<6)	/* R Receive ACK flag */
#define	USART_ISR_ERR		(USART_ISR_PE|USART_ISR_FE|USART_ISR_NF|USART_ISR_ORE)

#define	USART_ICR_PECF		(1<<0)
#define	USART_ICR_FECF		(1<<1)
#define	USART_ICR_NCF		(1<<2)
#define	USART_ICR_ORECF		(1<<3)
#define	USART_ICR_IDLECF	(1<<4)
#define	USART_ICR_TCCF		(1<<6)
#define	USART_ICR_TCBGTCF	(1<<7)
#define	USART_ICR_LBDCF		(1<<8)
#define	USART_ICR_CTSCF		(1<<9)
#define	USART_ICR_RTOCF		(1<<11)
#define	USART_ICR_EOBCF		(1<<12)
#define	USART_ICR_CMCF		(1<<17)
#define	USART_ICR_WUCF		(1<<20)
#define	USART_ICR_ALL		(0x00121BDF)

/*Initial register value when using debug */
#define USART_CR1_DEBUG		0x0000000D	/* USART enable, 8bit, Non parity */
#define USART_CR2_DEBUG		0		/* Stop bit 1 */
#define USART_CR3_DEBUG		0		/* No hard flow control */


/* USART interrupt number */
#define	INTNO_USART1		37
#define	INTNO_USART2		38
#define	INTNO_USART3		39

#endif		/* __DEV_SER_STM32L4_H__ */

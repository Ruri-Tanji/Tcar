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
 *	i2c_stml4.h
 *	I2C device driver
 *	System-dependent definition for STM32L4
 */

#ifndef	__DEV_I2C_STM32L4_H__
#define	__DEV_I2C_STM32L4_H__

#define	DEV_I2C_UNITNM		(3)	/* Number of devive units */
#define	DEV_I2C_1		(0)	/* I2C1 */
#define	DEV_I2C_2		(1)	/* I2C2 */
#define	DEV_I2C_3		(2)	/* I2C3 */

/* I2C device driver operating state */
#define	I2C_STS_START		0x0000
#define	I2C_STS_RESTART		0x0001
#define	I2C_STS_STOP		0x0003
#define	I2C_STS_SEND		0x0004
#define	I2C_STS_RECV		0x0005
#define	I2C_STS_TOP		0x8000

/*
 * I2C Clock source
 */
#define	RCC_CCIPR_I2CSEL	0x00003000	// I2C Clock source

/*----------------------------------------------------------------------
 * I2C registers
 */

/* Base address */
#define	I2C1_BASE		0x40005400
#define	I2C2_BASE		0x40005800
#define	I2C3_BASE		0x40005C00

/* Register offset */
#define	I2Cx_CR1		(0x00)		// Control register 1
#define	I2Cx_CR2		(0x04)		// Control register 2
#define	I2Cx_OAR1		(0x08)		// Own adress 1 register
#define	I2Cx_OAR2		(0x0C)		// Own adress 2 register
#define	I2Cx_TIMINGR		(0x10)		// Timing register
#define	I2Cx_TIMEOUTR		(0x14)		// Timeout register
#define	I2Cx_ISR		(0x18)		// Interrupt & status register
#define	I2Cx_ICR		(0x1C)		// Interrupt clear register
#define	I2Cx_PECR		(0x20)		// reserve
#define	I2Cx_RXDR		(0x24)		// Receive data register
#define	I2Cx_TXDR		(0x28)		// Transmission data register

#define I2C_CR1_PE		(1<<0)		// Peripheral enable
#define	I2C_CR1_TXIE		(1<<1)		// TX interrupt enable
#define	I2C_CR1_RXIE		(1<<2)		// RX interrupt enable
#define	I2C_CR1_NACKIE		(1<<4)		// NACK interrupt enable
#define	I2C_CR1_STOPIE		(1<<5)		// STOP interrupt enable
#define	I2C_CR1_TCIE		(1<<6)		// Transfer complete interrupt enable
#define	I2C_CR1_ERRIE		(1<<7)		// Error interrupt enable

#define	I2C_CR2_RD_WRN		(1<<10)		// Communication direction
#define	I2C_CR2_START		(1<<13)		// START
#define	I2C_CR2_AUTOEND		(1<<25)		// Auto end mode

#define	I2C_ISR_TXE		(1<<0)		// Transfer data empty
#define	I2C_ISR_TXIS		(1<<1)		// Transfer interrupt
#define	I2C_ISR_RXNE		(1<<2)		// Receive data not empty
#define	I2C_ISR_NACKF		(1<<4)		// NACK interrupt
#define	I2C_ISR_STOPF		(1<<5)		// STOP interrupt
#define	I2C_ISR_TC		(1<<6)		// Transfer complete interrupt
#define	I2C_ISR_TCR		(1<<7)		// Transfer complete & reload
#define	I2C_ISR_BUSY		(1<<15)		// Bus busy

#define	I2C_ICR_ALL		(0x00003F38)	// All flag clear

/* 
 * I2C interrupt number
  */
#define	INTNO_I2C1_EV		31		// I2C1 event interrupt
#define	ININO_I2C1_ER		32		// I2C1 error interrupt

#define	INTNO_I2C2_EV		33		// I2C2 event interrupt
#define	ININO_I2C2_ER		34		// I2C2 error interrupt

#define	INTNO_I2C3_EV		72		// I2C3 event interrupt
#define	ININO_I2C3_ER		73		// I2C3 error interrupt

#endif		/* __DEV_I2C_STM32L4_H__ */

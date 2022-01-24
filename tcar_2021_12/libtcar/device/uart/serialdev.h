/*
 *----------------------------------------------------------------------
 *    UCT micro T-Kernel Version 2.00.15
 *
 *    Copyright (c) 2011-2015 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 *
 *    Released by UC Technology (http://www.uctec.com/) at 2012/02/17.
 *    Modified by UC Technology at 2013/01/07.
 *    Modified by UC Technology at 2014/01/23.
 *    Modified by UC Technology at 2014/03/20.
 *    Modified by UC Technology at 2015/01/30.
 *    Modified by UC Technology at 2015/06/04.
 *    Modified by UC Technology at 2015/08/10.
 *    Modified by Imagination Technology on 2015/10/23 for PIC32MZ.
 *    Modified by UC Technology at 2016/03/23.
 *    Modified by UC Technology at 2016/07/04.
 *
 *----------------------------------------------------------------------
 */

#ifndef __DEVICE_SERIALDEV_H__
#define __DEVICE_SERIALDEV_H__

#include <tk/tkernel.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Start : MCU dependent */
#if defined(APP_MB9AF312K)
#define	RS_DEVNM		"rsa"

#elif defined(MCB_M362)
#define	RS_DEVNM		"rsa"

#elif defined(SK_M369)
#define	RS_DEVNM		"rsa"

#elif defined(EBD_M440)
#define	RS_DEVNM		"rsa"

#elif defined(EBD_TMPM46BF10FG)
#define	RS_DEVNM		"rsa"

#elif defined(SK_M440)
#define	RS_DEVNM		"rsa"

#elif defined(FRDM_K64F)
#define	RS_DEVNM		"rsa"

#elif defined(TWR_K70F120M)
#define	RS_DEVNM		"rsc"

#elif defined(TWR_K60N512)
#define	RS_DEVNM		"rsd"

#elif defined(TWR_K60F120M)
#define	RS_DEVNM		"rsf"

#elif defined(TWR_K60D100M)
#define	RS_DEVNM		"rsd"

#elif defined(EVAL_STM3221G)
#define	RS_DEVNM		"rsd"

#elif defined(EVAL_STM32437I)
#define	RS_DEVNM		"rsd"

#elif defined(EVAL2_STM32439I)
#define	RS_DEVNM		"rsa"

#elif defined(MCB_MB9BF500)
#define	RS_DEVNM		"rsa"

#elif defined(MCB_MB9AFB44)
#define	RS_DEVNM		"rsg"

#elif defined(MCB_MB9AF156M)
#define	RS_DEVNM		"rsa"

#elif defined(SKFM4_MB9BF568R)
#define	RS_DEVNM		"rsa"

#elif defined(SKFM4_S6E2CC)
#define	RS_DEVNM		"rsa"

#elif defined(SKFM0_S6E1B8)
#define	RS_DEVNM		"rsa"

#elif defined(AP_RX63N)
#define RS_DEVNM		"rsa"

#elif defined(GR_RX63N)
#define RS_DEVNM		"rsa"

#elif defined(ASURA_RX62N)
#define RS_DEVNM		"rsg"

#elif defined(AP_RX62N)
#define RS_DEVNM		"rsa"

#elif defined(HSB_RX62N)
#define RS_DEVNM		"rsg"

#elif defined(FE_RX2108)
#define RS_DEVNM		"rsa"

#elif defined(SK_PIC32MX)
#define	RS_DEVNM		"rsa"

#elif defined(SK_PIC32MZ)
#define	RS_DEVNM		"rsa"
#endif
/* End : MCU dependent */

#define	_USE_FIFO_		(0)

typedef enum {
	/* common attribute */
	DN_PCMCIAINFO	= TDN_PCMCIAINFO,
	
	/* individual attribute */
	DN_RSMODE	= -121,	/* mode of transport */
	DN_RSFLOW	= -122,	/* flow control */
	DN_RSSTAT	= -123,	/* line condition */
	DN_RSBREAK	= -124,	/* sending "break" */
	DN_RSSNDTMO	= -125,	/* timeout of sending */
	DN_RSRCVTMO	= -126,	/* timeout of receiving */
	DN_RSADDIN	= -127,	/* additional functions(N/A) */
	
	/* individual attribute for an IBM keyboard(N/A) */
	DN_IBMKB_KBID	= -200,	/* ID for the keyboard(N/A) */
	
	/* individual attribute for a touchpad(N/A) */
	DN_TP_CALIBSTS	= -200,	/* a state of calibration(N/A) */
	DN_TP_CALIBPAR	= -201,	/* a parameter for calibration(N/A) */
	
	/* individual attribute for hardware */
	DN_RS16450	= -300	/* set up for the hardware */
} RSDataNo;


typedef enum {
	RS_ABORT	= 0,
	RS_SUSPEND	= -200,
	RS_RESUME	= -201,
	RS_RCVBUFSZ	= -202,
	RS_LINECTL	= -203,
	RS_EXTFUNC	= -9999	/* N/A */
} SerialControlNo;
	
typedef struct {
	UB		major;
	UB		minor;
	UB		info[40];
} PCMCIAInfo;
	
	
typedef struct {
	UW		parity:2;	/* 0:None, 1:odd, 2:even, 3:- */
	UW		datalen:2;	/* 0:5bit, 1:6bit, 2:7bit, 3:8bit */
	UW		stopbits:2;	/* 0:1bit, 1:1.5bit, 2:2bit, 3:- */
	UW		rsv:2;
	UW		baud:24;	/* baud late */
} RsMode;	
	
	
typedef struct {
	UW		rsv:26;		/* reserved */
	UW		rcvxoff:1;	/* XOFF */
	UW		csflow:1;	/* CTS control */
	UW		rsflow:1;	/* RTS control */
	UW		xonany:1;	/* XON */
	UW		sxflow:1;	/* send XON/XOFF control */
	UW		rxflow:1;	/* receive XON/XOFF control */
} RsFlow;

#if defined(EBD_M440)||defined(SK_M440)
typedef struct {
	UW		BE;		/* Recv Buffer Overflow Error*/
	UW		FE;		/* Framing Error */
	UW		OE;		/* Overrun Error */
	UW		PE;		/* Parity Error */
} RsStat;
#else
typedef struct {
#if BIGENDIAN
	UW		rsv1:20;
	UW		BE:1;		/* Recv Buffer Overflow Error*/
	UW		FE:1;		/* Framing Error */
	UW		OE:1;		/* Overrun Error */
	UW		PE:1;		/* Parity Error */
	UW		rsv2:2;
	UW		XF:1;		/* Recv XOFF */
	UW		BD:1;		/* Break Detect */
	UW		DR:1;		/* Dataset Ready (DSR) */
	UW		CD:1;		/* Carrier Detect (DCD) */
	UW		CS:1;		/* Clear to Send (CTS) */
	UW		CI:1;		/* Calling Indicator(RI)*/
#else
	UW		CI:1;		/* Calling Indicator(RI)*/
	UW		CS:1;		/* Clear to Send (CTS) */
	UW		CD:1;		/* Carrier Detect (DCD) */
	UW		DR:1;		/* Dataset Ready (DSR) */
	UW		BD:1;		/* Break Detect */
	UW		XF:1;		/* Recv XOFF */
	UW		rsv2:2;
	UW		PE:1;		/* Parity Error */
	UW		OE:1;		/* Overrun Error */
	UW		FE:1;		/* Framing Error */
	UW		BE:1;		/* Recv Buffer Overflow Error*/
	UW		rsv1:20;
#endif
} RsStat;
#endif

typedef struct {
#if BIGENDIAN
	UW		ErrorClass:16;	/* Error class = EC_IO */
	UW		rsv1:2;
	UW		Aborted:1;	/* ABBORT has occured */
	UW		Timout:1;	/* Timeout has occured */
	UW		BE:1;		/* Recv Buffer Overflow Error*/
	UW		FE:1;		/* Framing Error */
	UW		OE:1;		/* Overrun Error */
	UW		PE:1;		/* Parity Error */
	UW		rsv2:2;
	UW		XF:1;		/* Recv XOFF */
	UW		BD:1;		/* Break Detect */
	UW		DR:1;		/* Dataset Ready (DSR) */
	UW		CD:1;		/* Carrier Detect (DCD) */
	UW		CS:1;		/* Clear to Send (CTS) */
	UW		CI:1;		/* Calling Indicator(RI)*/
#else
	UW		CI:1;		/* Calling Indicator(RI)*/
	UW		CS:1;		/* Clear to Send (CTS) */
	UW		CD:1;		/* Carrier Detect (DCD) */
	UW		DR:1;		/* Dataset Ready (DSR) */
	UW		BD:1;		/* Break Detect */
	UW		XF:1;		/* Recv XOFF */
	UW		rsv2:2;
	UW		PE:1;		/* Parity Error */
	UW		OE:1;		/* Overrun Error */
	UW		FE:1;		/* Framing Error */
	UW		BE:1;		/* Recv Buffer Overflow Error*/
	UW		Timout:1;	/* Timeout has occured */
	UW		Aborted:1;	/* ABBORT has occured */
	UW		rsv1:2;
	UW		ErrorClass:16;	/* Error class = EC_IO */
#endif
} RsError;
#ifdef __cplusplus
}
#endif

#endif	/* __DEVICE_SERIALDEV_H__ */

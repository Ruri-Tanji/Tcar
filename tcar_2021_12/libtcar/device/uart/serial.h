/*
 *----------------------------------------------------------------------
 *    UCT micro T-Kernel 2.0
 *
 *    Copyright (c) 2016 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 *
 *    Released by UC Technology (http://www.uctec.com/) at 2016/09/30.
 *
 *----------------------------------------------------------------------
 */

/*
 *	serial.h
 *
 *	シリアルドライバ
 */

#ifndef	__DEVICE_SERIAL_H__
#define	__DEVICE_SERIAL_H__

#include <tk/tkernel.h>
#include "serialdev.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0 /* serialdev.h に定義あり */
typedef enum {
	RS_ABORT	= 0,
	RS_SUSPEND	= -200,
	RS_RESUME	= -201,
	RS_RCVBUFSZ	= -202,
	RS_LINECTL	= -203,
} SerialControlNo;
#endif

/* 制御線 ON/OFF パラメータ */
#define	RSCTL_DTR	0x00000001	/* DTR 信号 */
#define	RSCTL_RTS	0x00000002	/* RTS 信号 */
#define	RSCTL_SET	0x00000000	/* 全信号の設定 */
#define	RSCTL_ON	0xc0000000	/* 指定信号の ON */
#define	RSCTL_OFF	0x80000000	/* 指定信号の OFF */

IMPORT ER serial_init(W port);

/* 以下の関数は T-Engine 標準デバイスドライバに従う */
IMPORT ER serial_in(W port, B* buf, W len, W *alen, W tmout);
IMPORT ER serial_out(W port, B* buf, W len, W *alen, W tmout);
IMPORT ER serial_ctl(W port, W kind, UW *arg);

#ifdef __cplusplus
}
#endif
#endif /* __DEVICE_SERIAL_H__ */

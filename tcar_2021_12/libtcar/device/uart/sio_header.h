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
 *	sio_header.c
 *
 *	UART ドライバー共通定義
 */

#ifndef	__SIO_HEADER_H__
#define	__SIO_HEADER_H__

#include <tk/tkernel.h>
#include "serial.h"

/* モニタコンソールとして使用するポート */
#define	STD_PORT_NUMBER		(5)
#define	TERM_PORT		STD_PORT_NUMBER

/* UART 定義テーブル */
IMPORT	const	UW	uart_reg_base[];

typedef struct uartdrv	UartDrv;

/*
 * 管理情報
 */
typedef struct {
	void	*exinf;		/* 拡張情報 */

	W	ch;		/* UART チャンネル番号 (0〜5) */
	UartDrv	*drv;		/* UART ドライバー */

	RsMode	rsmode;		/* 通信モード */
	RsFlow	rsflow;		/* フロー制御 */

	W	waitcnt;	/* 順番待ちカウント */
	UW	execptn;	/* 実行中フラグ */
	UW	abortptn;	/* アボート要求フラグ */
	W	suspend;	/* サスペンド中 */
} UartInfo;

/* UART ドライバー定義 */
struct uartdrv {
	ER (*serial_in)( W port, B* buf, W len, W *alen, W tmout, UartInfo* );
	ER (*serial_out)( W port, B* buf, W len, W *alen, W tmout, UartInfo* );
	ER (*serial_ctl)( W port, W kind, UW *arg, UartInfo* );
	ER (*serial_init)( W port, UartInfo* );
	W  (*rsstat)( W clear, UartInfo* );
	ER (*suspend)( BOOL sus, UartInfo* );
};

IMPORT UartDrv	Uart_m;		/* UART (modem) */
IMPORT UartDrv	Uart_s;		/* SIO/UART */

/*
 * 同期用イベントフラグ
 */
IMPORT ID	uartflgid;

#define	SEND_DATA(ch)	( 0x00000001 << (ch) )	/* 送信完了 */
#define	RECV_DATA(ch)	( 0x00000100 << (ch) )	/* 受信あり */
#define	SEND_CALL(ch)	( 0x00010000 << (ch) )	/* serial_out() */
#define	RECV_CALL(ch)	( 0x01000000 << (ch) )	/* serial_in()  */

/*
 * RsError のビットマスク
 */
#define	RSERR_TMOUT	0x1000
#define	RSERR_ABORT	0x2000

#define	XON	('Q' - '@')	/* XON/XOFF フロー制御文字 */
#define	XOFF	('S' - '@')

IMPORT ER uart_abort( UartInfo* );
IMPORT ER uart_suspend( W kind, UartInfo* );

#endif	/* __SIO_HEADER_H__ */

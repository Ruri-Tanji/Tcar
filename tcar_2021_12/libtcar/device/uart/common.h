/*
 *----------------------------------------------------------------------
 *    UCT micro T-Kernel Version 2.00.12
 *
 *    Copyright (c) 2011-2015 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 *
 *    Modified by UC Technology at 2015/06/04.
 *
 *----------------------------------------------------------------------
 */

#ifndef __COMMON_H__
#define __COMMON_H__

/* COMMON MACRO */

#define	ON			TRUE
#define	OFF			FALSE
#define START			TRUE
#define STOP			FALSE

#define IGNORE			(-1)	/* Ignore setting, Do nothing!	*/

/* MISC */
#define	GETSUBUNITNO(devid)	((devid) & 0x000000FF)


/* CHECK */
#define CHK_RECSIZE		(1)

#if CHK_RECSIZE
#define CHECK_RECSIZE(type)					\
	if( size != sizeof(type) ){				\
		return E_PAR;					\
	}
#else /* CHK_PAR */
#define CHECK_RECSIZE(type)
#endif /* CHK_PAR */

/* Common Data Record Number for All Drivers */
typedef enum {
				/* デバイスに共通のレコード番号		*/
	DN_ENABLE	= -100,	/* RW	デバイスの動作制御		*/
	DN_INIT		= -101,	/* RW	デバイスの初期化		*/
	DN_RESET	= -102,	/* RW	デバイスのリセット		*/
	DN_START	= -103,	/* RW	処理の開始、停止		*/
	DN_IDLEMODE	= -104,	/* RW	IDLE中のデバイスの制御		*/
	DN_IOMODE	= -105,	/* RW	入出力モード			*/

				/* タイマ関連の共通レコード		*/
	DN_RUNSTATE	= -110,	/* RW	カウンタの動作状態指定		*/
	DN_SWCAPTURE	= -111,	/* RW	ソフトウェアキャプチャ		*/
} DrvCommonDataNo;

/* utilities */
IMPORT	int	printf( const char *format, ... );	/* --> drv/common */

#endif	/* __COMMON_H__ */

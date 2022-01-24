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
 *	sio_base.c
 *
 *	シリアルドライバー共通部
 */

#include "sio_header.h"
#include "uart_def.h"
#include <tk/tkernel.h>

#define	N_PORT		2	/* ポート数 */

/*
 * 排他制御用ロック
 */
LOCAL FastMLock	uartlock;

#define	LockUART(ui)	MLock(&uartlock, (ui)->ch)
#define	UnlockUART(ui)	MUnlock(&uartlock, (ui)->ch)

/*
 * 同期用イベントフラグ
 */
EXPORT ID	uartflgid;

/*
 * 管理情報
 */
LOCAL UartInfo	uartinfo[N_PORT] = {
	{	.ch = 5,	/* UART5 コンソール */
		.drv = &Uart_m,
		.rsmode = {
			.baud = 115200,
			.datalen = 3, .stopbits = 0, .parity = 0
		},
	},
	{	.ch = 1,	/* UART1 無線モジュール */
		.drv = &Uart_s,
		.rsmode = {
			.baud = 115200,
			.datalen = 3, .stopbits = 0, .parity = 0
		},
	}
};

/*
 * 管理情報の取得
 */
LOCAL UartInfo* getUartInfo( W port )
{
	UartInfo	*ui;

	if ( port < 0 || port >= N_PORT ) return NULL;

	ui = uartinfo + port;

	if ( ui->exinf == NULL ) return NULL; /* 未初期化 */

	return ui;
}

/* ------------------------------------------------------------------------ */

/*
 * アボート
 */
EXPORT ER uart_abort( UartInfo *ui )
{
	UW	ptn;

	ptn = ui->execptn & (RECV_CALL(ui->ch) | SEND_CALL(ui->ch));
	if ( ptn != 0 ) {
		ui->abortptn = ptn;
		/* xxx_CALL と xxx_DATA のイベントを両方セット */
		tk_set_flg(uartflgid, ptn | (ptn >> 16));
	}

	return E_OK;
}

/*
 * サスペンド／リジューム
 */
EXPORT ER uart_suspend( W kind, UartInfo *ui )
{
	W	ch = ui->ch;

	if ( kind == RS_SUSPEND ) {
		/* サスペンド */
		ui->suspend = 1;
		uart_abort(ui);

		/* 送受信停止 */
		ui->drv->suspend(TRUE, ui);
	} else {
		/* 送受信再開 */
		ui->drv->suspend(FALSE, ui);

		/* リジューム */
		ui->suspend = 0;
		if ( ui->waitcnt > 0 ) {
			tk_set_flg(uartflgid, RECV_CALL(ch) | SEND_CALL(ch));
		}
	}

	return E_OK;
}

/* ------------------------------------------------------------------------ */

/*
 * 送受信要求の順番待ち
 *	ptn = SEND_CALL または RECV_CALL
 */
LOCAL ER waitturn( UW ptn, TMO tmout, UartInfo *ui )
{
	UINT	flgptn;
	ER	err = E_OK;

	LockUART(ui);

	while ( (ui->execptn & ptn) != 0 || ui->suspend != 0 ) {
		/* 処理中なので順番待ち、またはサスペンド解除待ち */
		ui->waitcnt++;
		UnlockUART(ui);

		err = tk_wai_flg(uartflgid, ptn,
				 TWF_ORW|TWF_BITCLR, &flgptn, tmout);
		if ( err < E_OK ) {
			if ( err == E_TMOUT ) {
				err = E_IO | RSERR_TMOUT |
					ui->drv->rsstat(0, ui);
			}
		}

		LockUART(ui);
		ui->waitcnt--;
		if ( err < E_OK ) break;
	}
	if ( err >= E_OK ) {
		ui->execptn |= ptn;
	}

	UnlockUART(ui);

	return err;
}

/*
 * 送受信要求の順番待ちの起床
 *	ptn = SEND_CALL または RECV_CALL
 */
LOCAL void nextturn( UW ptn, UartInfo *ui )
{
	LockUART(ui);

	ui->execptn  &= ~ptn;
	ui->abortptn &= ~ptn;

	if ( ui->waitcnt > 0 ) {
		/* 順番待ちしているタスクを起床 */
		tk_set_flg(uartflgid, ptn);
	}

	UnlockUART(ui);
}

/*
 * 受信
 */
EXPORT ER serial_in( W port, B* buf, W len, W *alen, W tmout )
{
	UartInfo	*ui = getUartInfo(port);
	ER		err;

	if ( ui == NULL ) {
		err = E_PAR;
		goto err_ret;
	}
	if ( tmout < 0 ) tmout = TMO_FEVR;

	err = waitturn(RECV_CALL(ui->ch), tmout, ui);
	if ( err < E_OK ) goto err_ret;

	err = ui->drv->serial_in(port, buf, len, alen, tmout, ui);

	nextturn(RECV_CALL(ui->ch), ui);

	return err;

  err_ret:
	*alen = 0;
	return err;
}

/*
 * 送信
 */
EXPORT ER serial_out( W port, B* buf, W len, W *alen, W tmout )
{
	UartInfo	*ui = getUartInfo(port);
	ER		err;

	if ( ui == NULL || tmout == 0 ) {
		err = E_PAR;
		goto err_ret;
	}
	if ( tmout < 0 ) tmout = TMO_FEVR;

	if ( len <= 0 ) {
		*alen = 0;
		return E_OK;
	}

	err = waitturn(SEND_CALL(ui->ch), tmout, ui);
	if ( err < E_OK ) goto err_ret;

	err = ui->drv->serial_out(port, buf, len, alen, tmout, ui);

	nextturn(SEND_CALL(ui->ch), ui);

	return err;

  err_ret:
	*alen = 0;
	return err;
}

/*
 * 制御
 */
EXPORT ER serial_ctl( W port, W kind, UW *arg )
{
	UartInfo	*ui = getUartInfo(port);
	ER		err;

	if ( ui == NULL ) {
		return E_PAR;
	}

	LockUART(ui);

	err = ui->drv->serial_ctl(port, kind, arg, ui);

	UnlockUART(ui);

	return err;
}

/*
 * 初期設定
 */
EXPORT ER serial_init( W port )
{
	UartInfo	*ui;
	T_CFLG		cflg;
	ER		err;

	if ( port < 0 || port >= N_PORT ) return E_PAR;
	ui = uartinfo + port;

	/* 排他制御用ロックの生成 */
	if ( uartlock.id == 0 ) {
		err = CreateMLock(&uartlock, NULL);
		if ( err < E_OK ) goto err_ret;
	}

	/* イベントフラグ生成 */
	if ( uartflgid == 0 ) {
		cflg.exinf   = NULL;
		cflg.flgatr  = TA_TFIFO | TA_WMUL;
		cflg.iflgptn = 0;
		err = tk_cre_flg(&cflg);
		if ( err < E_OK ) goto err_ret;
		uartflgid = err;
	}

	/* ドライバー初期化 */
	err = ui->drv->serial_init(port, ui);
	if ( err < E_OK ) goto err_ret;

	return E_OK;

  err_ret:
	ui->exinf = NULL;
	return err;
}

/* ------------------------------------------------------------------------ */
/*
 *	モニターコンソール用シリアル入出力
 */

EXPORT	void	sendChar( const UB *buf )
{
	UW	imask;

	DI(imask);

	while ( (UARTxFR(TERM_PORT) & UARTxFR_TXFE) == 0 );
	UARTxDR(TERM_PORT) = *buf;
	while ( (UARTxFR(TERM_PORT) & UARTxFR_TXFE) == 0 );

	EI(imask);
}

EXPORT	UB	getChar( UB *buf )
{
	UW	imask;

	DI(imask);

	while ( (UARTxFR(TERM_PORT) & UARTxFR_RXFE) != 0 );
	*buf = UARTxDR(TERM_PORT) & 0xff;

	EI(imask);

	return *buf;
}

EXPORT	void	uartInit( void )
{
	UW	n;

	UARTxCR(TERM_PORT) = 0;
	UARTxDMACR(TERM_PORT) = 0;
	UARTxIMSC(TERM_PORT) = 0;
	UARTxICR(TERM_PORT) = UARTxRIS(TERM_PORT);

	n = CLOCK_fsys * (64*2 / 16) / 115200;
	n = (n + 1) >> 1;	/* 丸め(四捨五入) */
	UARTxIBDR(TERM_PORT) = n >> 6;
	UARTxFBDR(TERM_PORT) = n & 0x3f;

	/* data 8bit, stop 1bit, no parity */
	UARTxLCR_H(TERM_PORT) = UARTxLCR_H_WLEN(8);

	UARTxCR(TERM_PORT) =
		UARTxCR_RTS | UARTxCR_DTR |
		UARTxCR_RXE | UARTxCR_TXE | UARTxCR_UARTEN;
}

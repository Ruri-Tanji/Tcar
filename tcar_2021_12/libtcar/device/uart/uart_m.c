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
 *	uart_m.c
 *
 *	TMPM367 UART (modem)
 *	UART driver
 */

#include "sio_header.h"
#include "uart_def.h"

#include <sys/sysdef.h>

/*
 * 受信バッファ
 *	本機の RAM 容量が小さく、また本機の用途的にサイズを可変にする必要性
 *	がないと判断し、単純な固定長バッファとする。
 */
#define	RCVBUFSZ_MIN	64

#define	RCVTHRESHOLD_H	48	/* 停止:受信フロー制御閾値 (バイト数) */
#define	RCVTHRESHOLD_L	8	/* 再開:受信フロー制御閾値 (バイト数) */

/*
 * ドライバー情報
 */
typedef struct {
	UartInfo *ui;		/* 共通情報 */

	UW	rcvsts;		/* 受信エラー履歴 */
	UB	*rcvbuf;	/* 受信バッファ */
	W	rcvbufsz;	/* 受信バッファサイズ (バイト数) */
	W	rcvbuf_pos;	/* 受信データ先頭 (一番古いデータの位置) */
	W	rcvbuf_len;	/* 受信データ数 (バイト数) */

	UB	*snddat;	/* 送信データ位置 */
	W	snddat_len;	/* 残りの送信データ数 (バイト数) */
	W	sndxoff;	/* XON/XOFF 送信要求 */

	UB	_rcvbuf[RCVBUFSZ_MIN];
} DrvInfo;

#define	N_DRVINFO	1

LOCAL DrvInfo	drvinfo[N_DRVINFO];

/* ------------------------------------------------------------------------ */

LOCAL void send_data( DrvInfo *di );

/*
 * RTS 制御
 *	割込禁止で呼び出すこと
 */
LOCAL void rts( W on, DrvInfo *di )
{
	W	ch = di->ui->ch;

	if ( on ) {
		/* ハードウエアによる自動制御に戻す */
		UARTxCR(ch) |= UARTxCR_RTSEN;
	} else {
		/* ハードウエアによる自動制御を解除し、常にオフとする */
		UARTxCR(ch) &= ~(UARTxCR_RTSEN | UARTxCR_RTS);
	}
}

/*
 * CTS 制御
 */
LOCAL W cts( DrvInfo *di )
{
	W	ch = di->ui->ch;

	return ( (UARTxFR(ch) & UARTxFR_CTS) != 0 )? 1: 0;
}

/*
 * 受信フロー制御
 *	c >= 0	受信時 (受信文字)
 *	c < 0	受信バッファ読み出し時
 *	割込禁止で呼び出すこと
 */
LOCAL W rxflow( W c, DrvInfo *di )
{
	UartInfo *ui = di->ui;

	if ( c < 0 ) {
		if ( di->rcvbuf_len == RCVTHRESHOLD_L ) {
			/* 受信再開 */
			if ( ui->rsflow.rxflow ) {
				di->sndxoff = XON;
				send_data(di);
			}
			if ( ui->rsflow.rsflow ) {
				rts(1, di);
			}
		}

		return -1;
	}

	/* XON/XOFF 受信による送信フロー制御 */
	if ( ui->rsflow.sxflow ) {
		if ( c == XOFF ) {
			/* 送信停止 */
			ui->rsflow.rcvxoff = 1;
			return 1;
		}
		if ( c == XON || (ui->rsflow.rcvxoff && ui->rsflow.xonany) ) {
			/* 送信再開 */
			ui->rsflow.rcvxoff = 0;
			if ( c == XON ) return 1;
		}
	}

	if ( di->rcvbufsz - di->rcvbuf_len == RCVTHRESHOLD_H ) {
		/* 受信停止 */
		if ( ui->rsflow.rxflow ) {
			di->sndxoff = XOFF;
		}
		if ( ui->rsflow.rsflow ) {
			rts(0, di);
		}
	}

	return 0;
}

/*
 * 送信フロー制御
 *	割込禁止で呼び出すこと
 */
LOCAL W txflow( DrvInfo *di )
{
	UartInfo *ui = di->ui;

	if ( ui->rsflow.rcvxoff ) return 1;

	if ( ui->rsflow.csflow ) {
		if ( cts(di) == 0 ) return 1;
	}

	return 0;
}

/*
 * 受信バッファに１文字格納
 *	割込ハンドラから呼び出される。
 */
LOCAL void put_rcvbuf( UB c, DrvInfo *di )
{
	W	i;

	if ( di->rcvbuf_len < di->rcvbufsz ) {
		i = di->rcvbuf_pos + di->rcvbuf_len;
		if ( i >= di->rcvbufsz ) i -= di->rcvbufsz;

		di->rcvbuf[i] = c;
		di->rcvbuf_len++;
	} else {
		di->rcvsts |= 0x80; /* Recv Buffer Overflow */
	}
}

/*
 * 受信バッファから１文字取得
 *	タスクから呼び出される。
 */
LOCAL W get_rcvbuf( DrvInfo *di )
{
	W	c;
	UW	imask;

	DI(imask);

	if ( di->rcvbuf_len > 0 ) {
		c = di->rcvbuf[di->rcvbuf_pos++];
		if ( di->rcvbuf_pos >= di->rcvbufsz ) {
			di->rcvbuf_pos = 0;
		}
		di->rcvbuf_len--;

		rxflow(-1, di);
	} else {
		c = -1; /* 受信データなし */
	}

	EI(imask);

	return c;
}

/*
 * データ送信
 *	割込禁止で呼び出すこと
 */
LOCAL void send_data( DrvInfo *di )
{
	UartInfo *ui = di->ui;
	W	ch = ui->ch;

	if ( di->sndxoff != 0 && (UARTxFR(ch) & UARTxFR_TXFF) == 0 ) {
		/* XON/XOFF 送信 */
		UARTxDR(ch) = di->sndxoff;
		di->sndxoff = 0;
	}

	while ( di->snddat_len > 0 ) {
		if ( (UARTxFR(ch) & UARTxFR_TXFF) != 0 ) break;

		if ( txflow(di) != 0 ) break;

		/* 送信 */
		UARTxDR(ch) = *di->snddat++;
		di->snddat_len--;
	}
}

/*
 * 割込ハンドラ
 */
LOCAL void um_inthdr( DrvInfo *di )
{
	UartInfo *ui = di->ui;
	W	ch = ui->ch;
	UINT	flgptn = 0;
	W	n, c, s;

	/* 割込クリア */
	UARTxICR(ch) = UARTxRIS(ch);

	/* 受信処理 */
	n = 0;
	while ( (UARTxFR(ch) & UARTxFR_RXFE) == 0 ) {

		c = UARTxDR(ch);
		s = c >> 8;
		if ( s != 0 ) {
			/* 受信エラー */
			di->rcvsts |= s;	/* 受信エラーを記録 */
			UARTxECR(ch) = 0;	/* 受信エラーをクリア */

			c &= 0xff;
		}

		if ( rxflow(c, di) != 0 ) continue;

		/* 受信 */
		put_rcvbuf(c, di);
		n++;
	}
	if ( n > 0 ) {
		/* 受信データあり */
		flgptn |= RECV_DATA(ch);
	}

	/* 送信処理 */
	send_data(di);
	if ( di->snddat_len <= 0 ) {
		if ( di->snddat != NULL ) {
			/* 送信完了 */
			flgptn |= SEND_DATA(ch);
		}
	}

	if ( flgptn != 0 ) {
		tk_set_flg(uartflgid, flgptn);
	}
}

/*
 * 割込ハンドラ入り口
 */
LOCAL void um_inthdr_entry( UINT intno )
{
	DrvInfo		*di;
	UartInfo	*ui;

	for ( di = drvinfo; di < &drvinfo[N_DRVINFO]; ++di ) {
		ui = di->ui;
		if ( ui == NULL ) continue;
		if ( UARTx_IRQ_NUMBER(ui->ch) == intno ) um_inthdr(di);
	}
}

/*
 * 回線状態
 */
LOCAL W um_rsstat( W clear, UartInfo *ui )
{
	DrvInfo	*di = ui->exinf;
	union {
		W	w;
		RsStat	s;
	} stat;
	UW	m;
	UW	imask;

	DI(imask);
	m = di->rcvsts;
	if ( clear != 0 ) di->rcvsts = 0;
	EI(imask);

	stat.w = 0;
	if ( (m & 0x80)        != 0 ) stat.s.BE = 1; /* Recv Buffer Overflow */
	if ( (m & UARTxRSR_FE) != 0 ) stat.s.FE = 1; /* Framing Error */
	if ( (m & UARTxRSR_OE) != 0 ) stat.s.OE = 1; /* Overrun Error */
	if ( (m & UARTxRSR_PE) != 0 ) stat.s.PE = 1; /* Parity Error */
	if ( (m & UARTxRSR_BE) != 0 ) stat.s.BD = 1; /* Break Detect */

	stat.s.XF = ui->rsflow.rcvxoff;	/* Recv XOFF */
	stat.s.CS = cts(di);		/* Clear to Send (CTS) */

	m = UARTxFR(ui->ch);
	if ( (m & UARTxFR_DSR) != 0 ) stat.s.DR = 1; /* Dataset Ready (DSR) */
	if ( (m & UARTxFR_DCD) != 0 ) stat.s.CD = 1; /* Carrier Detect (DCD) */
	if ( (m & UARTxFR_RI)  != 0 ) stat.s.CI = 1; /* Calling Indicator(RI)*/

	return stat.w;
}

/*
 * 受信
 */
LOCAL ER um_serial_in( W port, B* buf, W len, W *alen, W tmout, UartInfo *ui )
{
	DrvInfo	*di = ui->exinf;
	UINT	flgptn;
	W	i, c;
	ER	err;

	if ( len <= 0 ) {
		/* 受信済みバイト数 */
		*alen = di->rcvbuf_len;
		return E_OK;
	}

	err = E_OK;

	for ( i = 0; i < len; ) {

		/* 受信データ取得 */
		c = get_rcvbuf(di);
		if ( c < 0 ) {
			if ( tmout == 0 ) break;

			/* 受信待ち */
			err = tk_wai_flg(uartflgid, RECV_DATA(ui->ch),
					 TWF_ORW|TWF_BITCLR, &flgptn, tmout);
			if ( err < E_OK && err != E_TMOUT ) break;

			c = get_rcvbuf(di);
			if ( c < 0 && err == E_TMOUT ) {
				err = E_IO | RSERR_TMOUT;
				break;
			}
		}
		if ( c >= 0 ) buf[i++] = c;

		if ( (ui->abortptn & RECV_CALL(ui->ch)) != 0 ) {
			err = E_IO | RSERR_ABORT;
			break;
		}

		/* 受信エラーの確認 */
		if ( di->rcvsts != 0 ) {
			err = E_IO;
			break;
		}
	}
	*alen = i;

	if ( (err & 0xffff0000) == E_IO ) {
		/* 回線状態を付加 */
		err |= um_rsstat(1, ui);
	}

	return err;
}

/*
 * 送信
 */
LOCAL ER um_serial_out( W port, B* buf, W len, W *alen, W tmout, UartInfo *ui )
{
	DrvInfo	*di = ui->exinf;
	UINT	flgptn;
	W	n;
	UW	imask;
	ER	err;

	/* 送信データ設定 */
	DI(imask);
	di->snddat = (UB*)buf;
	di->snddat_len = len;

	/* 送信開始 */
	send_data(di);
	EI(imask);

	err = E_OK;

	while ( (n = di->snddat_len) > 0 ) {

		/* 送信完了待ち */
		err = tk_wai_flg(uartflgid, SEND_DATA(ui->ch),
				 TWF_ORW|TWF_BITCLR, &flgptn, tmout);
		if ( err < E_OK ) {
			if ( err != E_TMOUT ) break;
			if ( di->snddat_len == n ) {
				err = E_IO | RSERR_TMOUT;
				break;
			}
		}
		if ( (ui->abortptn & SEND_CALL(ui->ch)) != 0 ) {
			err = E_IO | RSERR_ABORT;
			break;
		}
	}

	if ( (err & 0xffff0000) == E_IO ) {
		/* 回線状態を付加 */
		err |= um_rsstat(0, ui);
	}

	DI(imask);
	n = len - di->snddat_len;
	di->snddat_len = 0;
	di->snddat = NULL;
	EI(imask);

	*alen = n;
	return err;
}

/* ------------------------------------------------------------------------ */

/*
 * サスペンド／リジューム
 */
LOCAL ER um_suspend( BOOL sus, UartInfo *ui )
{
	W	ch = ui->ch;

	if ( sus ) {
		/* サスペンド */
		UARTxCR(ch) &= ~(UARTxCR_TXE | UARTxCR_RXE);
	} else {
		/* リジューム */
		UARTxCR(ch) |= UARTxCR_TXE | UARTxCR_RXE;
	}

	return E_OK;
}

/*
 * UART 設定
 */
LOCAL ER um_setup( RsMode rsmode, RsFlow rsflow, DrvInfo *di )
{
	UartInfo *ui = di->ui;
	W	ch = ui->ch;
	UW	n;
	ER	err;

	if ( (UARTxCR(ch) & UARTxCR_TXE) != 0 ) {
		/* 送信動作中であれば待つ */
		for ( n = 0; n < 20; ++n ) {
			if ( (UARTxFR(ch) & UARTxFR_TXFE) != 0 ) break;
			tk_dly_tsk(10);
		}
	}

	/* 通信停止 */
	uart_abort(ui);
	UARTxCR(ch) = 0;
	UARTxDMACR(ch) = 0;
	UARTxIMSC(ch) = 0;
	UARTxICR(ch) = UARTxRIS(ch);

	/* 設定 */
	n = CLOCK_fsys * (64*2 / 16) / rsmode.baud;
	n = (n + 1) >> 1;	/* 丸め(四捨五入) */
	UARTxIBDR(ch) = n >> 6;
	UARTxFBDR(ch) = n & 0x3f;

	UARTxIFLS(ch) = UARTxIFLS_RXIFLSEL(UARTxIFLS_1_2) |
			UARTxIFLS_TXIFLSEL(UARTxIFLS_1_4);

	if ( rsflow.csflow && rsflow.rsflow ) {
		n = UARTxLCR_H_FEN;
	} else if ( rsflow.rxflow || rsflow.sxflow ) {
		/* XON/XOFF フロー制御のみの時は FIFO を使わない
		   FIFO を使うとフロー制御が間に合わない */
		n = 0;
	} else {
		n = UARTxLCR_H_FEN;
	}

	n |= UARTxLCR_H_WLEN(rsmode.datalen + 5);

	switch ( rsmode.stopbits ) {
	  case 0:	/* 1   bit */
		break;
	  case 1:	/* 1.5 bit */
		err = E_NOSPT;
		goto err_ret;
	  case 2:	/* 2   bit */
		n |= UARTxLCR_H_STP2;
		break;
	  default:
		err = E_PAR;
		goto err_ret;
	}

	switch ( rsmode.parity ) {
	  case 0:	/* なし */
		break;
	  case 1:	/* 奇数 */
		n |= UARTxLCR_H_PEN;
		break;
	  case 2:	/* 偶数 */
		n |= UARTxLCR_H_PEN | UARTxLCR_H_EPS;
		break;
	  default:
		err = E_PAR;
		goto err_ret;
	}
	UARTxLCR_H(ch) = n;

	n = UARTxCR_RXE | UARTxCR_TXE | UARTxCR_UARTEN;
	n |= ( rsflow.csflow )? UARTxCR_CTSEN: 0;
	n |= ( rsflow.rsflow )? UARTxCR_RTSEN: UARTxCR_RTS;
	n |= UARTxCR_DTR;

	/* 設定保存 */
	ui->rsmode = rsmode;
	ui->rsflow = rsflow;

	/* 送受信バッファクリア */
	di->snddat = NULL;
	di->snddat_len = 0;
	di->rcvbuf_pos = 0;
	di->rcvbuf_len = 0;
	di->rcvsts = 0;
	di->sndxoff = 0;

	/* 通信開始 */
	UARTxCR(ch) = n;
	UARTxIMSC(ch) =
		UARTxINT_RT |
		UARTxINT_TX |
		UARTxINT_RX |
		UARTxINT_CTS;

	return E_OK;

  err_ret:
	return err;
}

/*
 * DN_RSMODE: 通信モードの設定・取得
 */
LOCAL ER um_rsmode( W kind, RsMode *arg, DrvInfo *di )
{
	ER	err;

	if ( kind == -DN_RSMODE ) {
		/* 取得 */
		*arg = di->ui->rsmode;
		return E_OK;
	}

	/* 設定 */
	err = um_setup(*arg, (RsFlow){0}, di);
	if ( err < E_OK ) goto err_ret;

	return E_OK;

  err_ret:
	return err;
}

/*
 * DN_RSFLOW: フロー制御の設定・取得
 */
LOCAL ER um_rsflow( W kind, RsFlow *arg, DrvInfo *di )
{
	UartInfo *ui = di->ui;
	UW	imask, rcvxoff;
	ER	err;

	if ( kind == -DN_RSFLOW ) {
		/* 取得 */
		*arg = ui->rsflow;
		return E_OK;
	}

	if ( arg->csflow == ui->rsflow.csflow &&
	     arg->rsflow == ui->rsflow.rsflow &&
	     arg->sxflow == ui->rsflow.sxflow &&
	     arg->rxflow == ui->rsflow.rxflow ) {

		/* rcvxoff または xonany のみの変更 */
		DI(imask);
		rcvxoff = ui->rsflow.rcvxoff;
		ui->rsflow = *arg;

		if ( rcvxoff != 0 &&
		     di->snddat_len > 0 &&
		     arg->rcvxoff == 0 ) {
			/* 送信再開 */
			send_data(di);
		}
		EI(imask);
	} else {
		/* 設定 */
		err = um_setup(ui->rsmode, *arg, di);
		if ( err < E_OK ) goto err_ret;
	}

	return E_OK;

  err_ret:
	return err;
}

/*
 * DN_RSBREAK: ブレーク送出
 */
LOCAL ER um_rsbreak( UW arg, DrvInfo *di )
{
	W	ch = di->ui->ch;
	UW	imask;

	DI(imask);
	UARTxLCR_H(ch) |= UARTxLCR_H_BRK;
	EI(imask);

	tk_dly_tsk(arg);

	DI(imask);
	UARTxLCR_H(ch) &= ~UARTxLCR_H_BRK;
	EI(imask);

	return E_OK;
}

/*
 * RS_RCVBUFSZ:	受信バッファサイズの設定・取得
 */
LOCAL ER um_rcvbufsz( W kind, UW *arg, DrvInfo *di )
{
	if ( kind == -RS_RCVBUFSZ ) {
		/* 取得 */
		*arg = di->rcvbufsz;
		return E_OK;
	}

	if ( *arg < RCVBUFSZ_MIN ) return E_PAR;
	if ( *arg == di->rcvbufsz ) return E_OK; /* 変更なし */

	/* 設定：固定長のため未対応 */
	return E_NOSPT;
}

/*
 * RS_LINECTL: 制御線の ON/OFF
 */
LOCAL ER um_linectl( W kind, UW arg, DrvInfo *di )
{
	W	ch = di->ui->ch;
	UW	m, v;
	UW	imask;

	m = 0;
	if ( (arg & RSCTL_RTS) != 0 ) m = UARTxCR_RTSEN;

	v = 0;
	if ( (arg & RSCTL_RTS) != 0 ) v |= UARTxCR_RTS;
	if ( (arg & RSCTL_DTR) != 0 ) v |= UARTxCR_DTR;

	switch ( arg & ~3 ) {
	  case RSCTL_SET:
		m |= UARTxCR_RTS | UARTxCR_DTR;
		break;

	  case RSCTL_ON:
		m |= v;
		break;

	  case RSCTL_OFF:
		m |= v;
		v = 0;
		break;

	  default:
		return E_PAR;
	}

	DI(imask);
	UARTxCR(ch) = (UARTxCR(ch) & ~m) | v;
	EI(imask);

	return E_OK;
}

/*
 * 制御
 */
LOCAL ER um_serial_ctl( W port, W kind, UW *arg, UartInfo *ui )
{
	DrvInfo	*di = ui->exinf;
	ER	err;

	switch ( kind ) {
	  case RS_ABORT:	/* アボート */
		err = uart_abort(ui);
		break;

	  case RS_SUSPEND:	/* サスペンド */
	  case RS_RESUME:	/* リジューム */
		err = uart_suspend(kind, ui);
		break;

	  case DN_RSMODE:	/* 通信モードの設定 */
	  case -DN_RSMODE:	/* 通信モードの取得 */
		err = um_rsmode(kind, (RsMode*)arg, di);
		break;

	  case DN_RSFLOW:	/* フロー制御の設定 */
	  case -DN_RSFLOW:	/* フロー制御の取得 */
		err = um_rsflow(kind, (RsFlow*)arg, di);
		break;

	  case -DN_RSSTAT:	/* 回線状態の取得 */
		*arg = um_rsstat(1, ui);
		err = E_OK;
		break;

	  case DN_RSBREAK:	/* ブレーク送出 */
		err = um_rsbreak(*arg, di);
		break;

	  case RS_RCVBUFSZ:	/* 受信バッファサイズの設定 */
	  case -RS_RCVBUFSZ:	/* 受信バッファサイズの取得 */
		err = um_rcvbufsz(kind, arg, di);
		break;

	  case RS_LINECTL:	/* 制御線の ON/OFF */
		err = um_linectl(kind, *arg, di);
		break;

	  case DN_RS16450:	/* ハードウエア構成の設定 */
	  case -DN_RS16450:	/* ハードウエア構成の取得 */
		err = E_NOSPT;
		break;

	  default:
		err = E_PAR;
	}

	return err;
}

/* ------------------------------------------------------------------------ */

/*
 * 割込優先度
 */
#define	UMINTLEVEL	2

/*
 * 初期設定
 */
EXPORT ER um_serial_init( W port, UartInfo *ui )
{
	DrvInfo	*di;
	T_DINT	dint;
	ER	err;

	for ( di = drvinfo; di < &drvinfo[N_DRVINFO]; ++di ) {
		if ( di->ui == NULL ) break;
	}
	if ( di >= &drvinfo[N_DRVINFO] ) return E_LIMIT;
	di->ui = ui;
	ui->exinf = di;

	/* 受信バッファ割当 */
	di->rcvbuf = di->_rcvbuf;
	di->rcvbufsz = RCVBUFSZ_MIN;
	di->rcvbuf_pos = 0;
	di->rcvbuf_len = 0;

	/* 割込ハンドラ設定 */
	dint.intatr = TA_HLNG;
	dint.inthdr = um_inthdr_entry;
	err = tk_def_int(UARTx_IRQ_NUMBER(ui->ch), &dint);
	if ( err < E_OK ) goto err_ret;

	/* UART 初期化 */
	err = um_setup(ui->rsmode, ui->rsflow, di);
	if ( err < E_OK ) goto err_ret;

	/* 割込許可 */
	EnableInt(UARTx_IRQ_NUMBER(ui->ch), UMINTLEVEL);

	return E_OK;

  err_ret:
	di->ui = NULL;
	return err;
}

/*
 * ドライバー定義
 */
EXPORT UartDrv	Uart_m = {
	um_serial_in,
	um_serial_out,
	um_serial_ctl,
	um_serial_init,
	um_rsstat,
	um_suspend
};

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
 *	uart_s.c
 *
 *	TMPM367 SIO/UART
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

#define	RCVTHRESHOLD_H	(RCVBUFSZ_MIN - 16)	/* 停止:受信フロー制御閾値 (バイト数) */
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
	/* ハードウエア未対応 */
}

/*
 * CTS 制御
 */
LOCAL W cts( DrvInfo *di )
{
	/* ハードウエア未対応 */
	return 1;
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
		di->rcvbuf[di->rcvbuf_pos++] = c;
		if ( di->rcvbuf_pos >= di->rcvbufsz ) di->rcvbuf_pos = 0;

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

	if ( di->rcvsts != 0 ) {
		c = -2; /* Recv Error */
	} else if ( di->rcvbuf_len > 0 ) {
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

	if ( di->sndxoff != 0 && (UART_SCxTST(ch) & SCxTST_TLVL_MASK) < 2 ) {
		/* XON/XOFF 送信 */
		UART_SCxBUF(ch) = di->sndxoff;
		di->sndxoff = 0;
	}

	while ( di->snddat_len > 0 ) {
		if ( (UART_SCxTST(ch) & SCxTST_TLVL_MASK) >= 2 ) break;

		if ( txflow(di) != 0 ) break;

		/* 送信 */
		UART_SCxBUF(ch) = *di->snddat++;
		di->snddat_len--;
	}
}

/*
 * 割込ハンドラ
 */
LOCAL void us_inthdr( DrvInfo *di )
{
	UartInfo *ui = di->ui;
	W	ch = ui->ch;
	UINT	flgptn = 0;
	W	n, c;

	/* 受信エラー読み出し(読み出すとクリアされる) */
	c = UART_SCxCR(ch) & SCxCR_ERR;
	if ( c != 0 ) {
		/* 受信エラー */
		di->rcvsts |= c;	/* 受信エラーを記録 */
		flgptn |= RECV_DATA(ch);
	}

	/* 受信処理 */
	n = 0;
	while ( (UART_SCxRST(ch) & SCxRST_RLVL_MASK) > 0 ) {

		c = UART_SCxBUF(ch);

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
 * 受信割込ハンドラ入り口
 */
LOCAL void us_rxinthdr_entry( UINT intno )
{
	DrvInfo		*di;
	UartInfo	*ui;

	for ( di = drvinfo; di < &drvinfo[N_DRVINFO]; ++di ) {
		ui = di->ui;
		if ( ui == NULL ) continue;
		if ( SCx_RX_IRQ_NUMBER(ui->ch) == intno ) us_inthdr(di);
	}
}

/*
 * 送信割込ハンドラ入り口
 */
LOCAL void us_txinthdr_entry( UINT intno )
{
	DrvInfo		*di;
	UartInfo	*ui;

	for ( di = drvinfo; di < &drvinfo[N_DRVINFO]; ++di ) {
		ui = di->ui;
		if ( ui == NULL ) continue;
		if ( SCx_TX_IRQ_NUMBER(ui->ch) == intno ) us_inthdr(di);
	}
}

/*
 * 回線状態
 */
LOCAL W us_rsstat( W clear, UartInfo *ui )
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
	if ( (m & 0x80)       != 0 ) stat.s.BE = 1; /* Recv Buffer Overflow */
	if ( (m & SCxCR_FERR) != 0 ) stat.s.FE = 1; /* Framing Error */
	if ( (m & SCxCR_OERR) != 0 ) stat.s.OE = 1; /* Overrun Error */
	if ( (m & SCxCR_PERR) != 0 ) stat.s.PE = 1; /* Parity Error */

	stat.s.XF = ui->rsflow.rcvxoff;	/* Recv XOFF */
	stat.s.CS = cts(di);		/* Clear to Send (CTS) */

#if 0	/* ハードウエア未対応のため常に 0 */
	stat.s.BD; /* Break Detect */
	stat.s.DR; /* Dataset Ready (DSR) */
	stat.s.CD; /* Carrier Detect (DCD) */
	stat.s.CI; /* Calling Indicator(RI)*/
#endif

	return stat.w;
}

/*
 * 受信
 */
LOCAL ER us_serial_in( W port, B* buf, W len, W *alen, W tmout, UartInfo *ui )
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
		if ( c == -2 ) {
			err = E_IO;
			break;
		}
		if ( c < 0 ) {
			if ( tmout == 0 ) break;

			/* 受信待ち */
			err = tk_wai_flg(uartflgid, RECV_DATA(ui->ch),
					 TWF_ORW|TWF_BITCLR, &flgptn, tmout);
			if ( err < E_OK && err != E_TMOUT ) break;

			c = get_rcvbuf(di);
			if ( c == -2 ) {
				err = E_IO;
				break;
			}
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
	}
	*alen = i;

	if ( (err & 0xffff0000) == E_IO ) {
		/* 回線状態を付加 */
		err |= us_rsstat(1, ui);
	}

	return err;
}

/*
 * 送信
 */
LOCAL ER us_serial_out( W port, B* buf, W len, W *alen, W tmout, UartInfo *ui )
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
		err |= us_rsstat(0, ui);
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
LOCAL ER us_suspend( BOOL sus, UartInfo *ui )
{
	W	ch = ui->ch;

	if ( sus ) {
		/* サスペンド */
		UART_SCxMOD0(ch) &= ~SCxMOD0_RXE;
		UART_SCxMOD1(ch) &= ~SCxMOD1_TXE;
	} else {
		/* リジューム */
		UART_SCxMOD0(ch) |= SCxMOD0_RXE;
		UART_SCxMOD1(ch) |= SCxMOD1_TXE;
	}

	return E_OK;
}

/*
 * UART 設定
 */
LOCAL ER us_setup( RsMode rsmode, RsFlow rsflow, DrvInfo *di )
{
	UartInfo *ui = di->ui;
	W	ch = ui->ch;
	UW	n, m;
	ER	err;

	if ( (UART_SCxMOD1(ch) & SCxMOD1_TXE) != 0 ) {
		/* 送信動作中であれば待つ */
		for ( n = 0; n < 20; ++n ) {
			if ( (UART_SCxMOD2(ch) & (SCxMOD2_TXRUN|SCxMOD2_TBEMP))
						== SCxMOD2_TBEMP ) break;
			tk_dly_tsk(10);
		}
	}

	/* 通信停止 */
	uart_abort(ui);
	UART_SCxMOD0(ch) &= ~SCxMOD0_RXE;
	UART_SCxMOD1(ch) &= ~SCxMOD1_TXE;

	/* 設定 */
	n = (CLOCK_T0 / 2) * (16*2 / 16) / rsmode.baud;
	if ( n < 0x200 ) {
		m = SCxBRCR_BRCK_T01;
	} else if ( n < 0x200 * 4 ) {
		m = SCxBRCR_BRCK_T04;
		n /= 4;
	} else if ( n < 0x200 * 16 ) {
		m = SCxBRCR_BRCK_T16;
		n /= 16;
	} else if ( n < 0x200 * 64 ) {
		m = SCxBRCR_BRCK_T64;
		n /= 64;
	} else {
		err = E_NOSPT;
		goto err_ret;
	}
	n = (n + 1) >> 1;	/* 丸め(四捨五入) */
	if ( (n & 0xf) == 0 ) {
		UART_SCxBRADD(ch) = 0;
		UART_SCxBRCR(ch) = m | ((n >> 4) & SCxBRCR_BRS_MASK);
	} else {
		UART_SCxBRADD(ch) = 16 - (n & 0xf);
		UART_SCxBRCR(ch) = m | ((n >> 4) & SCxBRCR_BRS_MASK) |
					SCxBRCR_BRADDE;
	}

	UART_SCxMOD0(ch) = SCxMOD0_SC_BAUD;
	UART_SCxMOD1(ch) = SCxMOD1_FDXP_MASK;
	UART_SCxMOD2(ch) = SCxMOD2_WBUF;

	switch ( rsmode.datalen ) {
	  case 0:	/* 5 bit */
	  case 1:	/* 6 bit */
		err = E_NOSPT;
		goto err_ret;
	  case 2:	/* 7 bit */
		UART_SCxMOD0(ch) |= SCxMOD0_SM7;
		break;
	  case 3:	/* 8 bit */
		UART_SCxMOD0(ch) |= SCxMOD0_SM8;
		break;
	}

	switch ( rsmode.stopbits ) {
	  case 0:	/* 1   bit */
		break;
	  case 1:	/* 1.5 bit */
		err = E_NOSPT;
		goto err_ret;
	  case 2:	/* 2   bit */
		UART_SCxMOD2(ch) |= SCxMOD2_SBLEN;
		break;
	  default:
		err = E_PAR;
		goto err_ret;
	}

	switch ( rsmode.parity ) {
	  case 0:	/* なし */
		UART_SCxCR(ch) = 0;
		break;
	  case 1:	/* 奇数 */
		UART_SCxCR(ch) = SCxCR_PE;
		break;
	  case 2:	/* 偶数 */
		UART_SCxCR(ch) = SCxCR_PE | SCxCR_EVEN;
		break;
	  default:
		err = E_PAR;
		goto err_ret;
	}

	UART_SCxFCNF(ch) = SCxFCNF_CNFG;
	UART_SCxRFC(ch) = SCxRFC_RFCS | /*SCxRFC_RIL*/ 1;
	UART_SCxTFC(ch) = SCxTFC_TFCS | /*SCxTFC_TIL*/ 0;

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
	UART_SCxFCNF(ch) |= SCxFCNF_TFIE | SCxFCNF_RFIE;
	UART_SCxMOD0(ch) |= SCxMOD0_RXE;
	UART_SCxMOD1(ch) |= SCxMOD1_TXE;

	return E_OK;

  err_ret:
	return err;
}

/*
 * DN_RSMODE: 通信モードの設定・取得
 */
LOCAL ER us_rsmode( W kind, RsMode *arg, DrvInfo *di )
{
	ER	err;

	if ( kind == -DN_RSMODE ) {
		/* 取得 */
		*arg = di->ui->rsmode;
		return E_OK;
	}

	/* 設定 */
	err = us_setup(*arg, (RsFlow){0}, di);
	if ( err < E_OK ) goto err_ret;

	return E_OK;

  err_ret:
	return err;
}

/*
 * DN_RSFLOW: フロー制御の設定・取得
 */
LOCAL ER us_rsflow( W kind, RsFlow *arg, DrvInfo *di )
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
		err = us_setup(ui->rsmode, *arg, di);
		if ( err < E_OK ) goto err_ret;
	}

	return E_OK;

  err_ret:
	return err;
}

/*
 * DN_RSBREAK: ブレーク送出
 */
LOCAL ER us_rsbreak( UW arg, DrvInfo *di )
{
	/* ハードウエア未対応 */
	return E_NOSPT;
}

/*
 * RS_RCVBUFSZ:	受信バッファサイズの設定・取得
 */
LOCAL ER us_rcvbufsz( W kind, UW *arg, DrvInfo *di )
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
LOCAL ER us_linectl( W kind, UW arg, DrvInfo *di )
{
	/* ハードウエア未対応 */
	return E_NOSPT;
}

/*
 * 制御
 */
LOCAL ER us_serial_ctl( W port, W kind, UW *arg, UartInfo *ui )
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
		err = us_rsmode(kind, (RsMode*)arg, di);
		break;

	  case DN_RSFLOW:	/* フロー制御の設定 */
	  case -DN_RSFLOW:	/* フロー制御の取得 */
		err = us_rsflow(kind, (RsFlow*)arg, di);
		break;

	  case -DN_RSSTAT:	/* 回線状態の取得 */
		*arg = us_rsstat(1, ui);
		err = E_OK;
		break;

	  case DN_RSBREAK:	/* ブレーク送出 */
		err = us_rsbreak(*arg, di);
		break;

	  case RS_RCVBUFSZ:	/* 受信バッファサイズの設定 */
	  case -RS_RCVBUFSZ:	/* 受信バッファサイズの取得 */
		err = us_rcvbufsz(kind, arg, di);
		break;

	  case RS_LINECTL:	/* 制御線の ON/OFF */
		err = us_linectl(kind, *arg, di);
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
#define	RXINTLEVEL	2
#define	TXINTLEVEL	2

/*
 * 初期設定
 */
EXPORT ER us_serial_init( W port, UartInfo *ui )
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
	dint.inthdr = us_rxinthdr_entry;
	err = tk_def_int(SCx_RX_IRQ_NUMBER(ui->ch), &dint);
	if ( err < E_OK ) goto err_ret;

	dint.intatr = TA_HLNG;
	dint.inthdr = us_txinthdr_entry;
	err = tk_def_int(SCx_TX_IRQ_NUMBER(ui->ch), &dint);
	if ( err < E_OK ) goto err_ret;

	/* SIO/UART イネーブル */
	UART_SCxEN(ui->ch) = SCxEN_SIOENA;

	/* UART 初期化 */
	err = us_setup(ui->rsmode, ui->rsflow, di);
	if ( err < E_OK ) goto err_ret;

	/* 割込許可 */
	EnableInt(SCx_RX_IRQ_NUMBER(ui->ch), RXINTLEVEL);
	EnableInt(SCx_TX_IRQ_NUMBER(ui->ch), TXINTLEVEL);

	return E_OK;

  err_ret:
	di->ui = NULL;
	return err;
}

/*
 * ドライバー定義
 */
EXPORT UartDrv	Uart_s = {
	us_serial_in,
	us_serial_out,
	us_serial_ctl,
	us_serial_init,
	us_rsstat,
	us_suspend
};

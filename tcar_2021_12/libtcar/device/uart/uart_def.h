
#define	TX03_PERIPH_BASE	0x40000000	/* ペリフェラルレジスタベースアドレス */

/*
 * UART registers
 */
/* UART毎のベースアドレス */
#define	UART0_BASE		(TX03_PERIPH_BASE + 0x000E1000UL)	/* SIO/UART */
#define	UART1_BASE		(TX03_PERIPH_BASE + 0x000E1100UL)
#define	UART2_BASE		(TX03_PERIPH_BASE + 0x000E1200UL)
#define	UART3_BASE		(TX03_PERIPH_BASE + 0x000E1300UL)
#define	UART4_BASE		(TX03_PERIPH_BASE + 0x00048000UL)	/* UART modem */
#define	UART5_BASE		(TX03_PERIPH_BASE + 0x00049000UL)

/* SIO/UART */
#define	UART_SCxEN(p)		(*(_UB*)(uart_reg_base[p] + 0x0000))	/* イネーブルレジスタ */
#define	UART_SCxBUF(p)		(*(_UB*)(uart_reg_base[p] + 0x0004))	/* 送受信バッファレジスタ */
#define	UART_SCxCR(p)		(*(_UB*)(uart_reg_base[p] + 0x0008))	/* コントロールレジスタ */
#define	UART_SCxMOD0(p)		(*(_UB*)(uart_reg_base[p] + 0x000C))	/* モードコントロールレジスタ０ */
#define	UART_SCxBRCR(p)		(*(_UB*)(uart_reg_base[p] + 0x0010))	/* ボーレートジェネレータコントロールレジスタ */
#define	UART_SCxBRADD(p)	(*(_UB*)(uart_reg_base[p] + 0x0014))	/* ボーレートジェネレータコントロールレジスタ２ */
#define	UART_SCxMOD1(p)		(*(_UB*)(uart_reg_base[p] + 0x0018))	/* モードコントロールレジスタ１ */
#define	UART_SCxMOD2(p)		(*(_UB*)(uart_reg_base[p] + 0x001C))	/* モードコントロールレジスタ２ */
#define	UART_SCxRFC(p)		(*(_UB*)(uart_reg_base[p] + 0x0020))	/* 受信FIFOコンフィグレジスタ */
#define	UART_SCxTFC(p)		(*(_UB*)(uart_reg_base[p] + 0x0024))	/* 送信FIFOコンフィグレジスタ */
#define	UART_SCxRST(p)		(*(_UB*)(uart_reg_base[p] + 0x0028))	/* 受信FIFOステータスレジスタ */
#define	UART_SCxTST(p)		(*(_UB*)(uart_reg_base[p] + 0x002C))	/* 送信FIFOステータスレジスタ */
#define	UART_SCxFCNF(p)		(*(_UB*)(uart_reg_base[p] + 0x0030))	/* FIFOコンフィグレジスタ */

/* イネーブルレジスタ */
#define	SCxEN_SIOENA		0x00000001UL	/* SIO動作（動作） */
#define	SCxEN_SIODISA		0x00000000UL	/* SIO動作（停止） */
/* 送受信バッファレジスタ */
#define	SCxBUF_TBRB_MASK	0x000000FFUL	/* 送受信バッファマスク */
/* コントロールレジスタ */
#define	SCxCR_RB8		0x00000080UL	/* 受信データビット8 */
#define	SCxCR_EVEN		0x00000040UL	/* パリティ種別（EVEN） */
#define	SCxCR_PE		0x00000020UL	/* パリティ付加（イネーブル） */
#define	SCxCR_OERR		0x00000010UL	/* バッファオーバーランエラー */
#define	SCxCR_PERR		0x00000008UL	/* パリティエラー */
#define	SCxCR_FERR		0x00000004UL	/* フレーミングエラー */
#define	SCxCR_SCLKS		0x00000002UL	/* 入力クロックエッジ選択（非UART用） */
#define	SCxCR_IOC		0x00000001UL	/* クロック選択（非UART用） */
#define	SCxCR_ERR		(SCxCR_OERR|SCxCR_PERR|SCxCR_FERR)	/* UARTエラーマスク */
/* モードコントロールレジスタ０ */
#define	SCxMOD0_TB8		0x00000080UL	/* 送信データビット8 */
#define	SCxMOD0_CTSE		0x00000040UL	/* CTS機能制御 */
#define	SCxMOD0_RXE		0x00000020UL	/* 受信イネーブル */
#define	SCxMOD0_WU		0x00000010UL	/* ウェイクアップイネーブル */
#define	SCxMOD0_SM_MASK		0x0000000CUL	/* 転送ビット長マスク */
#define	SCxMOD0_SMIO		0x00000000UL	/* 転送ビット長IOモード（非UART） */
#define	SCxMOD0_SM7		0x00000004UL	/* 転送ビット長7ビット */
#define	SCxMOD0_SM8		0x00000008UL	/* 転送ビット長8ビット */
#define	SCxMOD0_SM9		0x0000000CUL	/* 転送ビット長9ビット */
#define	SCxMOD0_SC_MASK		0x00000003UL	/* シリアル転送クロックマスク */
#define	SCxMOD0_SC_TB		0x00000000UL	/* シリアル転送クロックTBxOUT */
#define	SCxMOD0_SC_BAUD		0x00000001UL	/* シリアル転送クロックボーレートジェネレータ */
#define	SCxMOD0_SC_FSYS		0x00000002UL	/* シリアル転送クロック内部クロック */
#define	SCxMOD0_SC_SCLK		0x00000003UL	/* シリアル転送クロック外部クロック */
/* ボーレートジェネレータコントロールレジスタ */
#define	SCxBRCR_BRADDE		0x00000040UL	/* N+16分周機能イネーブル */
#define	SCxBRCR_BRCK_MASK	0x00000030UL	/* 入力クロック選択マスク */
#define	SCxBRCR_BRCK_T01	0x00000000UL	/* 入力クロックφT1 */
#define	SCxBRCR_BRCK_T04	0x00000010UL	/* 入力クロックφT4 */
#define	SCxBRCR_BRCK_T16	0x00000020UL	/* 入力クロックφT16 */
#define	SCxBRCR_BRCK_T64	0x00000030UL	/* 入力クロックφT64 */
#define	SCxBRCR_BRS_MASK	0x0000000FUL	/* 分周値設定マスク */
/* ボーレートジェネレータコントロールレジスタ２ */
#define	SCxBRADD_BRK_MASK	0x0000000FUL	/* 分周値計算のKの値マスク */
/* モードコントロールレジスタ１ */
#define	SCxMOD1_I2SC		0x00000080UL	/* IDLEモード時動作（動作） */
#define	SCxMOD1_FDXP_MASK	0x00000060UL	/* 転送モード設定マスク（非UART用） */
#define	SCxMOD1_TXE		0x00000010UL	/* 送信イネーブル */
#define	SCxMOD1_SINT_MASK	0x0000000EUL	/* 連続転送インターバルマスク（非UART用） */
/* モードコントロールレジスタ２ */
#define	SCxMOD2_TBEMP		0x00000080UL	/* 送信バッファエンプティフラグ */
#define	SCxMOD2_RBFLL		0x00000040UL	/* 受信バッファフルフラグ */
#define	SCxMOD2_TXRUN		0x00000020UL	/* 送信動作中フラグ */
#define	SCxMOD2_SBLEN		0x00000010UL	/* ストップビット長（0：1ビット、1：2ビット） */
#define	SCxMOD2_DRCHG		0x00000008UL	/* 転送方向（0：LSBFirst=UART） */
#define	SCxMOD2_WBUF		0x00000004UL	/* ダブルバッファイネーブル */
#define	SCxMOD2_SWRST1		0x00000002UL	/* ソフトウェアリセット第１動作 */
#define	SCxMOD2_SWRST2		0x00000001UL	/* ソフトウェアリセット第２動作 */
/* 受信FIFOコンフィグレジスタ */
#define	SCxRFC_RFCS		0x00000080UL	/* 受信FIFOクリア */
#define	SCxRFC_RFIS		0x00000040UL	/* 割込み発生条件選択 */
#define	SCxRFC_RIL_MASK		0x00000003UL	/* 受信割込みが発生するFIFOのfillレベルマスク */
/* 送信FIFOコンフィグレジスタ */
#define	SCxTFC_TFCS		0x00000080UL	/* 送信FIFOクリア */
#define	SCxTFC_TFIS		0x00000040UL	/* 割込み発生条件選択 */
#define	SCxTFC_TIL_MASK		0x000000U03L	/* 送信割込みが発生するFIFOのfillレベルマスク */
/* 受信FIFOステータスレジスタ */
#define	SCxRST_ROR		0x00000080UL	/* 受信FIFOオーバーラン */
#define	SCxRST_RLVL_MASK	0x00000007UL	/* 受信FIFOのfillレベルマスク */
/* 送信FIFOステータスレジスタ */
#define	SCxTST_TUR		0x00000080UL	/* 送信FIFOアンダーラン */
#define	SCxTST_TLVL_MASK	0x00000007UL	/* 送信FIFOのfillレベルマスク */
/* FIFOコンフィグレジスタ */
#define	SCxFCNF_RFST		0x00000010UL	/* 受信FIFO使用バイト数 */
#define	SCxFCNF_TFIE		0x00000008UL	/* 送信FIFO使用時の送信割込み許可 */
#define	SCxFCNF_RFIE		0x00000004UL	/* 受信FIFO使用時の受信割込み許可 */
#define	SCxFCNF_RXTXCNT		0x00000002UL	/* RXE/TXEの自動禁止 */
#define	SCxFCNF_CNFG		0x00000001UL	/* FIFOの許可 */
#define	SCxFCNF_DISABLE		0x00000000UL	/* FIFOを使用しない */

/* UART modem */
#define	UARTxDR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0000))	/* Data register */
#define	UARTxRSR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0004))	/* Receive status register */
#define	UARTxECR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0004))	/* Error clear register */
#define	UARTxFR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0018))	/* Flag register */
#define	UARTxILPR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0020))	/* IrDA low-power counter */
#define	UARTxIBDR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0024))	/* Integer baud rate register */
#define	UARTxFBDR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0028))	/* Fractional baud rate register */
#define	UARTxLCR_H(p) 		(*(_UW*)(uart_reg_base[p] + 0x002C))	/* Line control register */
#define	UARTxCR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0030))	/* Control register */
#define	UARTxIFLS(p) 		(*(_UW*)(uart_reg_base[p] + 0x0034))	/* Interrupt FIFO level select register */
#define	UARTxIMSC(p) 		(*(_UW*)(uart_reg_base[p] + 0x0038))	/* Interrupt mask set/clear register */
#define	UARTxRIS(p) 		(*(_UW*)(uart_reg_base[p] + 0x003C))	/* Raw interrupt status register */
#define	UARTxMIS(p) 		(*(_UW*)(uart_reg_base[p] + 0x0040))	/* Masked interrupt status register */
#define	UARTxICR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0044))	/* Interrupt clear register */
#define	UARTxDMACR(p) 		(*(_UW*)(uart_reg_base[p] + 0x0048))	/* DMA control register */

#define	UARTxDR_OE		(1 << 11)	/* R オーバーランエラー */
#define	UARTxDR_BE		(1 << 10)	/* R ブレークエラー */
#define	UARTxDR_PE		(1 << 9)	/* R パリティエラー */
#define	UARTxDR_FE		(1 << 8)	/* R フレーミングエラー */
#define	UARTxDR_DATA		(0xff)		/* RW 送受信データ */

#define	UARTxRSR_OE		(1 << 3)	/* R オーバーランエラー */
#define	UARTxRSR_BE		(1 << 2)	/* R ブレークエラー */
#define	UARTxRSR_PE		(1 << 1)	/* R パリティーエラー */
#define	UARTxRSR_FE		(1 << 0)	/* R フレーミングエラー */

#define	UARTxFR_RI		(1 << 8)	/* R Link indicator */
#define	UARTxFR_TXFE		(1 << 7)	/* R 送信エンプティ */
#define	UARTxFR_RXFF		(1 << 6)	/* R 受信フル */
#define	UARTxFR_TXFF		(1 << 5)	/* R 送信フル */
#define	UARTxFR_RXFE		(1 << 4)	/* R 受信エンプティ */
#define	UARTxFR_BUSY		(1 << 3)	/* R UART busy */
#define	UARTxFR_DCD		(1 << 2)	/* R Data carrier detect */
#define	UARTxFR_DSR		(1 << 1)	/* R Data set ready */
#define	UARTxFR_CTS		(1 << 0)	/* R Clear to send */

#define	UARTxLCR_H_SPS		(1 << 7)	/* RW スティックパリティ選択 */
#define	UARTxLCR_H_WLEN(n)	(((n)-5) << 5)	/* RW ワード長 (5～8bit) */
#define	UARTxLCR_H_FEN		(1 << 4)	/* RW イネーブルFIFO */
#define	UARTxLCR_H_STP2		(1 << 3)	/* RW ストップビット選択 */
#define	UARTxLCR_H_EPS		(1 << 2)	/* RW 偶数パリティ選択 */
#define	UARTxLCR_H_PEN		(1 << 1)	/* RW パリティイネーブル */
#define	UARTxLCR_H_BRK		(1 << 0)	/* RW 送信ブレーク */

#define	UARTxCR_CTSEN		(1 << 15)	/* RW CTS ハードウェアフロー制御イネーブル */
#define	UARTxCR_RTSEN		(1 << 14)	/* RW RTS ハードウェアフロー制御イネーブル */
#define	UARTxCR_RTS		(1 << 11)	/* RW 送信要求(RTS) */
#define	UARTxCR_DTR		(1 << 10)	/* RW データ送信準備完了 (DTS) */
#define	UARTxCR_RXE		(1 << 9)	/* RW 受信イネーブル */
#define	UARTxCR_TXE		(1 << 8)	/* RW 送信イネーブル */
#define	UARTxCR_SIRLP		(1 << 2)	/* RW IrDA 　SIR 低電力モード */
#define	UARTxCR_SIREN		(1 << 1)	/* RW SIR イネーブル */
#define	UARTxCR_UARTEN		(1 << 0)	/* RW UART イネーブル */

#define	UARTxIFLS_RXIFLSEL(n)	((n) << 3)	/* RW 受信割り込み FIFO レベル選択 */
#define	UARTxIFLS_TXIFLSEL(n)	((n) << 0)	/* RW 送信割り込み FIFO レベル選択 */
#define	UARTxIFLS_1_8		0		/* FIFO 1/8 */
#define	UARTxIFLS_1_4		1		/* FIFO 1/4 */
#define	UARTxIFLS_1_2		2		/* FIFO 1/2 */
#define	UARTxIFLS_3_4		3		/* FIFO 3/4 */
#define	UARTxIFLS_7_8		4		/* FIFO 7/8 */

#define	UARTxINT_OE		(1 << 10)	/* RW オーバーランエラー割り込み */
#define	UARTxINT_BE		(1 << 9)	/* RW ブレークエラー割り込み */
#define	UARTxINT_PE		(1 << 8)	/* RW パリティーエラー割り込み */
#define	UARTxINT_FE		(1 << 7)	/* RW フレーミングエラー割り込み */
#define	UARTxINT_RT		(1 << 6)	/* RW 受信タイムアウト割り込み */
#define	UARTxINT_TX		(1 << 5)	/* RW 送信割り込み */
#define	UARTxINT_RX		(1 << 4)	/* RW 受信割り込み */
#define	UARTxINT_DSR		(1 << 3)	/* RW DSR モデム割り込み */
#define	UARTxINT_DCD		(1 << 2)	/* RW DCD モデム割り込み */
#define	UARTxINT_CTS		(1 << 1)	/* RW CTS モデム割り込み */
#define	UARTxINT_RI		(1 << 0)	/* RW RIN モデム割り込み */

#define	UARTxDMACR_DMAONERR	(1 << 2)	/* RW DMA オンエラー */
#define	UARTxDMACR_TXDMAE	(1 << 1)	/* RW 送信DMA イネーブル */
#define	UARTxDMACR_RXDMAE	(1 << 0)	/* RW 受信 DMA イネーブル */

#define	SCx_RX_IRQ_NUMBER(n)	(16 + (n)*2)	/* n=0～3 : SIO/UART */
#define	SCx_TX_IRQ_NUMBER(n)	(17 + (n)*2)
#define	UARTx_IRQ_NUMBER(n)	(24 + (n)-4)	/* n=4～5 : UART modem */
#define	SBIx_IRQ_NUMBER(n)	(26 + (n))	/* n=0～2 : I2C/SIO */
#define	SSPx_IRQ_NUMBER(n)	(29 + (n))	/* n=0〜2 : SSP/SPI */

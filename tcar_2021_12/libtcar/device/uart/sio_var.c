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
 *	sio_var.c
 *
 *	UART 定義データ
 */

#include "sio_header.h"
#include "uart_def.h"

/* 各UARTに対応したレジスタベースアドレス宣言 */
EXPORT	const	UW	uart_reg_base[] = {
					UART0_BASE,
					UART1_BASE,
					UART2_BASE,
					UART3_BASE,
					UART4_BASE,
					UART5_BASE
};

/*
 *----------------------------------------------------------------------
 *    UCT 6LoWPAN Protocol Stack
 *
 *    Copyright (c) 2014-2016 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.00
 *    Released by UC Technology (http://www.uctec.com/) at 2014/08/27.
 *    Modified by UC Technology at 2016/09/30.
 *
 *----------------------------------------------------------------------
 */

#ifndef __UTKN_UTKN_H__
#define __UTKN_UTKN_H__

#include <tk/tkernel.h>
#include <utkn/utkn_endian.h>

/* エラーコード */
#define EN_IDLE		(-257)	/* スタンバイ可能な状態となった */
#define EN_ORPHAN	(-258)	/* 6LBRが見つからない状態となった */

#define RF_CHANNEL_ANY	(0)

#define WPAN_SCAN_ACTIVE		(0x01)
#define WPAN_SCAN_PASSIVE		(0x02)
#define WPAN_SCAN_ENHANCED_ACTIVE	(0x07)
#define WPAN_SCAN_NONE			(0xff)

#define WPAN_OPT_INDIRECT		(0x01)	/* 間接送受信を用いる */
#define WPAN_OPT_ONLYMAC		(0x02)	/* MAC層のみを使用する */
#define WPAN_OPT_EXITORPHAN		(0x04)	/* 6LBRが見つからない場合に終了 */
#define WPAN_OPT_MACSECURITY		(0x08)	/* MAC層セキュリティを有効にする */


typedef struct {
	UB	channel;
	UB	scan_type;
	UB	scan_duration;
	UB	options;
	TMO	tmout;
} utkn_6ln_init_t;

IMPORT ER utkn_6ln_init(const utkn_6ln_init_t *p);

typedef struct {
	UB	addr[16];
} ip6_addr_t;

typedef struct {
	ip6_addr_t addr;
	UH port;
} udp6_cep_t;

#define UDP6_PORT_ANY (0)
IMPORT const ip6_addr_t ip6_addr_unspec;

IMPORT ID udp6_open(const udp6_cep_t *cep);
IMPORT ER udp6_close(ID cepid);
IMPORT INT udp6_send(ID cepid, const UB *buf, INT len, const udp6_cep_t *dst, TMO tmout);
IMPORT INT udp6_recv(ID cepid, UB *buf, INT len, udp6_cep_t *src, udp6_cep_t *dst, UB *rssi, TMO tmout);

#endif /* __UTKN_UTKN_H__ */

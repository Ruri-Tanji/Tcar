/*
 *----------------------------------------------------------------------
 *    UCT 6LoWPAN Development Kit - CoAP Client/Server Library
 *
 *    Copyright (c) 2014 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 */

#ifndef __UTKN_COAP_H__
#define __UTKN_COAP_H__

#include <tk/tkernel.h>
#include <sys/queue.h>

/* デフォルトのポート番号 */
#define COAP_PORT_DEFAULT	(5683)

/* サービスの初期化 */
typedef struct {
	INT		maxreqq; /* 要求・応答の同時キューイング数 */
	INT		maxlogq; /* 送受信履歴の同時キューイング数 */
	INT		maxfsz; /* フレーム全体の最大サイズ */
	udp6_cep_t	cep;
} coap_init_t;

/* トークン */
typedef struct {
	INT		len;
	UB		data[8];
} coap_token_t;

/* メッセージタイプ */
#define COAP_T_CON	(0U)
#define COAP_T_NON	(1U)
#define COAP_T_ACK	(2U)
#define COAP_T_RST	(3U)

/* メッセージコード */
#define COAP_METHOD_GET				(0x01U)
#define COAP_METHOD_POST			(0x02U)
#define COAP_METHOD_PUT				(0x03U)
#define COAP_METHOD_DELETE			(0x04U)

#define COAP_CODE_CREATED			(0x41U)
#define COAP_CODE_DELETED			(0x42U)
#define COAP_CODE_VALID				(0x43U)
#define COAP_CODE_CHANGED			(0x44U)
#define COAP_CODE_CONTENT			(0x45U)
#define COAP_CODE_CONTINUE			(0x5fU)
#define COAP_CODE_BAD_REQUEST			(0x80U)
#define COAP_CODE_UNAUTHORIZED			(0x81U)
#define COAP_CODE_BAD_OPTION			(0x82U)
#define COAP_CODE_FORBIDDEN			(0x83U)
#define COAP_CODE_NOT_FOUND			(0x84U)
#define COAP_CODE_METHOD_NOT_ALLOWED		(0x85U)
#define COAP_CODE_NOT_ACCEPTABLE		(0x86U)
#define COAP_CODE_REQUEST_ENTITY_INCOMPLETE	(0x88U)
#define COAP_CODE_PRECONDITION_FAILED		(0x8cU)
#define COAP_CODE_REQUEST_ENTITY_TOO_LARGE	(0x8dU)
#define COAP_CODE_UNSUPPORTED_CONTENT_FORMAT	(0x8fU)
#define COAP_CODE_INTERNAL_SERVER_ERROR		(0xa0U)
#define COAP_CODE_NOT_IMPLEMENTED		(0xa1U)
#define COAP_CODE_BAD_GATEWAY			(0xa2U)
#define COAP_CODE_SERVICE_UNAVAILABLE		(0xa3U)
#define COAP_CODE_GATEWAY_TIMEOUT		(0xa4U)
#define COAP_CODE_PROXYING_NOT_SUPPORTED	(0xa5U)

/* メッセージ */
typedef struct {
	/* 接続先情報*/
	udp6_cep_t	dst;
	UB		rssi;

	/* Header */
	UB		ver;
	UB		t;
	coap_token_t	token;
	UB		code;
	UH		msgid;

	/* Options */
	UB*		uri_host;
	UB*		uri_path;
	UINT		uri_port;
	UB*		uri_query;
	UB*		location_path;
	UB*		location_query;
	UB*		proxy_uri;
	UB*		proxy_scheme;
	UB*		if_match;
	UB*		etag;
	BOOL		if_none_match; /* exists or not */
	INT		content_format;
	UINT		max_age; /* defaults to 60 */
	INT		accept; /* -1: 指定なし */
	INT		size1; /* -1: 指定なし */
	INT		size2; /* -1: 指定なし */
	INT		block1; /* -1: 指定なし */
	INT		block2; /* -1: 指定なし */

	/* Vendor-specific options*/
	BOOL		use_tls;

	/* Option Length
	   (アラインメントによるメモリ消費増加を避けるため、ポインタとは別にまとめる */
	UB		uri_host_len;
	UB		uri_path_len;
	UB		uri_query_len;
	UB		location_path_len;
	UB		location_query_len;
	UB		proxy_scheme_len;
	UB		proxy_uri_len;
	UB		if_match_len;
	UB		etag_len;

	/* Body */
	UB*		body;
	INT		bodysz;
	INT		bodymax;
} coap_msg_t;

/* target は定数文字列 */
#define coap_option_isequal(opt, target) \
	((opt##_len == sizeof(target)-1) && strncmp((const char*)(opt), target, sizeof(target)-1) == 0)
#define coap_body_isequal(body, target) \
	((body##sz == strlen(target)) && strncmp((const char*)(body), target, strlen(target))==0)

IMPORT ER coap_init( const coap_init_t* p );

/* GDI風の低レベルAPI */
IMPORT void coap_msg_init( coap_msg_t* msg, INT toklen );

IMPORT ER coap_accept( coap_msg_t* req, TMO tmout );
IMPORT ER coap_acknowledge( coap_msg_t* req );
IMPORT ER coap_reply( coap_msg_t* rpl, TMO tmout );
IMPORT ER coap_ignore( coap_msg_t* req );

IMPORT ER coap_request( coap_msg_t* req, TMO tmout );
IMPORT ER coap_request_end( coap_msg_t* req );


/* デバッグ用インタフェース (利用すると大幅にバイナリサイズは増加) */
typedef int coap_dbgspt_printfn_t( const char* fmt, ... );

IMPORT const char* coap_code_to_text( UB code );
IMPORT UINT coap_code_to_http( UB code );
IMPORT UB coap_code_from_http( UINT code );
IMPORT void coap_msg_dump( const coap_msg_t* msg, const char* header, coap_dbgspt_printfn_t* printfn );

#define USE_COAP_STAT	1

#if USE_COAP_STAT
typedef struct {
	struct {
		UW con;		/* CONの送信回数 */
		UW non;		/* NONの送信回数 */
		UW ack;		/* ACKの送信回数 (Piggybacked ACK除く) */
		UW rst;		/* RSTの送信回数 */
		UW pack;	/* Piggybacked ACKの送信回数 */
		UW rtx;		/* 再送回数 */
		UW loss;	/* 送信失敗回数 */
	} tx;
	struct {
		UW con;		/* CONの受信回数 */
		UW non;		/* NONの受信回数 */
		UW ack;		/* ACKの受信回数 (Piggybacked ACK除く) */
		UW rst;		/* RSTの受信回数 */
		UW pack;	/* Piggybacked ACKの受信回数 */
		UW drop;	/* 破棄した回数 */
	} rx;
} coap_stat_t;

IMPORT void coap_stat_get(coap_stat_t *stat);
IMPORT void coap_stat_reset(void);
#endif /* USE_COAP_STAT */

#endif /* __UTKN_COAP_H__ */

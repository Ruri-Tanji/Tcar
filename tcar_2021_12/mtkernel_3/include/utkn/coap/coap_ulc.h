/*
 *----------------------------------------------------------------------
 *    UCT 6LoWPAN Development Kit - CoAP Ultra-Light Client Library
 *
 *    Copyright (c) 2015 UC Technology. All Rights Reserved.
 *----------------------------------------------------------------------
 */

#ifndef __UTKN_COAP_ULC_H__
#define __UTKN_COAP_ULC_H__

#include <utkn/coap/coap.h>

/* coap_request() を2つに分割したもの */
IMPORT ER coap_ulc_send( coap_msg_t* msg, TMO tmout );
IMPORT ER coap_ulc_recv( coap_msg_t* msg, TMO tmout );

#endif /* __UTKN_COAP_ULC_H__ */

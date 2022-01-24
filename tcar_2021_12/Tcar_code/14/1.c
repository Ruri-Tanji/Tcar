#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>
#include <utkn/utkn.h>
#include <utkn/coap/coap.h>

#define HEADLIGHT_ADR   0x18

const UB SERVER_ADDR[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};
const UB SERVER_HOST[] = "tcar-1f10190221.herokuapp.com";
const UB SERVER_PATH[] = "api/v1/tcar/1/controls";

#define CTL_FLG_RUN 0x00000001

UB data[2];
SZ asz;
ER err;

ID coap_tskid;

void coap_client(void)
{
  static coap_msg_t req, rpl;
  ID dd;
  dd = tk_opn_dev("iicb", TD_UPDATE);

  data[0] = 0x00;
  data[1] = 0x00;
  err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

  data[0] = 0;
  data[1] = 0x00;
  err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

  data[0] = 1;
  data[1] = 0x05;
  err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

  data[0] = 0x0c;
  data[1] = 0x2a;
  err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

  data[0] = 0x0d;
  data[1] = 0x2a;
  err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

  /* 0.6LowPANミドルウェアの初期化 */
  static utkn_6ln_init_t param;
  ER err;

  param.channel       = RF_CHANNEL_ANY;
  param.scan_type     = WPAN_SCAN_ACTIVE;
  param.scan_duration = 4;
  param.options       = 0;
  param.tmout         = TMO_FEVR;
  err = utkn_6ln_init(&param);

  /* 1.CoAPサービス初期化 */
  static  coap_init_t coap;

  coap.maxreqq    = 16;
  coap.maxlogq    = 128;
  coap.maxfsz     = 256;
  coap.cep.addr   = ip6_addr_unspec;
  coap.cep.port   = htons(COAP_PORT_DEFAULT);
  err = coap_init(&coap);
  
  static UB buf[128];
  while(1){
    /* 2.CoAP要求メッセージの作成*/
    coap_msg_t msg;
    coap_msg_init(&msg, 4);

    memcpy(msg.dst.addr.addr, SERVER_ADDR, sizeof(msg.dst.addr.addr));
    msg.dst.port = htons(COAP_PORT_DEFAULT);

    msg.t             = COAP_T_CON;
    msg.code          = COAP_METHOD_GET;
    msg.uri_path      = (UB*)SERVER_PATH;
    msg.uri_path_len  = strlen(SERVER_PATH);
    msg.uri_host      = (UB*)SERVER_HOST;
    msg.uri_host_len  = strlen(SERVER_HOST);
    msg.body          = NULL;
    msg.bodysz        = -1;
    msg.bodymax       = 0;

    msg.content_format  = 50;
    msg.use_tls = TRUE;

    /* 3.CoAP要求の実行*/
    //coap_msg_dump(&msg, "Request", (coap_dbgspt_printfn_t*)tm_printf);
    err = coap_request(&msg, 30000);

    /* 4.受信したCoAP要求に対する応答の処理 */
    if(err >= E_OK){
      coap_msg_dump(&msg, "Reply", (coap_dbgspt_printfn_t*)tm_printf);
      if (msg.bodysz > 0){
        if(coap_body_isequal(msg.body, "turn-red")){
          data[0] = 0x02;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x03;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x04;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x06;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x07;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x08;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
        }else if(coap_body_isequal(msg.body, "turn-green")){
          data[0] = 0x02;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x03;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x04;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x06;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x07;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x08;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
        }else if(coap_body_isequal(msg.body, "turn-blue")){
          data[0] = 0x02;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x03;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x04;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x06;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x07;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x08;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
        }else if(coap_body_isequal(msg.body, "turn-white")){
          data[0] = 0x02;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x03;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x04;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x06;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x07;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x08;
          data[1] = 0xff;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
        }else if(coap_body_isequal(msg.body, "turn-off")){
          data[0] = 0x02;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x03;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x04;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x06;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x07;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
          data[0] = 0x08;
          data[1] = 0x00;
          err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
        }else{
          tm_printf("err");
        }
      }else{
        tm_printf((UB*)"ERR:coap_request: %d\n", err);
      }
    }
    /* 5.CoAP応答の実行完了*/
    coap_request_end(&msg);
  }
}

EXPORT  INT usermain(void){
    T_CTSK  coap_ctsk;
    ER      err;

    coap_ctsk.tskatr = TA_HLNG | TA_RNG3;
    coap_ctsk.task   = (FP)coap_client;
    coap_ctsk.itskpri= 10;
    coap_ctsk.stksz  = 1024;

    coap_tskid = tk_cre_tsk( &coap_ctsk );
    tk_sta_tsk(coap_tskid, 0);

    tk_slp_tsk(TMO_FEVR);

    return 0;
}
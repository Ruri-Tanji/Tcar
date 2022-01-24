#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */


// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

// GPIOポートE
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)

// GPIOポートE
#define PE_DATA 0x400C0400
#define PE_CR   0x400C0404

ID led_tskid, sw_tskid;
ID mbfid;
SYSTIM pk_tim;

void sw_task(INT stackd, void *exinf)
{

    UW sw3, sw4;

    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PECR &= ~(1<<7);
    *(_UW*)PEIE |= (1<<7);

    INT msg;

    while(1) {
      sw3 = *(_UW*)PADATA & (1<<3);
      if(sw3 == 0){
        msg = 3;
        tk_snd_mbf(mbfid, &msg, sizeof(msg), TMO_FEVR);
      }
      sw4 = *(_UW*)PEDATA & (1<<7);
      if(sw4 == 0){
        msg = 4;
        tk_snd_mbf(mbfid, &msg, sizeof(msg), TMO_FEVR);
      }
      tk_dly_tsk(100);
    }
    
    /* このタスクは終了しない */
}

void led_task(INT stackd, void *exinf)
{

    _UW   i;
  
    /* GPIOポートEの初期化 */
    *(_UW*)(PE_CR) |= (1<<3);
    *(_UW*)(PE_CR) |= (1<<2);

    INT msg;

    while(1) {
        tk_rcv_mbf(mbfid, &msg, TMO_FEVR);

        if( msg == 3 ){
          *(_UB*)(PE_DATA) |= (1<<3);
          tk_dly_tsk(1000);
          *(_UB*)(PE_DATA) &= ~(1<<3);
          tk_get_tim(&pk_tim);
          tm_printf("Time: %d\n", &pk_tim);
        }
        else if(msg == 4){
          *(_UB*)(PE_DATA) |= (1<<2);
          tk_dly_tsk(1000);
          *(_UB*)(PE_DATA) &= ~(1<<2);
          tk_get_tim(&pk_tim);
          tm_printf("Time: %d\n", &pk_tim);
        }
    }
    
    /* このタスクは終了しない */
}

EXPORT INT usermain(void)
{
    typedef struct {
        W   hi;
        UW  lo;
    } SYSTIM;

    typedef struct {
        INT     sw;
        SYSTIM  time;
    } T_MSG_SW;

    T_CTSK led_ctsk;
    T_CTSK sw_ctsk;

    T_CMBF cmbf;

    cmbf.mbfatr = TA_TFIFO;
    cmbf.bufsz  = sizeof(INT) * 10;
    cmbf.maxmsz = sizeof(INT);

    mbfid = tk_cre_mbf(&cmbf);

    led_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    led_ctsk.task    = led_task;
    led_ctsk.itskpri = 10;
    led_ctsk.stksz   = 1024;
    sw_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    sw_ctsk.task    = sw_task;
    sw_ctsk.itskpri = 10;
    sw_ctsk.stksz   = 1024;


    led_tskid = tk_cre_tsk( &led_ctsk );

    sw_tskid = tk_cre_tsk( &sw_ctsk );

    tk_sta_tsk(led_tskid, 0);
    tk_sta_tsk(sw_tskid, 0);

    tk_slp_tsk(TMO_FEVR);
    return 0;
}
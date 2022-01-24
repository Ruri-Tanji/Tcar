/* LED制御タスク */

#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートE
#define PE_DATA 0x400C0400
#define PE_CR   0x400C0404

// ADC定義
#define ADA_BASE 0x40050000
#define ADACLK  (ADA_BASE + 0x00)
#define ADAMOD0 (ADA_BASE + 0x04)
#define ADAMOD1 (ADA_BASE + 0x08)
#define ADAMOD3 (ADA_BASE + 0x10)
#define ADAMOD4 (ADA_BASE + 0x14)
#define ADAMOD5 (ADA_BASE + 0x18)
#define ADAREG00 (ADA_BASE + 0x34)
#define ADAREG01 (ADA_BASE + 0x38)
#define ADAREG02 (ADA_BASE + 0x3c)
#define ADAREG03 (ADA_BASE + 0x40)

void led_task(INT stackd, void *exinf)
{

    _UW   i;
  
    /* GPIOポートEの初期化 */
    *(_UW*)(PE_CR) |= (1<<3);

    UINT d;

    /* ADC */
    *(_UW*)ADACLK  = 0x01;  // クロック供給
    *(_UW*)ADAMOD1 = 0x80;  // ADC有効化
    tk_dly_tsk(1);   // 3マイクロ秒以上待つ
    *(_UW*)ADAMOD3 = 0x01;  // チャンネルスキャンモード
    *(_UW*)ADAMOD4 = 0x00;  // AINA0の1チャンネルのみ

    while(1) {
        /* A/D変換を行う */
        *(_UW*)ADAMOD0 = 0x01; // 変換開始
        while ((*(_UW*)ADAMOD5 & (0x01<<1)) == 0);  // 変換完了待ち
        d = *(_UW*)ADAREG00 & 0b1111111111111;          // AINA0の変換結果 12ビット分のみを読み出す

        if (d>0 && d< 5000){
          *(_UW*)(PE_DATA) |= (1<<3);
        }
        else{
          *(_UW*)(PE_DATA) &= ~(1<<3);
        }
        tk_dly_tsk(100); // 100 ミリ秒待つ
    }
    
    /* このタスクは終了しない */
}

EXPORT INT usermain(void)
{
    T_CTSK led_ctsk;
    ID     led_tskid;

    led_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    led_ctsk.task    = led_task;
    led_ctsk.itskpri = 10;
    led_ctsk.stksz   = 1024;

    led_tskid = tk_cre_tsk( &led_ctsk );
    tk_sta_tsk(led_tskid, 0);

    tk_slp_tsk(TMO_FEVR);
    return 0;
}
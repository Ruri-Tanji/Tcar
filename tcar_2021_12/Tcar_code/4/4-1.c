/* ブザー制御タスク */

#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// MPT2タイマ
#define	MT2_BASE	(0x400C7200UL)
#define	MT2EN   (MT2_BASE + 0x00)
#define	MT2RUN	(MT2_BASE + 0x04)
#define	MT2IGCR	(MT2_BASE + 0x30)
#define	MT2IGOCR	(MT2_BASE + 0x40)
#define	MT2IGRG2	(MT2_BASE + 0x44)
#define	MT2IGRG3	(MT2_BASE + 0x48)
#define	MT2IGRG4	(MT2_BASE + 0x4C)

// GPIOポートH
#define GPIOH_BASE (0x400C0700)
#define PHCR    (GPIOH_BASE + 0x04)
#define PHFR3   (GPIOH_BASE + 0x10)
#define PHIE    (GPIOH_BASE + 0x38)

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

void bz_task(INT stackd, void *exinf)
{
    UW sw3;

    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);

    // MTP2初期設定
    *(_UW*)MT2EN     |= 0b10000001;
    *(_UW*)MT2IGOCR  |= (1<<1);
    *(_UW*)MT2IGOCR  &= ~(1<<5);
    *(_UW*)MT2IGCR   &= 0b11010000;

    // 端子をMTP2に設定
    *(_UW*)PHFR3   |=  (1<<2);
    *(_UW*)PHIE    &=  ~(1<<2);
    *(_UW*)PHCR   |=  (1<<2);
  
    // MTP2 出力波形の設定 
    *(_UW*)MT2IGRG2  = 1;
    *(_UW*)MT2IGRG4  = 18000;
    *(_UW*)MT2IGRG3  = 9000;

    while(1) {
      sw3 = *(_UW*)PADATA & (1<<3);
      if(sw3 == 0){
        *(_UW*)MT2RUN |= ((1<<2) | (1<<0));
        tk_dly_tsk(2000);
        *(_UW*)MT2RUN &= ~((1<<2) | (1<<0));
      }
    }
    
    /* このタスクは終了しない */
}

EXPORT INT usermain(void)
{
    T_CTSK bz_ctsk;
    ID     bz_tskid;

    bz_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    bz_ctsk.task    = bz_task;
    bz_ctsk.itskpri = 10;
    bz_ctsk.stksz   = 1024;

    bz_tskid = tk_cre_tsk( &bz_ctsk );
    tk_sta_tsk(bz_tskid, 0);

    tk_slp_tsk(TMO_FEVR);
    return 0;
}
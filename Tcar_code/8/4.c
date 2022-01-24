/*
 *	usermain.c (usermain)
 *	User Main
 */
#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートE
#define PE_DATA 0x400C0400
#define PE_CR   0x400C0404

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)
#define PAFR3   (GPIOA_BASE + 0x010) 

// GPIOポートE
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)
#define PEFR4   (GPIOE_BASE + 0x0014)

#define CGICRCG 0x400F3060

#define INT3    3
#define INT6    6

ID led_right_tskid, led_left_tskid;

void inthdr(UINT intno){
    ClearInt(INT3);
    tk_wup_tsk(led_left_tskid);

    return;
}

void inthdr1(UINT intno){
    ClearInt(INT6);
    tk_wup_tsk(led_right_tskid);

    return;
}

void led_left_task(INT stackd, void *exinf)
{
    /* GPIOポートEの初期化 */
    *(_UW*)(PE_CR) |= (1<<3);

    while(1) {
        tk_slp_tsk(TMO_FEVR);
        *(_UB*)(PE_DATA) |= (1<<3);
        tk_dly_tsk(1000);
        *(_UB*)(PE_DATA) &= ~(1<<3);
    }
    
    /* このタスクは終了しない */
}

void led_right_task(INT stackd, void *exinf)
{  
    /* GPIOポートEの初期化 */
    *(_UW*)(PE_CR) |= (1<<2);

    while(1) {
        tk_slp_tsk(TMO_FEVR);
        *(_UB*)(PE_DATA) |= (1<<2);
        tk_dly_tsk(1000);
        *(_UB*)(PE_DATA) &= ~(1<<2);
    }
    
    /* このタスクは終了しない */
}

EXPORT INT usermain(void)
{

    UINT intsts;

    T_DINT  dint;
    dint.intatr = TA_HLNG;
    dint.inthdr = inthdr;
    tk_def_int(INT3, &dint);

    T_DINT  dint1;
    dint1.intatr = TA_HLNG;
    dint1.inthdr = inthdr1;
    tk_def_int(INT6, &dint1);

    DI(intsts);
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PAFR3 |= (1<<3);
    *(_UW*)PECR &= ~(1<<7);
    *(_UW*)PEIE |= (1<<7);
    *(_UW*)PEFR4 |= (1<<7);

    SetIntMode(INT3, IM_EDGE | IM_LOW);
    SetIntMode(INT6, IM_EDGE | IM_LOW);

    ClearInt(INT3);
      EnableInt(INT3, 5);

    ClearInt(INT6);
      EnableInt(INT6, 5);
    EI(intsts);

    T_CTSK led_left_ctsk;

    led_left_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    led_left_ctsk.task    = led_left_task;
    led_left_ctsk.itskpri = 10;
    led_left_ctsk.stksz   = 1024;

    led_left_tskid = tk_cre_tsk( &led_left_ctsk );

    tk_sta_tsk(led_left_tskid, 0);



    T_CTSK led_right_ctsk;

    led_right_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    led_right_ctsk.task    = led_right_task;
    led_right_ctsk.itskpri = 10;
    led_right_ctsk.stksz   = 1024;

    led_right_tskid = tk_cre_tsk( &led_right_ctsk );
    
    tk_sta_tsk(led_right_tskid, 0);
    tk_slp_tsk(TMO_FEVR);

    return 0;
}
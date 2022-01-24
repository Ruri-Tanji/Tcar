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

ID A_tskid, B_tskid, C_tskid;
ID mtxid;


void A_task(INT stackd, void *exinf)
{
    _UW   i;
  
    /* GPIOポートEの初期化 */
    *(_UW*)(PE_CR) |= (1<<3);

    T_RTSK rtsk;

    while(1) {
      tk_loc_mtx(mtxid,TMO_FEVR);
      tk_ref_tsk(0, &rtsk);
      tm_printf( "A1:%d\n", rtsk.tskpri);
      for(i=0; i<10; i++){
        *(_UB*)(PE_DATA) |= (1<<3);
        tk_dly_tsk(100);
        *(_UB*)(PE_DATA) &= ~(1<<3);
        tk_dly_tsk(100);
      }
      tk_ref_tsk(0, &rtsk);
      tm_printf( "A1:%d\n", rtsk.tskpri);
      tk_unl_mtx(mtxid);
    }  
}

void B_task(INT stackd, void *exinf)
{
    _UW   i;
  
    /* GPIOポートEの初期化 */
    *(_UW*)(PE_CR) |= (1<<3);

    T_RTSK rtsk;

    while(1) {
      tk_loc_mtx(mtxid,TMO_FEVR);
      tk_ref_tsk(0, &rtsk);
      tm_printf( "B1:%d\n", rtsk.tskpri);
      for(i=0; i<10; i++){
        *(_UB*)(PE_DATA) |= (1<<3);
        tk_dly_tsk(1000);
        *(_UB*)(PE_DATA) &= ~(1<<3);
        tk_dly_tsk(1000);
      }
      tk_ref_tsk(0, &rtsk);
      tm_printf( "B2:%d\n", rtsk.tskpri);
      tk_unl_mtx(mtxid);
    }
}

void C_task(INT stackd, void *exinf)
{
    _UW   i;
  
    /* GPIOポートEの初期化 */
    *(_UW*)(PE_CR) |= (1<<3);

    T_RTSK rtsk;

    while(1) {
      tk_loc_mtx(mtxid,TMO_FEVR);
      tk_ref_tsk(0, &rtsk);
      tm_printf( "C1:%d\n", rtsk.tskpri);
      for(i=0; i<10; i++){
        *(_UB*)(PE_DATA) |= (1<<3);
        tk_dly_tsk(500);
        *(_UB*)(PE_DATA) &= ~(1<<3);
        tk_dly_tsk(500);
      }
      tk_ref_tsk(0, &rtsk);
      tm_printf( "C2:%d\n", rtsk.tskpri);
      tk_unl_mtx(mtxid);
    }
}

EXPORT INT usermain(void)
{
    T_CTSK A_ctsk;
    T_CTSK B_ctsk;
    T_CTSK C_ctsk;

    T_CMTX cmtx;

    cmtx.mtxatr = TA_CEILING;
    cmtx.ceilpri = 8;

    mtxid = tk_cre_mtx(&cmtx);

    A_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    A_ctsk.task    = A_task;
    A_ctsk.itskpri = 10;
    A_ctsk.stksz   = 1024;
    B_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    B_ctsk.task    = B_task;
    B_ctsk.itskpri = 10;
    B_ctsk.stksz   = 1024;
    C_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    C_ctsk.task    = C_task;
    C_ctsk.itskpri = 10;
    C_ctsk.stksz   = 1024;

    A_tskid = tk_cre_tsk( &A_ctsk );
    B_tskid = tk_cre_tsk( &B_ctsk );
    C_tskid = tk_cre_tsk( &C_ctsk );

    tk_sta_tsk(A_tskid, 0);
    tk_sta_tsk(B_tskid, 0);
    tk_sta_tsk(C_tskid, 0);

    tk_slp_tsk(TMO_FEVR);
    return 0;
}
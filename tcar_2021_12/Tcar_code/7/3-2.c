#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

ID Send_tskid, Receive_tskid;
ID mbfid, mplid;

void Send_task(INT stackd, void *exinf)
{
    void    *blk;
    T_RMPL  rmpl;

    while(1) {
        tk_get_mpl(mplid, 100, &blk, TMO_FEVR);

        tk_snd_mbf(mbfid, &blk, sizeof(blk), TMO_FEVR);
        tm_printf("Send: %d\n", &blk);
    }
}

void Receive_task(INT stackd, void *exinf)
{
    void    *blk;
    T_RMPL  rmpl;

    while(1) {
        tk_rcv_mbf(mbfid, &blk, TMO_FEVR);
        tm_printf("Receive: %d\n", &blk);

        //tk_rel_mpl(mplid, blk);
    }
}

EXPORT INT usermain(void)
{
    T_CTSK Send_ctsk;
    T_CTSK Receive_ctsk;

    T_CMBF cmbf;

    cmbf.mbfatr = TA_TFIFO;
    cmbf.bufsz  = sizeof(INT) * 10;
    cmbf.maxmsz = sizeof(INT);

    mbfid = tk_cre_mbf(&cmbf);

    T_CMPL cmpl;

    cmpl.mplatr = TA_TFIFO | TA_RNG3;
    cmpl.mplsz = 1024;
    mplid = tk_cre_mpl(&cmpl);

    Send_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    Send_ctsk.task    = Send_task;
    Send_ctsk.itskpri = 10;
    Send_ctsk.stksz   = 1024;
    Receive_ctsk.tskatr  = TA_HLNG | TA_RNG3;
    Receive_ctsk.task    = Receive_task;
    Receive_ctsk.itskpri = 10;
    Receive_ctsk.stksz   = 1024;
    
    Send_tskid = tk_cre_tsk( &Send_ctsk );
    Receive_tskid = tk_cre_tsk( &Receive_ctsk );

    tk_sta_tsk(Send_tskid, 0);
    tk_sta_tsk(Receive_tskid, 0);

    tk_slp_tsk(TMO_FEVR);
    return 0;
}
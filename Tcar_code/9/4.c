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

#define CGICRCG 0x400F3060

#define INT3    3

ID cycid;

void inthdr(UINT intno){
    ClearInt(INT3);

    static BOOL is_on = FALSE;

    *(_UW*)PE_CR |= (1<<3);
    while(1){
      tk_slp_tsk(TMO_FEVR);
      if(is_on){
        tk_stp_cyc(cycid);
        *(_UB*)(PE_DATA) &= ~(1<<3);
        is_on = FALSE;
      }
      else{
        tk_sta_cyc(cycid);
        is_on = TRUE;
      }
    }

    return;
}


void cychdr(void *exinf)
{
    static BOOL is_on = FALSE;

    if(is_on){
      *(_UB*)(PE_DATA) &= ~(1<<3);
      is_on = FALSE;
    } 
    else{
      *(_UB*)(PE_DATA) |= (1<<3);
      is_on = TRUE;
    }
    
    return;
}

EXPORT INT usermain(void)
{
    UINT intsts;

    T_DINT  dint;
    dint.intatr = TA_HLNG;
    dint.inthdr = inthdr;
    tk_def_int(INT3, &dint);

    inthdr(intsts);

    DI(intsts);
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PAFR3 |= (1<<3);

    SetIntMode(INT3, IM_EDGE | IM_LOW);

    ClearInt(INT3);
      EnableInt(INT3, 5);
    EI(intsts);

    T_CCYC  ccyc;

    *(_UW*)PE_CR |= (1<<3);

    ccyc.cycatr = TA_HLNG;
    ccyc.cychdr = cychdr;
    ccyc.cyctim = 500;
    ccyc.cycphs = 500;

    cycid = tk_cre_cyc(&ccyc);

    tk_slp_tsk(TMO_FEVR);

    return 0;
}
/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 3.00.03
 *
 *    Copyright (C) 2006-2021 by Ken Sakamura.
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2021/03/31.
 *
 *----------------------------------------------------------------------
 */

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

/* ---------------------------------------------------------
 * Sample User Program
 * ---------------------------------------------------------
 * 
 * Entry routine for the user application.
 * At this point, Initialize and start the user application.
 *
 * Entry routine is called from the initial task for Kernel,
 * so system call for stopping the task should not be issued 
 * from the contexts of entry routine.
 * We recommend that:
 * (1)'usermain()' only generates the user initial task.
 * (2)initialize and start the user application by the user
 * initial task.
 */

#if USE_TMONITOR
#define TM_PUTSTRING(a)	tm_putstring(a)

void print_err( UB* str, ER err)
{
	tm_printf(str, err);
}

#else
#define TM_PUTSTRING(a)

void print_err( UB* str, INT par) {}

#endif /* USE_TMONITOR */

/* ----------------------------------------------------------
 *
 * User Task-1 Definition
 *
 */
void tsk1(INT stacd, void *exinf)
{
	TM_PUTSTRING((UB*)"Start Task-1\n");

	tk_exd_tsk();	/* Exit task */
}

/* ---------------------------------------------------------
 *
 * User Task-2 Definition
 *
 */
void tsk2(INT stacd, void *exinf)
{
	TM_PUTSTRING((UB*)"Start Task-2\n");

	tk_exd_tsk();	/* Exit Task */
}

const T_CTSK	ctsk1	= {0, (TA_HLNG | TA_RNG3), &tsk1, 10, 1024, 0};
const T_CTSK	ctsk2	= {0, (TA_HLNG | TA_RNG3), &tsk2, 11, 1024, 0};

/* ----------------------------------------------------------
 *
 * User-Main Definition (Run on initial task)
 *
 */

EXPORT INT usermain( void )
{
	T_RVER	rver;
	ID	id1, id2;

	TM_PUTSTRING((UB*)"Start User-main program.\n");

	tk_ref_ver(&rver);		/* Get the OS Version. */

#if USE_TMONTOR
	tm_printf((UB*)"Make Code: %04x  Product ID: %04x\n", rver.maker, rver.prid);
	tm_printf((UB*)"Product Ver. %04x\nProduct Num. %04x %04x %04x %04x\n", 
			rver.prver, rver.prno[0],rver.prno[1],rver.prno[2],rver.prno[3]);
#endif

	id1 = tk_cre_tsk(&ctsk1);
	tk_sta_tsk(id1, 0);

	id2 = tk_cre_tsk(&ctsk2);
	tk_sta_tsk(id2, 0);

	tk_slp_tsk(TMO_FEVR);

	return 0;
}

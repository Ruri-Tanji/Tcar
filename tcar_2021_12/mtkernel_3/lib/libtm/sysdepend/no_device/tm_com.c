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

/*
 *    tm_com.c
 *    T-Monitor Communication low-level device driver (No device)
 * 	This is a dummy program when there is no communication device.
 */

#include <tk/typedef.h>
#include <sys/sysdef.h>

#if USE_TMONITOR
#include "../../libtm.h"

#ifdef TM_COM_NO_DEV

EXPORT	void	tm_snd_dat( const UB* buf, INT size )
{
}


EXPORT	void	tm_rcv_dat( UB* buf, INT size )
{
}


EXPORT	void	tm_com_init(void)
{
}

#endif /* USE_COM_NO_DEVICE */
#endif /* USE_TMONITOR */
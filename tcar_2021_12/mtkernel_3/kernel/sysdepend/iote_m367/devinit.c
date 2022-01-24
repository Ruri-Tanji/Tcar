/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 3.00.00
 *
 *    Copyright (C) 2006-2019 by Ken Sakamura.
 *    This software is distributed under the T-License 2.1.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2019/12/11.
 *
 *----------------------------------------------------------------------
 */
#include <sys/machine.h>
#ifdef IOTE_M367

/*
 *	devinit.c (M367 IoT-Engine)
 *	Device-Dependent Initialization
 */

#include <sys/sysdef.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

#include "kernel.h"
#include "sysdepend.h"

/* ------------------------------------------------------------------------ */

/*
 * Initialization before micro T-Kernel starts
 */

EXPORT ER knl_init_device( void )
{
	return E_OK;
}

/* ------------------------------------------------------------------------ */
/*
 * Start processing after T-Kernel starts
 *	Called from the initial task contexts.
 */
EXPORT ER knl_start_device( void )
{

#if USE_SDEV_DRV	// Use sample driver
	ER	err;

	/* A/D Converter unit.A "adca" & Unit.B "adcb" */
	#if DEVCNF_DEV_ADC
		err = dev_init_adc(0);
		if(err < E_OK) return err;
		err = dev_init_adc(1);
		if(err < E_OK) return err;
	#endif

	/* I2C SBI1 "iicb" */
	#if DEVCNF_DEV_IIC
		err = dev_init_i2c(1);
		if(err < E_OK) return err;
	#endif

	/* Serial UART5 "serb" */
	#if DEVCNF_DEV_SER
		err = dev_init_ser(1);
		if(err < E_OK) return err;
	#endif
#endif

	/* T-Car device */
	*(_UW*)PORT_CR(L) |= (1<<1);
	*(_UW*)PORT_DATA(L) |= (1<<1);

	err = dev_init_pmd();
	if(err < E_OK) return err;

	return E_OK;
}

#if USE_SHUTDOWN
/* ------------------------------------------------------------------------ */
/*
 * System finalization
 *	Called just before system shutdown.
 *	Execute finalization that must be done before system shutdown.
 */
EXPORT ER knl_finish_device( void )
{
	return E_OK;
}

#endif /* USE_SHUTDOWN */

#endif /* IOTE_M367 */

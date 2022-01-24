/*
 * uT-Kernel PDM(Programmable Motor Driver) Device for TX03-M367
 * Copyright (c) 2019 by INIAD
 */

#include <sys/machine.h>
#include "../config/devconf.h"
#if DEVCNF_DEV_PMD

#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tstdlib.h>

#include "../common/drvif/msdrvif.h"
#include "../include/dev_pmd.h"

/*-------------------------------------------------------
 * PWM global data
 *-------------------------------------------------------
 */
#define		PMD_MAX_PERIOD	14563

typedef struct {
	ID		devid;		/* Device ID */
	UW		en;		/* PWM enable */
	UW		period;		/* PWM period */
	UW		pulse0;		/* pulse width 0 */
	UW		pulse1;		/* pulse width 1 */
	UW		pulse2;		/* pulse width 2 */
} T_PMDCB;

LOCAL T_PMDCB	pmdcb;

/*-------------------------------------------------------
 * PWM Register definition
 *-------------------------------------------------------
 */
#define PMD_BASE	0x400f6000
#define PMD_MTPDMDEN	(*(_UW*)(PMD_BASE + 0x00))
#define PMD_MTPDPORTMD	(*(_UW*)(PMD_BASE + 0x04))
#define PMD_MTPDMDCR	(*(_UW*)(PMD_BASE + 0x08))
#define PMD_MTPDMDCNT	(*(_UW*)(PMD_BASE + 0x10))
#define PMD_MTPDMDPRD	(*(_UW*)(PMD_BASE + 0x14))
#define PMD_MTPDCMPU	(*(_UW*)(PMD_BASE + 0x18))
#define PMD_MTPDCMPV	(*(_UW*)(PMD_BASE + 0x1c))
#define PMD_MTPDCMPW	(*(_UW*)(PMD_BASE + 0x20))
#define PMD_MTPDMDOUT	(*(_UW*)(PMD_BASE + 0x28))
#define PMD_MTPDEMGREL	(*(_UW*)(PMD_BASE + 0x30))
#define PMD_MTPDEMGCR	(*(_UW*)(PMD_BASE + 0x34))
#define PMD_MTPDEMGSTA	(*(_UW*)(PMD_BASE + 0x38))

/*-------------------------------------------------------
 * mSDI I/F Function
 *-------------------------------------------------------
 */
LOCAL ER dev_pmd_openfn( ID devid, UINT omode, T_MSDI *msdi)
{
	UINT	imask;

	PMD_MTPDMDEN = (0 << 0);	// PWMEN=0 Waveform synthesis function prohibited

	// Prohibit EMG protection
	DI(imask);
	PMD_MTPDEMGREL = 0x5a;		// EMGREL=0x5a
	PMD_MTPDEMGREL = 0xa5;		// EMGREL=0xa5
	PMD_MTPDEMGCR &= ~(1 << 0);	// EMGEN=0
	EI(imask);

	PMD_MTPDPORTMD = (1 << 0);	// PORTMD=1 (PMD output)
	PMD_MTPDMDCR = (1 << 0)		// PWMMD=1 (Triangle wave)
		| (1 << 1)		// INTPRD=1 (Interrupt every cycle)
		| (1 << 4)		// DTYMD=1 (3-phase independent)
		| (1 << 6);		// PWMCK=1 (4x cycle)
	PMD_MTPDMDOUT = (0 << 0)	// UOC=0
		| (0 << 2)		// VOC=0
		| (0 << 4)		// WOC=0
		| (0 << 8)		// UPWM=0
		| (0 << 9)		// VPWM=0
		| (0 << 10);		// WPWM=0

	return E_OK;
}

LOCAL ER dev_pmd_closefn( ID devid, UINT option, T_MSDI *msdi)
{
	PMD_MTPDMDEN = (0 << 0);	// PWMEN=0 Waveform synthesis function prohibited

	return E_OK;
}


LOCAL INT dev_pmd_readfn( T_DEVREQ *req, T_MSDI *p_msdi)
{
	UW	data;
	ER	err = E_OK;

	if(req->size != 1) return E_PAR;

	switch(req->start) {
	case PMD_DATA_EN:
		data = pmdcb.en;
		break;
	case PMD_DATA_PERIOD:
		data = pmdcb.period;
		break;
	case PMD_DATA_PULSE0:
		data = pmdcb.pulse0;
		break;
	case PMD_DATA_PULSE1:
		data = pmdcb.pulse1;
		break;
	case PMD_DATA_PULSE2:
		data = pmdcb.pulse2;
		break;
	default:
		err = E_PAR;
	}
	if(err == E_OK) {
		*(UW*)(req->buf) = data;
	}

	return err;
}

/*----------------------------------------------------------------------
 * Event Device
 */
ER dev_pmd_eventfn( INT evttyp, void *evtinf, T_MSDI *msdi)
{
	return E_NOSPT;
}

LOCAL INT dev_pmd_writefn( T_DEVREQ *req, T_MSDI *p_msdi)
{
	ER	err = E_OK;
	UW	data;
	UINT	imask;

	if(req->size != 1) return E_PAR;

	data = *(UW*)(req->buf);

	/* 範囲チェック */
	switch(req->size) {
	case PMD_DATA_EN:
		if(data > 1) err = E_PAR;
		break;
	case PMD_DATA_PERIOD:
		if( data <= 0 || data > PMD_MAX_PERIOD) err = E_PAR;
		break;
	case PMD_DATA_PULSE0:
	case PMD_DATA_PULSE1:
	case PMD_DATA_PULSE2:
		if( data <= 0 || data >= pmdcb.period) err = E_PAR;
		break;
	default:
		err = E_PAR;
	}
	if(err != E_OK ) return err;

	switch(req->start) {
	case PMD_DATA_EN:
		pmdcb.en = data;
		PMD_MTPDMDEN = data << 0;
		break;
	case PMD_DATA_PERIOD:
		pmdcb.period = data;
		data = data * 0xffff / PMD_MAX_PERIOD;
		PMD_MTPDMDPRD = data;
		break;
	case PMD_DATA_PULSE0:
		pmdcb.pulse0 = data;
		PMD_MTPDMDOUT |= (1 << 8); /* UPWM=1 */
		data = data * 0xffff / PMD_MAX_PERIOD;
		PMD_MTPDCMPU = data;
		break;
	case PMD_DATA_PULSE1:
		pmdcb.pulse1 = data;
		PMD_MTPDMDOUT |= (1 << 9); /* VPWM=1 */
		data = data * 0xffff / PMD_MAX_PERIOD;
		PMD_MTPDCMPV = data;
		break;
	case PMD_DATA_PULSE2:
		pmdcb.pulse2 = data;
		PMD_MTPDMDOUT |= (1 << 10); /* WPWM=1 */
		data = data * 0xffff / PMD_MAX_PERIOD;
		PMD_MTPDCMPW = data;
		break;
	default:
		err = E_PAR;
	}

	return err;
}


/*
 * PMD Device initialization and registration
 */
EXPORT ER dev_init_pmd( void )
{
	T_DMSDI		dmsdi;
	T_IDEV		idev;
	T_MSDI		*p_msdi;
	INT		n, i;
	ER		err;

	/* Pin setting PG5->VO PG6->XO */
	for(n = 5; n <= 6; n++) {
		UINT	imask;

		DI(imask);
		*(_UW*)PORT_FR1(G)	&= ~(1 << n);
		*(_UW*)PORT_FR2(G)	|= (1 << n); /* PWM */
		*(_UW*)PORT_FR3(G)	&= ~(1 << n);
		*(_UW*)PORT_FR4(G)	&= ~(1 << n);
		*(_UW*)PORT_IE(G)	&= ~(1 << n);
		*(_UW*)PORT_CR(G)	|= (1 << n);

		EI(imask);
	}

	/* Device registration information */
	dmsdi.exinf	= &pmdcb;
	dmsdi.drvatr	= 0;			/* Driver attributes */
	dmsdi.devatr	= TDK_UNDEF;		/* Device attributes */
	dmsdi.nsub	= 0;			/* Number of subunits */
	dmsdi.blksz	= 1;			/* Unique data block size (-1 = unknown) */
	dmsdi.openfn	= dev_pmd_openfn;
	dmsdi.closefn	= dev_pmd_closefn;
	dmsdi.readfn	= dev_pmd_readfn;
	dmsdi.writefn	= dev_pmd_writefn;
	dmsdi.eventfn	= dev_pmd_eventfn;
	
	knl_strcpy( dmsdi.devnm, "pmda");
	i = knl_strlen("pmda");
	dmsdi.devnm[i] = 0;

	err = msdi_def_dev( &dmsdi, &idev, &p_msdi);

	return err;
}

#endif		/* DEVCNF_DEV_PMD */
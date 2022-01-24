/*
 * uT-Kernel PDM Device for TX03-M367
 * Copyright (c) 2019 by INIAD
 */

#ifndef	__DEVINC_PDM_H__
#define	__DEVINC_PDM_H__

#define	PMD_DATA_EN		0
#define	PMD_DATA_PERIOD		1
#define	PMD_DATA_PULSE0		2
#define	PMD_DATA_PULSE1		3
#define	PMD_DATA_PULSE2		4

IMPORT ER dev_init_pmd( void );

#endif /* __DEVINC_PDM_H__ */
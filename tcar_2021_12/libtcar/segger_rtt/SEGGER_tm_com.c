/*
 *    tm_com.c
 *    T-Monitor Communication low-level device driver (SEGGER RTT)
 */

#include <tk/typedef.h>
#include <sys/sysdef.h>
#include <tstdlib.h>

#include "../../mtkernel_3/lib/libtm/libtm.h"
#include "RTT.h"

#define BUFFER_SIZE_UP		(1024)	// Size of the buffer for terminal output of target, up to host (Default: 1k)
#define BUFFER_SIZE_DOWN	(16)	// Size of the buffer for terminal input to target from host (Usually keyboard input) (Default: 16)

LOCAL B		_acUpBuffer[BUFFER_SIZE_UP];
LOCAL B		_acDownBuffer[BUFFER_SIZE_DOWN];
 RTT_CB	_SEGGER_RTT;

EXPORT	void	tm_snd_dat( const UB *buf, INT size )
{
	RTT_BUFFER	*rtt_buff;

	UW	avail;
	UW	rem;

	rtt_buff = &_SEGGER_RTT.aUp[0];

	/* Get free buffer space */
	if ( rtt_buff->RdOff <= rtt_buff->WrOff) {
		avail = rtt_buff->SizeOfBuffer - 1 - rtt_buff->WrOff + rtt_buff->RdOff;
	} else {
		avail = rtt_buff->RdOff - rtt_buff->WrOff - 1;
	}
	if (avail < size) return;	// There is no free space in the buffe

	rem = rtt_buff->SizeOfBuffer - rtt_buff->WrOff;
	if (rem > size) {
		knl_memcpy(rtt_buff->pBuffer + rtt_buff->WrOff, buf, size);
		rtt_buff->WrOff += size;
	} else {
		knl_memcpy(rtt_buff->pBuffer + rtt_buff->WrOff, buf, rem);
		knl_memcpy(rtt_buff->pBuffer, buf + rem, size - rem);
		rtt_buff->WrOff = size - rem;
	}
}


EXPORT	void	tm_rcv_dat( UB *buf, INT size )
{
	RTT_BUFFER	*rtt_buff;

	UW	rem;

	rtt_buff = &_SEGGER_RTT.aDown[0];

	while(rtt_buff->RdOff == rtt_buff->WrOff);

	if (rtt_buff->RdOff > rtt_buff->WrOff) {
		rem = rtt_buff->SizeOfBuffer - rtt_buff->RdOff;
		if(rem > size) rem = size;
		knl_memcpy(buf, rtt_buff->pBuffer + rtt_buff->RdOff, rem);
		buf += rem;
		size -= rem;
		rtt_buff->RdOff += rem;

		if(rtt_buff->RdOff == rtt_buff->SizeOfBuffer) rtt_buff->RdOff = 0;
	}

	rem = rtt_buff->WrOff - rtt_buff->RdOff;
	if(rem > size) rem = size;
	if(rem > 0) {
		knl_memcpy(buf, rtt_buff->pBuffer + rtt_buff->RdOff, rem);
		buf += rem;
		size -= rem;
		rtt_buff->RdOff += rem;
	}
}

EXPORT	void	tm_com_init(void)
{
	RTT_CB* p;

	// Initialize control block
	p = &_SEGGER_RTT;
	p->MaxNumUpBuffers	= 1;
	p->MaxNumDownBuffers	= 1;

	/* Initialize Up buffer	*/
	p->aUp[0].sName			= "Terminal";
	p->aUp[0].pBuffer		= _acUpBuffer;
	p->aUp[0].SizeOfBuffer		= sizeof(_acUpBuffer);
	p->aUp[0].RdOff			= 0;
	p->aUp[0].WrOff			= 0;
	p->aUp[0].Flags			= 0;

	/* Initialize Down buffer */
	p->aDown[0].sName		= "Terminal";
	p->aDown[0].pBuffer		= _acDownBuffer;
	p->aDown[0].SizeOfBuffer	= sizeof(_acDownBuffer);
	p->aDown[0].RdOff		= 0;
	p->aDown[0].WrOff		= 0;
	p->aDown[0].Flags		= 0;

	// Finish initialization of the control block.
	knl_strcpy(&p->acID[7], "RTT");
	knl_strcpy(&p->acID[0], "SEGGER");
	p->acID[6] = ' ';
}

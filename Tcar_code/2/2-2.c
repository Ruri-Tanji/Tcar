/* ウィンカーのLEDを点滅 */

/*
 *	usermain.c (usermain)
 *	User program main function
 */
#include <tm/tmonitor.h>

#define PE_DATA 0x400C0400
#define PE_CR   0x400C0404

EXPORT  INT  usermain( void )
{
    _UW   i;

    *(_UW*)(PE_CR) |= (1<<3);
    *(_UW*)(PE_CR) |= (1<<2);

    for(int i=0; i<3; i++){
      *(_UW*)(PE_DATA) |= (1<<3);
      tk_dly_tsk(500);
      *(_UW*)(PE_DATA) &= ~(1<<3);
      tk_dly_tsk(500);
      *(_UW*)(PE_DATA) |= (1<<2);
      tk_dly_tsk(500);
      *(_UW*)(PE_DATA) &= ~(1<<2);
      tk_dly_tsk(500);
    }
 
    return 0;
}

/* for文でbreak機能を使う */

/*
 *	usermain.c (usermain)
 *	User program main function
 */
#include <tm/tmonitor.h>

INT a = 0;

EXPORT  INT  usermain( void )
{
    INT i, j;

    for(i=0; i<10; i++){
      for(j=0; j<10; j++){
       a = i + j;
      }
     }
 
    return 0;
}

/* ステップ実行を試す */

/*
 *	usermain.c (usermain)
 *	User program main function
 */
#include <tm/tmonitor.h>

INT func(INT a)
{
  return a*2;
}

EXPORT  INT  usermain( void )
{
    INT i;

    i = 0;
    i = func(i);
    tm_printf("i = %d\n", i);
 
    return 0;
}

/*
 *	usermain.c (usermain)
 *	User program main function
 */
#include <tk/tkernel.h>
#include <tm/tmonitor.h>

EXPORT INT	usermain( void )
{
  tm_printf("hello,world\n");

  return 0;
}

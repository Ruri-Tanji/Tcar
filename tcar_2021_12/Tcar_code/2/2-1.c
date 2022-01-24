/* 初期タスクのID取得 */

/*
 *	usermain.c (usermain)
 *	User program main function
 */
#include <tk/tkernel.h>
#include <tm/tmonitor.h>

EXPORT INT	usermain( void )
{
  ID tskid;

  tskid = tk_get_tid();
  tm_printf("Task ID: %d\n", (INT)tskid);

  return 0;
}

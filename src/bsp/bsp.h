/*



*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define LOGD					printf

#define MSG(fmt, args...)     	LOGD("MSG "fmt"", ## args)
#define LOG(fmt, args...)     	LOGD("LOG "fmt"", ## args)
#define ERR(fmt, args...)     	LOGD("ERR "fmt"", ## args)

#define FALSE	0
#define	TRUE	1

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct BSP_S
{
   bool isInit;

   /* Periodic task related parameters */
   int prdTick;    // in msec

   /************************************/


}BSP_ST;

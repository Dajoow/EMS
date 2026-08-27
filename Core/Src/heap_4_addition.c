#include "heap_4_addition.h"
#include "FreeRTOS.h"
#include <string.h>
void *pvPortCalloc( size_t n, size_t size )
{
    void *pvReturn;

    pvReturn = pvPortMalloc(n * size);
    if(pvReturn)
      memset(pvReturn, 0, n * size);

    return pvReturn;
}

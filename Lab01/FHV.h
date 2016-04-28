/* FHV.h - Pilsan FHV General Header File */

#ifndef __INCFHVh
#define __INCFHVh

#include <taskLib.h>

#define aioBase 0x300

#define INUM_TO_IVEC(intNum) ((VOIDFUNCPTR *) ((intNum) << 3))

#define beLazy(milisecs) (taskDelay (sysClkRateGet()*milisecs/1000))

#endif /* __INCFHVh */

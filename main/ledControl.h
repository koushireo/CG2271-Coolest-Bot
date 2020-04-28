#include "MKL25Z4.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

extern osEventFlagsId_t greenEventFlag;
extern osSemaphoreId_t myConnectSem;
extern int redDelay; //500 when moving, 250 when staying
void tLedRed(void *argument);
void tLedGreenRunning(void *argument);
void tLedGreenConnect(void *argument);
void tLedGreenStop(void *argument);
void initLED(void);

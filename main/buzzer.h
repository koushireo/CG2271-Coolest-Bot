#include "MKL25Z4.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "string.h"

extern osSemaphoreId_t musicSem, musicSem2, musicSem3, musicSem4;
extern osEventFlagsId_t idleMusicFlag;
extern int MusicRepeat1;

void initBuzzer(void);
void tAudio1(void* argument);
void tAudio2(void* argument);
void tAudio3(void* argument);
void tAudio4(void* argument);
void tAudio5(void* argument);

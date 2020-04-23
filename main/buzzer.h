#include "MKL25Z4.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "string.h"

extern osSemaphoreId_t musicSem, musicSem2, musicSem3, musicSem4;
extern osEventFlagsId_t idleMusicFlag;
extern int MusicRepeat1;

void initBuzzer(void);
void cruelAngelThesis1Thread(void* argument);
void cruelAngelThesis2Thread(void* argument);
void cruelAngelThesis3Thread(void* argument);
void cruelAngelThesis4Thread(void* argument);
void cruelAngelThesis5Thread(void* argument);

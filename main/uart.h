#include "MKL25Z4.h"
#include "cmsis_os2.h"
extern osMessageQueueId_t UARTMsgQ;
void initUART2(uint32_t baud_rate);
void UART2_IRQHandler(void);

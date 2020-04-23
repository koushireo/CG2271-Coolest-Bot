#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"

#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

osMessageQueueId_t UARTMsgQ;

void initUART2(uint32_t baud_rate) {
    uint32_t divisor, bus_clock;
    
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    
    PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);
    
    PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
    
    UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
    
    bus_clock = (DEFAULT_SYSTEM_CLOCK)/2;
    divisor = bus_clock / (baud_rate * 16);
    UART2->BDH = UART_BDH_SBR(divisor >> 8);
    UART2->BDL = UART_BDL_SBR(divisor);
    
    UART2->C1 = 0;
    UART2->S2 = 0;
    UART2->C3 = 0;
    
    UART2->C2 |= ((UART_C2_RE_MASK) | (UART_C2_RIE_MASK));
		
    //enable Uart2 IRQ
    NVIC_SetPriority(UART2_IRQn, 3);
    NVIC_ClearPendingIRQ(UART2_IRQn);
    NVIC_EnableIRQ(UART2_IRQn);	
		
    UARTMsgQ = osMessageQueueNew(1, 1, NULL);
}

//Define UART2 interrupt handler
void UART2_IRQHandler() {
    NVIC_ClearPendingIRQ(UART2_IRQn);
    //clear flag
    int UARTdata;
    UARTdata = UART2->S1;
    UARTdata = UART2->D;
    osMessageQueuePut(UARTMsgQ, &UARTdata, NULL, 0);
    //do something
}

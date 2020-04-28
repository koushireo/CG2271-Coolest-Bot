#include "MKL25Z4.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define COL_1 5 // PortC Pin 5
#define COL_2 6 // PortC Pin 6
#define ROW_1 0 // PortC Pin 0
#define ROW_2 3 // PortC Pin 3
#define ROW_3 4 // PortC Pin 4
#define ROW_4 7 // PortC Pin 7
#define RED 1 // PortA Pin 13
#define SWITCH 6 // PortD Pin 6
#define MASK(x) (1 << (x))


osEventFlagsId_t greenEventFlag;
osSemaphoreId_t myConnectSem;

int greenDelay = 500; //500 when moving, 1 when staying
int redDelay = 250; //500 when moving, 250 when staying

void tLedGreenRunning(void *argument);
void tLedGreenStop(void *argument);

void initLED(void) {
	// Enable Clock to PORTC
	SIM->SCGC5 |= ((SIM_SCGC5_PORTC_MASK) | (SIM_SCGC5_PORTA_MASK));
	
	// Configure MUX settings to make all 7 pins GPIO
	PORTC->PCR[COL_1] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
	PORTC->PCR[COL_1] |= PORT_PCR_MUX(1);
	
	PORTC->PCR[COL_2] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
	PORTC->PCR[COL_2] |= PORT_PCR_MUX(1);
    
    PORTC->PCR[ROW_1] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
	PORTC->PCR[ROW_1] |= PORT_PCR_MUX(1);
    
    PORTC->PCR[ROW_2] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
	PORTC->PCR[ROW_2] |= PORT_PCR_MUX(1);
    
    PORTC->PCR[ROW_3] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
	PORTC->PCR[ROW_3] |= PORT_PCR_MUX(1);
    
    PORTC->PCR[ROW_4] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
	PORTC->PCR[ROW_4] |= PORT_PCR_MUX(1);
    
    PORTC->PCR[RED] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
	PORTC->PCR[RED] |= PORT_PCR_MUX(1);
	
	// Set Data Direction Registers for PortC
	PTC->PDDR |= (MASK(COL_1) | MASK(COL_2));
    PTC->PDDR |= (MASK(ROW_1) | MASK(ROW_2) | MASK(ROW_3) | MASK(ROW_4));
    PTC->PDDR |= (MASK(RED));
    
    greenEventFlag = osEventFlagsNew(NULL);
    myConnectSem = osSemaphoreNew(1,0,NULL);
}

void tLedGreenRunning(void *argument) {
    int vert = 0;
    int hor = 0;
    int hor_array[4] = {MASK(ROW_1),MASK(ROW_2), MASK(ROW_3), MASK(ROW_4)};
    int vert_array[2] = {MASK(COL_1), MASK(COL_2)};
    while(1) {
        osEventFlagsWait(greenEventFlag, 0x1, osFlagsWaitAny, osWaitForever);
        PTC->PDOR = 0;
        PTC->PDOR |= vert_array[vert];
        PTC->PDOR |= hor_array[hor];
        osDelay(greenDelay);
        hor += 1;
        if (hor == 4) {
            hor = 0;
            vert = 1 - vert;
        }
    }
}

void tLedGreenStop(void *argument) {
    int hor_array = {MASK(ROW_1) | MASK(ROW_2) | MASK(ROW_3) | MASK(ROW_4)};
    int vert_array = {MASK(COL_1) | MASK(COL_2)};  
    while (1) {
        osEventFlagsWait(greenEventFlag, 0x10, osFlagsWaitAny, osWaitForever);
        PTC->PCOR |= vert_array;
        PTC->PSOR |= hor_array;
    }
}

void tLedGreenConnect(void *argument) {
    int hor_array = {MASK(ROW_1) | MASK(ROW_2) | MASK(ROW_3) | MASK(ROW_4)};
    int vert_array = {MASK(COL_1) | MASK(COL_2)};
    osSemaphoreAcquire(myConnectSem, osWaitForever);
    PTC->PCOR |= vert_array;
    PTC->PSOR |= hor_array;
    osDelay(greenDelay/2);
    PTC->PSOR |= vert_array;
    PTC->PCOR |= hor_array;
    osDelay(greenDelay/2);
    PTC->PCOR |= vert_array;
    PTC->PSOR |= hor_array;
}

//Red LED
//All 8 flashes at 250ms intervals when stationary 
//All 8 flashes at 500ms intervals when moving

void tLedRed(void *argument) {
    while(1) {
        PTC->PSOR = MASK(RED);
        osDelay(redDelay);
        PTC->PCOR = MASK(RED);
        osDelay(redDelay);
    }
}

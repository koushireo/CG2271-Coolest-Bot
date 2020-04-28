#include "MKL25Z4.h"
#include "cmsis_os2.h"
#include "freqToMod.h"
#include "ledControl.h"

#define PTB0_Pin 0
#define PTB1_Pin 1
#define MASK(x) (1 << (x))
#define LEFT_POWER 0.5
#define RIGHT_POWER 0.5
#define LEFT_RATIO 0.01
#define RIGHT_RATIO 0.01
#define FORWARD_POWER 1
#define REVERSE_POWER 1
#define MOD_VAL 256

osSemaphoreId_t PWMsem;
osMessageQueueId_t tMotorMsgQ;

void initPWM(void) {
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
    
    PORTA->PCR[1] &= ~PORT_PCR_MUX_MASK; //timer 2
    PORTA->PCR[1] |= PORT_PCR_MUX(3);
    PORTA->PCR[2] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
    PORTA->PCR[2] |= PORT_PCR_MUX(1);
    PORTA->PCR[4] &= ~PORT_PCR_MUX_MASK; //timer 0
    PORTA->PCR[4] |= PORT_PCR_MUX(3);
    PORTA->PCR[5] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
    PORTA->PCR[5] |= PORT_PCR_MUX(1);

    PTA->PDDR |= (MASK(2) | MASK(5));
	
    SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM2_MASK;
    
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    
    TPM0->SC &= ~((TPM_SC_CMOD_MASK) |TPM_SC_PS_MASK);
    TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
    
    TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM0_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
    
    TPM2->SC &= ~((TPM_SC_CMOD_MASK) |TPM_SC_PS_MASK);
    TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM2->SC &= ~(TPM_SC_CPWMS_MASK);
		
    TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
		
    TPM0->MOD = MOD_VAL;
    TPM2->MOD = MOD_VAL;
		
    osSemaphoreId_t PWMsem = osSemaphoreNew(1,0,NULL);
    tMotorMsgQ = osMessageQueueNew(1, 1, NULL);
}

void pwm_stop(void) {
	TPM2_C0V = 0;
	TPM0_C1V = 0;
	PTA->PCOR |= (MASK(2) | MASK(5));
}

void pwm_forward(void) {
	TPM2_C0V = (int)(LEFT_POWER * FORWARD_POWER * MOD_VAL);
	TPM0_C1V = (int)(RIGHT_POWER * FORWARD_POWER * MOD_VAL);
	PTA->PSOR |= (MASK(2) | MASK(5));
}

void pwm_backward(void) {
	TPM2_C0V = 255 - (int)(LEFT_POWER * REVERSE_POWER * MOD_VAL);
	TPM0_C1V = 255 - (int)(RIGHT_POWER * REVERSE_POWER * MOD_VAL);
	PTA->PCOR |= (MASK(2) | MASK(5));
}

void pwm_left(void) {
	TPM2_C0V = 255 - (int)(LEFT_POWER * REVERSE_POWER * MOD_VAL * 0.5);
	TPM0_C1V = (int)(RIGHT_POWER * FORWARD_POWER * MOD_VAL * 0.5);
	PTA->PCOR |= MASK(2);
	PTA->PSOR |= MASK(5);
}

void pwm_right(void) {
	TPM2_C0V = (int)(LEFT_POWER * FORWARD_POWER * MOD_VAL * 0.5);
	TPM0_C1V = 255 - (int)(RIGHT_POWER * REVERSE_POWER * MOD_VAL * 0.5);
	PTA->PCOR |= MASK(5);
	PTA->PSOR |= MASK(2);
}

void pwm_forward_right(void) {
	TPM2_C0V = (int)(LEFT_POWER * FORWARD_POWER * LEFT_RATIO * MOD_VAL);
	TPM0_C1V = (int)(RIGHT_POWER * FORWARD_POWER * MOD_VAL);
	PTA->PSOR |= (MASK(2) | MASK(5));
}

void pwm_forward_left(void) {
	TPM2_C0V = (int)(LEFT_POWER * FORWARD_POWER * MOD_VAL);
	TPM0_C1V = (int)(RIGHT_POWER * FORWARD_POWER * RIGHT_RATIO * MOD_VAL);
	PTA->PSOR |= (MASK(2) | MASK(5));
}

void pwm_backward_right(void) {
	TPM2_C0V = 255 - (int)(LEFT_POWER * REVERSE_POWER * MOD_VAL * 0.2);
	TPM0_C1V = 255 - (int)(RIGHT_POWER * REVERSE_POWER * MOD_VAL * 1.7);
	PTA->PCOR |= (MASK(2) | MASK(5));
}

void pwm_backward_left(void) {
	TPM2_C0V = 255 - (int)(LEFT_POWER * REVERSE_POWER * MOD_VAL * 1.7);
	TPM0_C1V = 255 - (int)(RIGHT_POWER * REVERSE_POWER * MOD_VAL * 0.2);
	PTA->PCOR |= (MASK(2) | MASK(5));
}

void tMotorControl(void* argument) {
    int UARTdata;
    for(;;){
        osMessageQueueGet(tMotorMsgQ, &UARTdata, NULL, osWaitForever);
        if (UARTdata & MASK(2)) {
            if (UARTdata & MASK(4)) {
                pwm_forward_left();
            } else if (UARTdata & MASK(5)) {
                pwm_forward_right();
            } else {				
                pwm_forward();
            }
        } else if (UARTdata & MASK(3)) {
            if (UARTdata & MASK(4)) {
                pwm_backward_left();
            } else if (UARTdata & MASK(5)) {
                pwm_backward_right();
            } else {				
                pwm_backward();
            }
        } else if (UARTdata & MASK(4)) {
            pwm_left();
        } else if (UARTdata & MASK(5)) {
            pwm_right();
        } else {
            pwm_stop();
            osEventFlagsClear(greenEventFlag, 0x11);
            osEventFlagsSet(greenEventFlag, 0x10);
            redDelay = 250;
        }
    }
}

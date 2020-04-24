#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include "ledControl.h"
#include "uart.h"
#include "pwm.h"
#include "buzzer.h"

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define BAUD_RATE 9600
#define MASK(x) (1 << (x))

//Define thread to handle UART2 interrupts

void tBrain(void *argument) {
    int UARTdata;
    for (;;) {
        osMessageQueueGet(UARTMsgQ, &UARTdata, NULL, osWaitForever);  //wait for message from UART IRQ
        redDelay = 250;                                               //set initial red blinking speed to 250
        osEventFlagsSet(greenEventFlag, 0x10);                        //set all LED to non blink initially
        if ((UARTdata & 0b11) == 0b11) {                              //to get tMotorControl to move
            redDelay = 500;                                           //if tMotorControl moves, set red blinking speed to 500
            osEventFlagsClear(greenEventFlag, 0x11);
            osEventFlagsSet(greenEventFlag, 0x1);                     //enable green led blinking
            osMessageQueuePut(tMotorMsgQ, &UARTdata, NULL, 0);        //decode tMotorControl data
        } else if ((UARTdata & 0b11) == 0b00) {
            osSemaphoreRelease(musicSem1);                             //play music to indicate communication established
            osSemaphoreRelease(myConnectSem);
        } else if ((UARTdata & 0b11) == 0b01) {
            osEventFlagsClear(runningMusicFlag, 0x1);                    //disable idle music
            osSemaphoreRelease(musicSem4);                            //play ending music
        }
    }
}

int main (void) { 
    // System Initialization
    SystemCoreClockUpdate();
    osKernelInitialize();                 // Initialize CMSIS-RTOS
    initPWM();
    initUART2(BAUD_RATE);
    initBuzzer();
    initLED();
    osThreadNew(tBrain, NULL, NULL);
    osThreadNew(tMotorControl, NULL, NULL);
    osThreadNew(tAudio1, NULL, NULL);
    osThreadNew(tAudio2, NULL, NULL);
    osThreadNew(tAudio3, NULL, NULL);
    osThreadNew(tAudio4, NULL, NULL);
    osThreadNew(tAudio5, NULL, NULL);
    osThreadNew(tLedRed, NULL, NULL);
    osThreadNew(tLedGreenRunning, NULL, NULL);
    osThreadNew(tLedGreenConnect, NULL, NULL);
    osThreadNew(tLedGreenStop, NULL, NULL);
    osKernelStart();                      // Start thread execution
    for (;;) {
    }
}

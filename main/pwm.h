#include "MKL25Z4.h"
#include "cmsis_os2.h"

int freqToMod(int freq);
void initPWM(void);
void pwm_forward(void);
void pwm_backward(void);
void pwm_stop(void);
void pwm_left(void);
void pwm_right(void);
void pwm_forward_left(void);
void pwm_forward_right(void);
void pwm_backward_left(void);
void pwm_backward_right(void);
void tMotorControl(int UARTDATA);

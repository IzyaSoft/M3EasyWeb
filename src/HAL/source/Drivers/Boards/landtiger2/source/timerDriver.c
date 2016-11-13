#include "hal.h"
#include "LPC17xx.h"

unsigned short GetTimerCountValue(unsigned char timerNumber)
{
    if(timerNumber == 0)
        return LPC_TIM0->TC;
    else if(timerNumber == 1)
        return LPC_TIM1->TC;
    else if(timerNumber == 2)
        return LPC_TIM2->TC;
    return LPC_TIM3->TC;
}

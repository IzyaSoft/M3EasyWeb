#include "hal.h"
#include "LPC17xx.h"

void (*externalSysTickHandler)();

void SysTick_Handler()
{
	(*externalSysTickHandler)();
}

void ConfigureSystemClock()
{
    SystemInit();
}

void SetSysTickHandler(void (*sysTickHandler)(), uint32_t systemClockDivider)
{
    SysTick_Config(SystemCoreClock / systemClockDivider);
    externalSysTickHandler = sysTickHandler;
}

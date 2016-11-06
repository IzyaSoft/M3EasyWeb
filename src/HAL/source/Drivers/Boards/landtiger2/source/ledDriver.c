#include "hal.h"
#ifdef LANDTIGER2
#include "LPC17xx.h"

#ifdef USE_GPIO_LED_DRIVER
void ConfigureLedPort(unsigned char gpioPortNumber, uint32_t ledGpioDirectionMask, uint32_t initialValue)
{
    // We know that on landtiger2 LED ports is connected to GPIO2, so we are omitting gpioPortNumber
    LPC_GPIO2->FIODIR |= ledGpioDirectionMask;
    SetLedsValue(gpioPortNumber, initialValue);
}

void SetLedsValue(unsigned char gpioPortNumber, uint32_t ledsValue)
{
    // We know that on landtiger2 LED ports is connected to GPIO2, so we are omitting gpioPortNumber
    LPC_GPIO2->FIOPIN = ledsValue;
}
#endif

#endif

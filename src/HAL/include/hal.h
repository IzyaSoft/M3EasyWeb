#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include "driverConfig.h"

// LED
#ifdef USE_GPIO_LED_DRIVER
void ConfigureLedPort(unsigned char gpioPortNumber, uint32_t ledGpioDirectionMask, uint32_t initialValue);
void SetLedsValue(unsigned char gpioPortNumber, uint32_t ledsValue);
#endif

#endif

#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include "driverConfig.h"

// LED DRIVER API
#ifdef USE_GPIO_LED_DRIVER
void ConfigureLedPort(unsigned char gpioPortNumber, uint32_t ledGpioDirectionMask, uint32_t initialValue);
void SetLedsValue(unsigned char gpioPortNumber, uint32_t ledsValue);
#endif

// ADC DRIVER API
#ifdef USE_GPIO_ADC_DRIVER
void ConfigureAdc(unsigned char adcChannel, unsigned char clockDivider);
void DisableAdc(unsigned char adcChannel);
void StartAdc(unsigned char adcChanne);
void StopAdc(unsigned char adcChannel);
uint32_t GetAdcValue(unsigned char adcChannel);
#endif

#endif

#ifndef HAL_H
#define HAL_H

#include <stdint.h>
// defined drivers
#include "driverConfig.h"
// types
#include "ethernetConfiguration.h"
#include "ethernetBuffer.h"

// CLOCK API
void ConfigureSystemClock();
void SetSysTickHandler(void (*sysTickHandler)(), uint32_t systemClockDivider);

// TIMER API
unsigned short GetTimerCountValue(unsigned char timerNumber);

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

// ETHERNET DRIVER API
#ifdef USE_ETHERNET_DRIVER
    unsigned char InitializeEthrernet(struct EthernetConfiguration* configuration);
    void Read(struct EthernetBuffer* readBuffer);
    void Write(struct EthernetBuffer* bufferToWrite);
    uint32_t GetPhyStatus(uint32_t parameter);
    unsigned char CheckIsDataAvailable();
    uint32_t CheckAvailableDataSize();
#endif

#endif

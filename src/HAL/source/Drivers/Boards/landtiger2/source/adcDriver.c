#include "hal.h"
#ifdef LANDTIGER2
#include "LPC17xx.h"

#ifdef USE_GPIO_ADC_DRIVER

#define ADC_PINSEL_SELECTED 3
#define ADC_PINSEL_DESELECTED 0

void ConfigureAdc(unsigned char adcChannel, unsigned char clockDivider)
{
    LPC_SC->PCONP |= (1 << 12);
    AdjustAdcGpio(adcChannel, ADC_PINSEL_SELECTED);
    LPC_ADC->ADCR |= (1 << adcChannel) | (clockDivider << 8) | (1 << 21);
}

void DisableAdc(unsigned char adcChannel)
{
    LPC_ADC->ADCR &= (0 << adcChannel);
    AdjustAdcGpio(adcChannel, ADC_PINSEL_DESELECTED);
}

void StartAdc(unsigned char adcChanne)
{
    LPC_ADC->ADCR |= (1 << 24);
}

void StopAdc(unsigned char adcChannel)
{
    LPC_ADC->ADCR &= ~(7 << 24);
}

uint32_t GetAdcValue(unsigned char adcChannel)
{
	uint32_t value = 0;
    StartAdc(adcChannel);
    while (!(LPC_ADC->ADGDR & (1 << 31)));    // Wait for Conversion end
    value = ((LPC_ADC->ADGDR >> 4) & 0xFFF);  // read converted value
    StopAdc(adcChannel);
    return value;
}

void AdjustAdcGpio(unsigned char adcChannel, unsigned char value)
{
    switch(adcChannel)
    {
        case 0:
             LPC_PINCON->PINSEL1 |= (value << 14); // P0.23 is AD0.0
             break;
        case 1:
             LPC_PINCON->PINSEL1 |= (value << 16); // P0.24 is AD0.1
             break;
        case 2:
             LPC_PINCON->PINSEL1 |= (value << 18); // P0.24 is AD0.2
             break;
        case 3:
             LPC_PINCON->PINSEL1 |= (value << 20); // P0.24 is AD0.3
             break;
        case 4:
             LPC_PINCON->PINSEL3 |= (value << 28); // P1.30 is AD0.4
             break;
        case 5:
             LPC_PINCON->PINSEL3 |= (value << 30); // P1.31 is AD0.5
             break;
        case 6:
             LPC_PINCON->PINSEL0 |= (value << 6); // P0.3 is AD0.6
             break;
        case 7:
             LPC_PINCON->PINSEL0 |= (value << 4); // P0.2 is AD0.7
             break;
    }
}
#endif

#endif

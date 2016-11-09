#ifndef DEBUG_PRINT_FUNCTIONS_H
#define DEBUG_PRINT_FUNCTIONS_H

#include <stdint.h>

//void printStringHexSymbols(void* bufferPtr, uint32_t length, uint32_t lineLength = -1);

void printStringHexSymbols(void* bufferPtr, uint32_t length, uint32_t lineLength)
{
	unsigned char* symbolPtr = bufferPtr;
    for(uint32_t symbolCounter = 0; symbolCounter < length; symbolCounter++)
    {
        printf(" 0x%x ", *symbolPtr);
        if(lineLength > 0 && symbolCounter > 0)
        {
            if(lineLength % symbolCounter == 0)
            	printf("\n\r");
        }
        symbolPtr++;
    }
    printf("\r\n");
}

#endif

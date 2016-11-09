#include "networkUtils.h"

static void SwapByteOrderImpl(unsigned char* buffer, uint32_t length)
{
    unsigned char swap;
    for(uint32_t counter = 0; counter < length; counter += 2)
    {
        swap = buffer[counter];
        buffer[counter] = buffer[counter + 1];
        buffer[counter] = swap;
    }
}

void MakeNetworkBytesOrder(unsigned char* buffer, uint32_t length)
{
    SwapByteOrderImpl(buffer, length);
}

void MakeMcuBytesOrder(unsigned char* buffer, uint32_t length)
{
    SwapByteOrderImpl(buffer, length);
}

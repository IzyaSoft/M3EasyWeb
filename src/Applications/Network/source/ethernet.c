#include "ethernet.h"

void InsertEthernetHeader(struct EthernetBuffer* buffer, unsigned char* sourceMac, unsigned char* destinationMac, unsigned short etherType)
{
    memcpy(&buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], destinationMac, MAC_ADDRESS_LENGTH);
    memcpy(&buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], sourceMac, MAC_ADDRESS_LENGTH);
    *(unsigned short *)&buffer->_buffer[ETHERNET_ETHERTYPE_INDEX] = SWAPBYTES(etherType);
}

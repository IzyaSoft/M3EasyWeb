#include "ethernet.h"

void InsertEthernetHeader(struct EthernetBuffer* buffer, unsigned char* sourceMac, unsigned char* destinationMac, unsigned short etherType)
{
    memcpy(&buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], destinationMac, MAC_ADDRESS_LENGTH);
    memcpy(&buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], sourceMac, MAC_ADDRESS_LENGTH);
    SetWord(etherType, buffer, ETHERNET_ETHERTYPE_INDEX);
}

void InsertEthernetHeader2(struct EthernetBuffer* buffer, struct EthernetHeader* header)
{
    memcpy(&buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], header->_destinationMac, MAC_ADDRESS_LENGTH);
    memcpy(&buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], header->_sourceMac, MAC_ADDRESS_LENGTH);
    SetWord(header->_etherType, buffer, ETHERNET_ETHERTYPE_INDEX);
}

unsigned short GetEtherType(struct EthernetBuffer* buffer)
{
    return GetWord(buffer, ETHERNET_ETHERTYPE_INDEX);
}

struct EthernetHeader ReadEthernetHeader(struct EthernetBuffer* buffer)
{
    struct EthernetHeader ethernetHeader;
    memcpy(&ethernetHeader._destinationMac[0], &buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
    memcpy(&ethernetHeader._sourceMac[0], &buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
    ethernetHeader._etherType = GetEtherType(buffer);
    return ethernetHeader;
}

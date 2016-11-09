#ifndef HARDFAULT_DIAGNOSTIC_H
#define HARDFAULT_DIAGNOSTIC_H

#include <stdint.h>

#define HardFaultHandler HardFault_Handler
#define MemoryManageHandler MemManage_Handler

void CheckCFSRValue(uint32_t CFSRValue);
void CheckBusError(uint32_t CFSRValue, uint32_t regValue);
void CheckMemoryManagementError(uint32_t CFSRValue, uint32_t regValue);
void GetRegistersFromStack(uint32_t* pulFaultStackAddress, uint32_t faultSource);
void DisableWriteBufferization();

#endif

#include "diagnostic.h"
#include "LPC17xx.h"

static char message[256];

void CheckCFSRValue(uint32_t CFSRValue)
{
    printf("Usage fault: ");
    CFSRValue >>= 16;   // right shift to lsb

    if((CFSRValue & (1 << 9)) != 0)
        printf( "Divide by zero\n" );
    if((CFSRValue & (1 << 8)) != 0)
        printf( "Unaligned access\n" );
    if((CFSRValue & (1 << 3)) != 0)
        printf( "No coproccessor\n" );
    if((CFSRValue & ( 1 << 2 )) != 0)
        printf( "Invalid Program Counter Register Value\n" );
    if((CFSRValue & (1 << 1)) != 0)
        printf( "Invalid State\n" );
    if((CFSRValue & (1 << 0)) != 0)
        printf( "Undefined Instruction\n" );
}

void CheckMemoryManagementError(uint32_t CFSRValue, uint32_t regValue)
{
    if((CFSRValue & (1 << 7)) != 0)
    {
        sprintf(message, "Address register valid: 0x%08x\n", (unsigned int)regValue);
        printf(message);
    }

    if((CFSRValue & (1 << 5)) != 0)
        printf("Float Point Lazy State Preserve\n");
    if((CFSRValue & (1 << 4)) != 0)
        printf( "Stacking Fault\n" );
    if((CFSRValue & (1 << 3)) != 0)
        printf("Unstacking Fault\n");
    if((CFSRValue & (1 << 1)) != 0)
        printf("Data Access violation\n");
    if((CFSRValue & (1 << 0)) != 0)
        printf("Instruction Access violation\n");
}

void CheckBusError(uint32_t CFSRValue, uint32_t regValue)
{
    if((CFSRValue & (1 << 7)) != 0)
    {
        sprintf(message, "Address register valid: 0x%08x\n", (unsigned int)regValue);
        printf(message);
    }

    if((CFSRValue & (1 << 7)) == 0)
        printf("Invalid Bus Fault Address Register\n");
    if((CFSRValue & (1 << 4)) != 0)
        printf( "Stacking Fault\n" );
    if((CFSRValue & (1 << 3)) != 0)
        printf("Unstacking Fault\n");
    if((CFSRValue & (1 << 2)) != 0)
        printf("Imprecise bus error\n");
    if((CFSRValue & (1 << 1)) != 0)
        printf("Precise bus error\n");
    if((CFSRValue & (1 << 0)) != 0)
        printf("Instruction bus error\n");
}

void GetRegistersFromStack(uint32_t* pulFaultStackAddress, uint32_t faultSource)
{
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */
    volatile uint32_t r0 __attribute__((unused));
    volatile uint32_t r1 __attribute__((unused));
    volatile uint32_t r2 __attribute__((unused));
    volatile uint32_t r3 __attribute__((unused));
    volatile uint32_t r12 __attribute__((unused));
    volatile uint32_t lr __attribute__((unused)); /* Link register. */
    volatile uint32_t pc __attribute__((unused)); /* Program counter. */
    volatile uint32_t psr __attribute__((unused));/* Program status register. */
    volatile uint32_t psp __attribute__((unused));
    volatile uint32_t msp __attribute__((unused));
    volatile uint32_t _CFSR __attribute__((unused));
    volatile uint32_t _HFSR __attribute__((unused));
    volatile uint32_t _DFSR __attribute__((unused));
    volatile uint32_t _AFSR __attribute__((unused));
    volatile uint32_t _BFAR __attribute__((unused));
    volatile uint32_t _MMAR __attribute__((unused));
    volatile uint32_t _ACTLR __attribute__((unused));

    r0 = pulFaultStackAddress[0];
    r1 = pulFaultStackAddress[1];
    r2 = pulFaultStackAddress[2];
    r3 = pulFaultStackAddress[3];

    r12 = pulFaultStackAddress[4];
    lr = pulFaultStackAddress[5];
    pc = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];

    msp = __get_MSP();
    psp = __get_PSP();

    // Configurable Fault Status Register
    // Consists of MMSR, BFSR and UFSR
    _CFSR = (*((volatile unsigned long *)(0xE000ED28)));

    // Hard Fault Status Register
    _HFSR = (*((volatile unsigned long *)(0xE000ED2C)));

    // Debug Fault Status Register
    _DFSR = (*((volatile unsigned long *)(0xE000ED30)));

    // Auxiliary Fault Status Register
    _AFSR = (*((volatile unsigned long *)(0xE000ED3C)));

    // Read the Fault Address Registers. These may not contain valid values.
    // Check BFARVALID/MMARVALID to see if they are valid values
    // MemManage Fault Address Register
    _MMAR = (*((volatile unsigned long *)(0xE000ED34)));
    // Bus Fault Address Register
    _BFAR = (*((volatile unsigned long *)(0xE000ED38)));

    _ACTLR = (*((volatile unsigned long *)(0xE000E008)));

    if(faultSource == 1)
        printf("\n Hard Fault Detected \n");
    else if(faultSource == 2)
        printf("\n Memory Management Fault Detected\n");
    else
        printf("\n Unknown Fault Detected\n");

    sprintf(message, "MSP = 0x%08x\n", (unsigned int)msp); printf(message);
    sprintf(message, "PSP = 0x%08x\n", (unsigned int)psp); printf(message);

    sprintf(message, "SCB->CFSR = 0x%08x\n", (unsigned int)_CFSR);  printf(message);
    sprintf(message, "SCB->DFSR = 0x%08x\n", (unsigned int)_DFSR);  printf(message);
    sprintf(message, "SCB->AFSR = 0x%08x\n", (unsigned int)_AFSR);  printf(message);
    sprintf(message, "SCB->MMAR = 0x%08x\n", (unsigned int)_MMAR);  printf(message);
    sprintf(message, "SCB->BFAR = 0x%08x\n", (unsigned int)_BFAR);  printf(message);
    sprintf(message, "SCB->HFSR = 0x%08x\n", (unsigned int)_HFSR);  printf(message);
    sprintf(message, "SCB->ACTLR = 0x%08x\n", (unsigned int)_ACTLR);  printf(message);

    if ((SCB->HFSR & (1 << 30)) != 0)
        printf("Forced Hard Fault\n");

    if((_CFSR & 0xFFFF0000) != 0)
    {
        printf("Hard Fault:\n");
        CheckCFSRValue(_CFSR);
    }

    if((_CFSR & 0x0000FFFF) != 0)
    {
        if((_CFSR & 0x000000FF) != 0)
        {
            printf("Memory Management Fault:\n");
            CheckMemoryManagementError(_CFSR, _MMAR);
        }
        else
        {
            printf("Bus Fault:\n");
            CheckBusError(_CFSR >> 8, _BFAR);
        }
    }

    sprintf(message, "\nr0  = 0x%08x\n", (unsigned int)pulFaultStackAddress[0]);  printf(message);
    sprintf(message, "r1  = 0x%08x\n", (unsigned int)pulFaultStackAddress[1]);  printf(message);
    sprintf(message, "r2  = 0x%08x\n", (unsigned int)pulFaultStackAddress[2]);  printf(message);
    sprintf(message, "r3  = 0x%08x\n", (unsigned int)pulFaultStackAddress[3]);  printf(message);
    sprintf(message, "r12 = 0x%08x\n", (unsigned int)pulFaultStackAddress[4]);  printf(message);
    sprintf(message, "lr  = 0x%08x\n", (unsigned int)pulFaultStackAddress[5]);  printf(message);
    sprintf(message, "pc  = 0x%08x\n", (unsigned int)pulFaultStackAddress[6]);  printf(message);
    sprintf(message, "psr = 0x%08x\n", (unsigned int)pulFaultStackAddress[7]);  printf(message);

    __asm("BKPT #0\n") ; // Break into the debugger

    while (1) ;
}

// function to make precise Bus Fault when we have imprecise fault
void DisableWriteBufferization()
{
    volatile uint32_t* _ACTLR __attribute__((unused));
    _ACTLR = 0xE000E008;
    *_ACTLR = 2;
}


void HardFaultHandler() //__attribute__ (( naked ))
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " mov r1, 1     \n" /* Fault source */
        " ldr r3, handler1_address_const                            \n"
        " bx r3                                                     \n"
        " handler1_address_const: .word GetRegistersFromStack       \n"
    );

    while ( 1 )
    {
        ;
    }
}

void MemoryManageHandler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " mov r1, 2     \n" /* Fault source */
        " ldr r3, handler2_address_const                            \n"
        " bx r3                                                     \n"
        " handler2_address_const: .word GetRegistersFromStack       \n"
    );

    while ( 1 )
    {
       __asm("BKPT #0\n") ; // Break into the debugger
        ;
    }
}


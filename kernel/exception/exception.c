#include "lib/type.h"
#include "kernel/peripherals/uart.h"

#include "exception.h"
#include "irq.h"
#include "timer.h"

void exec_controller_el1 ( SYS_CALL x0 )
{
    uint64_t elr, esr;

    asm volatile ( 
        "mrs     %0, ELR_EL1;"
        "mrs     %1, ESR_EL1;"
        : "=r" ( elr ), "=r" ( esr )
        :
    );
    
    switch ( x0 ) 
    {
        case TEST_SVC:
            print_exec_info ( elr, esr );
            break;
        case IRQ_EL1_ENABLE:
            irq_el1_enable ();
            // since this instruction can only done in EL2, just request higher el.
            // asm volatile ( "hvc #1;" );
            break;
        case IRQ_EL1_DISABLE:
            irq_el1_disable ();
            // since this instruction can only done in EL2, just request higher el.
            // asm volatile ( "hvc #1;" );
            break;
        case CORE_TIMER_ENABLE:
            core_timer_enable ();
            break;
        case CORE_TIMER_DISABLE:
            core_timer_disable ();
            break;
        default:
            print_exec_info ( elr, esr );
            break;
    }
}

void exec_controller_el2 ( SYS_CALL x0 )
{
    uint64_t elr, esr;    

    asm volatile ( 
        "mrs     %0, ELR_EL2;"
        "mrs     %1, ESR_EL2;"
        : "=r" ( elr ), "=r" ( esr )
        :
    );
    
    switch ( x0 ) 
    {
        case TEST_SVC:
            print_exec_info ( elr, esr );
            break;
        case IRQ_EL1_ENABLE: 
            irq_el1_enable ();
            break;
        case IRQ_EL1_DISABLE: 
            irq_el1_disable ();
            break;
        default:
            print_exec_info ( elr, esr );
            break;
    }
}

void print_exec_info ( uint64_t elr, uint64_t esr )
{
    uint32_t ec, iss;

    ec = (((uint32_t)esr)>>26);
    // int il = (((uint32_t)esr)>>25) & 0x1 ;
    iss = (((uint32_t)esr)) & 0x00FFFFFF;

    uart_printf("Exception return address: %x\n", elr);
    uart_printf("Exception clss (EC): %x\n", ec);
    uart_printf("Instruction specific syndrome (ISS): %x\n", iss);
}

int get_current_el ()
{
    int el;
    asm volatile (
        "mrs    x0, CurrentEL;"
        "mov    %0, x0;" 
        : "=r" ( el )
    );

    el >>= 2;
    el &= 2;

    return el;
}
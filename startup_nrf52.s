.syntax unified
.cpu cortex-m4
.thumb

/* External declaration for the pointer to the stack top (defined by the linker) */
.global __StackTop
.global Reset_Handler

/* The vector table. Place it in a dedicated section so that the linker script can locate it. */
.section .isr_vector, "a", %progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word __StackTop       /* Initial stack pointer */
    .word Reset_Handler    /* Reset Handler */
    .word NMI_Handler      /* NMI Handler */
    .word HardFault_Handler/* Hard Fault Handler */
    /* Add other exception/interrupt vectors as needed */

    /* For unused interrupts, you can use a default handler. */

/* Reset Handler: sets up the runtime environment and calls main() */
Reset_Handler:
    /* Here you should initialize data and bss sections, set up clocks, etc.
       For simplicity, this example just calls main(). */
    bl main
    b .

/* Default interrupt handlers */
NMI_Handler:
    b .

HardFault_Handler:
    b .


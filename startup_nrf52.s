.syntax unified
.cpu cortex-m4              /* pinetime uses cortex-m4 */
.thumb

.global __StackTop
.global Reset_Handler

/* Vector table */
.section .isr_vector, "a", %progbits    /* Stores interrupt vector table */ 
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

/* Define global pointers to functions */
g_pfnVectors:
    .word __StackTop        /* Initial stack pointer */
    .word Reset_Handler     /* Reset Handler */

Reset_Handler:
    bl main                 /* For now, just jump to main */
    b .



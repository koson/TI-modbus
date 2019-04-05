//Dependencies
#include "isr_support.h"

   .global ext1IrqWrapper
   .extern extIntHandler
   .extern xISRStackTop

   .set nomips16
   .set noreorder
   .set noat

   .ent ext1IrqWrapper

//External 1 interrupt
ext1IrqWrapper:
   //Save the current task context
   portSAVE_CONTEXT
   //Call interrupt handler
   jal extIntHandler
   nop
   //Restore the context of the next task to execute
   portRESTORE_CONTEXT

   .end ext1IrqWrapper

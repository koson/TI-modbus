//Dependencies
#include "isr_support.h"

   .global ext0IrqWrapper
   .extern extIntHandler
   .extern xISRStackTop

   .set nomips16
   .set noreorder
   .set noat

   .ent ext0IrqWrapper

//External 0 interrupt
ext0IrqWrapper:
   //Save the current task context
   portSAVE_CONTEXT
   //Call interrupt handler
   jal extIntHandler
   nop
   //Restore the context of the next task to execute
   portRESTORE_CONTEXT

   .end ext0IrqWrapper

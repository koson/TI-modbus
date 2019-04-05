//Dependencies
#include "samg55.h"

//Linker file constants
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

//Function declaration
void SystemInit(void);
void __libc_init_array(void);
int main(void);

//Default empty handler
void Default_Handler(void);

//Cortex-M4 core handlers
void Reset_Handler      (void);
void NMI_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler (void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler    (void) __attribute__((weak, alias("Default_Handler")));

//Peripheral handlers
void SUPC_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void RSTC_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void RTC_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void RTT_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void WDT_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void PMC_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void EFC_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void FLEXCOM7_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void FLEXCOM0_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void FLEXCOM1_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void PIOA_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void PIOB_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void PDMIC0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void FLEXCOM2_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void MEM2MEM_Handler    (void) __attribute__((weak, alias("Default_Handler")));
void I2SC0_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void I2SC1_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void PDMIC1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void FLEXCOM3_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void FLEXCOM4_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void FLEXCOM5_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void FLEXCOM6_Handler   (void) __attribute__((weak, alias("Default_Handler")));
void TC0_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC1_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC2_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC3_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC4_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC5_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void ADC_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void ARM_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void UHP_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void UDP_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void CRCCU_Handler      (void) __attribute__((weak, alias("Default_Handler")));

//Vector table
__attribute__((section(".vectors")))
const uint32_t vectorTable[66] =
{
   //Initial stack pointer
   (uint32_t) (&_estack),

   //Core handlers
   (uint32_t) Reset_Handler,
   (uint32_t) NMI_Handler,
   (uint32_t) HardFault_Handler,
   (uint32_t) MemManage_Handler,
   (uint32_t) BusFault_Handler,
   (uint32_t) UsageFault_Handler,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) SVC_Handler,
   (uint32_t) DebugMon_Handler,
   (uint32_t) 0,
   (uint32_t) PendSV_Handler,
   (uint32_t) SysTick_Handler,

   //Peripheral handlers
   (uint32_t) SUPC_Handler,     //0  Supply Controller
   (uint32_t) RSTC_Handler,     //1  Reset Controller
   (uint32_t) RTC_Handler,      //2  Real Time Clock
   (uint32_t) RTT_Handler,      //3  Real Time Timer
   (uint32_t) WDT_Handler,      //4  Watchdog Timer
   (uint32_t) PMC_Handler,      //5  Power Management Controller
   (uint32_t) EFC_Handler,      //6  Enhanced Flash Controller
   (uint32_t) FLEXCOM7_Handler, //7  FLEXCOM 7
   (uint32_t) FLEXCOM0_Handler, //8  FLEXCOM 0
   (uint32_t) FLEXCOM1_Handler, //9  FLEXCOM 1
   (uint32_t) Default_Handler,  //10 Reserved
   (uint32_t) PIOA_Handler,     //11 Parallel I/O Controller A
   (uint32_t) PIOB_Handler,     //12 Parallel I/O Controller B
   (uint32_t) PDMIC0_Handler,   //13 PDM 0
   (uint32_t) FLEXCOM2_Handler, //14 FLEXCOM2
   (uint32_t) MEM2MEM_Handler,  //15 MEM2MEM
   (uint32_t) I2SC0_Handler,    //16 I2SC0
   (uint32_t) I2SC1_Handler,    //17 I2SC1
   (uint32_t) PDMIC1_Handler,   //18 PDM 1
   (uint32_t) FLEXCOM3_Handler, //19 FLEXCOM3
   (uint32_t) FLEXCOM4_Handler, //20 FLEXCOM4
   (uint32_t) FLEXCOM5_Handler, //21 FLEXCOM5
   (uint32_t) FLEXCOM6_Handler, //22 FLEXCOM6
   (uint32_t) TC0_Handler,      //23 Timer/Counter 0
   (uint32_t) TC1_Handler,      //24 Timer/Counter 1
   (uint32_t) TC2_Handler,      //25 Timer/Counter 2
   (uint32_t) TC3_Handler,      //26 Timer/Counter 3
   (uint32_t) TC4_Handler,      //27 Timer/Counter 4
   (uint32_t) TC5_Handler,      //28 Timer/Counter 5
   (uint32_t) ADC_Handler,      //29 Analog To Digital Converter
   (uint32_t) ARM_Handler,      //30 FPU
   (uint32_t) Default_Handler,  //31 Reserved
   (uint32_t) Default_Handler,  //32 Reserved
   (uint32_t) Default_Handler,  //33 Reserved
   (uint32_t) Default_Handler,  //34 Reserved
   (uint32_t) Default_Handler,  //35 Reserved
   (uint32_t) Default_Handler,  //36 Reserved
   (uint32_t) Default_Handler,  //37 Reserved
   (uint32_t) Default_Handler,  //38 Reserved
   (uint32_t) Default_Handler,  //39 Reserved
   (uint32_t) Default_Handler,  //40 Reserved
   (uint32_t) Default_Handler,  //41 Reserved
   (uint32_t) Default_Handler,  //42 Reserved
   (uint32_t) Default_Handler,  //43 Reserved
   (uint32_t) Default_Handler,  //44 Reserved
   (uint32_t) Default_Handler,  //45 Reserved
   (uint32_t) Default_Handler,  //46 Reserved
   (uint32_t) UHP_Handler,      //47 USB OHCI
   (uint32_t) UDP_Handler,      //48 USB Device FS
   (uint32_t) CRCCU_Handler     //49 CRCCU
};


/**
 * @brief Reset handler
 **/

void Reset_Handler(void)
{
   uint32_t *src;
   uint32_t *dest;

   //System initialization
   SystemInit();

   //Initialize the relocate segment
   src = &_etext;
   dest = &_srelocate;

   if(src != dest)
   {
      while(dest < &_erelocate)
	  {
         *dest++ = *src++;
      }
   }

   //Clear the zero segment
   for(dest = &_szero; dest < &_ezero;)
   {
      *dest++ = 0;
   }

   //Set the vector table base address
   src = (uint32_t *) & _sfixed;
   SCB->VTOR = ((uint32_t) src & SCB_VTOR_TBLOFF_Msk);

   //C library initialization
   __libc_init_array();

   //Branch to main function
   main();

   //Endless loop
   while(1);
}


/**
 * @brief Default interrupt handler
 **/

void Default_Handler(void)
{
   while(1)
   {
   }
}

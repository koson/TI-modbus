//Dependencies
#include "stm32l496xx.h"

//Linker file constants
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

//Function declaration
void SystemInit(void);
void __libc_init_array(void);
int main(void);

//Default empty handler
void Default_Handler(void);

//Cortex-M7 core handlers
void Reset_Handler                 (void);
void NMI_Handler                   (void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler             (void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler             (void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler              (void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler            (void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler                   (void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler              (void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler                (void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler               (void) __attribute__((weak, alias("Default_Handler")));

//Peripheral handlers
void WWDG_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void PVD_PVM_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler         (void) __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_TX_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX0_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM15_IRQHandler     (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT3_IRQHandler        (void) __attribute__((weak, alias("Default_Handler")));
void TIM8_BRK_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void TIM8_UP_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void TIM8_TRG_COM_IRQHandler       (void) __attribute__((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void ADC3_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void FMC_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void SDMMC1_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void UART4_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void UART5_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel1_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel2_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel3_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel4_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel5_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT0_IRQHandler        (void) __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT1_IRQHandler        (void) __attribute__((weak, alias("Default_Handler")));
void DFSDM1_FLT2_IRQHandler        (void) __attribute__((weak, alias("Default_Handler")));
void COMP_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void LPTIM1_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void LPTIM2_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel6_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel7_IRQHandler      (void) __attribute__((weak, alias("Default_Handler")));
void LPUART1_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void QUADSPI_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void SAI1_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void SAI2_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void SWPMI1_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void TSC_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void LCD_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void RNG_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void CRS_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void I2C4_EV_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void I2C4_ER_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void DCMI_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void CAN2_TX_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void CAN2_RX0_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void CAN2_RX1_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void CAN2_SCE_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void DMA2D_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));

//Vector table
__attribute__((section(".isr_vector")))
const uint32_t vectorTable[107] =
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
   (uint32_t) WWDG_IRQHandler,               //Window WatchDog
   (uint32_t) PVD_PVM_IRQHandler,            //PVD/PVM1/PVM2/PVM3/PVM4 through EXTI Line detection
   (uint32_t) TAMP_STAMP_IRQHandler,         //Tamper and TimeStamps through the EXTI line
   (uint32_t) RTC_WKUP_IRQHandler,           //RTC Wakeup through the EXTI line
   (uint32_t) FLASH_IRQHandler,              //FLASH
   (uint32_t) RCC_IRQHandler,                //RCC
   (uint32_t) EXTI0_IRQHandler,              //EXTI Line0
   (uint32_t) EXTI1_IRQHandler,              //EXTI Line1
   (uint32_t) EXTI2_IRQHandler,              //EXTI Line2
   (uint32_t) EXTI3_IRQHandler,              //EXTI Line3
   (uint32_t) EXTI4_IRQHandler,              //EXTI Line4
   (uint32_t) DMA1_Channel1_IRQHandler,      //DMA1 Channel 1
   (uint32_t) DMA1_Channel2_IRQHandler,      //DMA1 Channel 2
   (uint32_t) DMA1_Channel3_IRQHandler,      //DMA1 Channel 3
   (uint32_t) DMA1_Channel4_IRQHandler,      //DMA1 Channel 4
   (uint32_t) DMA1_Channel5_IRQHandler,      //DMA1 Channel 5
   (uint32_t) DMA1_Channel6_IRQHandler,      //DMA1 Channel 6
   (uint32_t) DMA1_Channel7_IRQHandler,      //DMA1 Channel 7
   (uint32_t) ADC1_2_IRQHandler,             //ADC1, ADC2
   (uint32_t) CAN1_TX_IRQHandler,            //CAN1 TX
   (uint32_t) CAN1_RX0_IRQHandler,           //CAN1 RX0
   (uint32_t) CAN1_RX1_IRQHandler,           //CAN1 RX1
   (uint32_t) CAN1_SCE_IRQHandler,           //CAN1 SCE
   (uint32_t) EXTI9_5_IRQHandler,            //External Line[9:5]s
   (uint32_t) TIM1_BRK_TIM15_IRQHandler,     //TIM1 Break and TIM15
   (uint32_t) TIM1_UP_TIM16_IRQHandler,      //TIM1 Update and TIM16
   (uint32_t) TIM1_TRG_COM_TIM17_IRQHandler, //TIM1 Trigger and Commutation and TIM17
   (uint32_t) TIM1_CC_IRQHandler,            //TIM1 Capture Compare
   (uint32_t) TIM2_IRQHandler,               //TIM2
   (uint32_t) TIM3_IRQHandler,               //TIM3
   (uint32_t) TIM4_IRQHandler,               //TIM4
   (uint32_t) I2C1_EV_IRQHandler,            //I2C1 Event
   (uint32_t) I2C1_ER_IRQHandler,            //I2C1 Error
   (uint32_t) I2C2_EV_IRQHandler,            //I2C2 Event
   (uint32_t) I2C2_ER_IRQHandler,            //I2C2 Error
   (uint32_t) SPI1_IRQHandler,               //SPI1
   (uint32_t) SPI2_IRQHandler,               //SPI2
   (uint32_t) USART1_IRQHandler,             //USART1
   (uint32_t) USART2_IRQHandler,             //USART2
   (uint32_t) USART3_IRQHandler,             //USART3
   (uint32_t) EXTI15_10_IRQHandler,          //External Line[15:10]
   (uint32_t) RTC_Alarm_IRQHandler,          //RTC Alarm (A and B) through EXTI Line
   (uint32_t) DFSDM1_FLT3_IRQHandler,        //DFSDM1 Filter 3 global Interrupt
   (uint32_t) TIM8_BRK_IRQHandler,           //TIM8 Break Interrupt
   (uint32_t) TIM8_UP_IRQHandler,            //TIM8 Update Interrupt
   (uint32_t) TIM8_TRG_COM_IRQHandler,       //TIM8 Trigger and Commutation Interrupt
   (uint32_t) TIM8_CC_IRQHandler,            //TIM8 Capture Compare Interrupt
   (uint32_t) ADC3_IRQHandler,               //ADC3 global  Interrupt
   (uint32_t) FMC_IRQHandler,                //FMC
   (uint32_t) SDMMC1_IRQHandler,             //SDMMC1
   (uint32_t) TIM5_IRQHandler,               //TIM5
   (uint32_t) SPI3_IRQHandler,               //SPI3
   (uint32_t) UART4_IRQHandler,              //UART4
   (uint32_t) UART5_IRQHandler,              //UART5
   (uint32_t) TIM6_DAC_IRQHandler,           //TIM6 and DAC1&2 underrun errors
   (uint32_t) TIM7_IRQHandler,               //TIM7
   (uint32_t) DMA2_Channel1_IRQHandler,      //DMA2 Channel 1
   (uint32_t) DMA2_Channel2_IRQHandler,      //DMA2 Channel 2
   (uint32_t) DMA2_Channel3_IRQHandler,      //DMA2 Channel 3
   (uint32_t) DMA2_Channel4_IRQHandler,      //DMA2 Channel 4
   (uint32_t) DMA2_Channel5_IRQHandler,      //DMA2 Channel 5
   (uint32_t) DFSDM1_FLT0_IRQHandler,        //DFSDM1 Filter 0 global Interrupt
   (uint32_t) DFSDM1_FLT1_IRQHandler,        //DFSDM1 Filter 1 global Interrupt
   (uint32_t) DFSDM1_FLT2_IRQHandler,        //DFSDM1 Filter 2 global Interrupt
   (uint32_t) COMP_IRQHandler,               //COMP Interrupt
   (uint32_t) LPTIM1_IRQHandler,             //LP TIM1 interrupt
   (uint32_t) LPTIM2_IRQHandler,             //LP TIM2 interrupt
   (uint32_t) OTG_FS_IRQHandler,             //USB OTG FS
   (uint32_t) DMA2_Channel6_IRQHandler,      //DMA2 Channel 6
   (uint32_t) DMA2_Channel7_IRQHandler,      //DMA2 Channel 7
   (uint32_t) LPUART1_IRQHandler,            //LP UART1 interrupt
   (uint32_t) QUADSPI_IRQHandler,            //Quad SPI global interrupt
   (uint32_t) I2C3_EV_IRQHandler,            //I2C3 event
   (uint32_t) I2C3_ER_IRQHandler,            //I2C3 error
   (uint32_t) SAI1_IRQHandler,               //Serial Audio Interface 1 global interrupt
   (uint32_t) SAI2_IRQHandler,               //Serial Audio Interface 2 global interrupt
   (uint32_t) SWPMI1_IRQHandler,             //Serial Wire Interface 1 global interrupt
   (uint32_t) TSC_IRQHandler,                //Touch Sense Controller global interrupt
   (uint32_t) LCD_IRQHandler,                //LCD global interrupt
   (uint32_t) 0,                             //Reserved
   (uint32_t) RNG_IRQHandler,                //RNG global interrupt
   (uint32_t) FPU_IRQHandler,                //FPU
   (uint32_t) CRS_IRQHandler,                //CRS error
   (uint32_t) I2C4_EV_IRQHandler,            //I2C4 event
   (uint32_t) I2C4_ER_IRQHandler,            //I2C4 error
   (uint32_t) DCMI_IRQHandler,               //DCMI global interrupt
   (uint32_t) CAN2_TX_IRQHandler,            //CAN2 TX
   (uint32_t) CAN2_RX0_IRQHandler,           //CAN2 RX0
   (uint32_t) CAN2_RX1_IRQHandler,           //CAN2 RX1
   (uint32_t) CAN2_SCE_IRQHandler,           //CAN2 SCE
   (uint32_t) DMA2D_IRQHandler,              //DMA2D global interrupt
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
   src = &_sidata;
   dest = &_sdata;

   if(src != dest)
   {
      while(dest < &_edata)
      {
         *dest++ = *src++;
      }
   }

   //Clear the zero segment
   for(dest = &_sbss; dest < &_ebss;)
   {
      *dest++ = 0;
   }

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

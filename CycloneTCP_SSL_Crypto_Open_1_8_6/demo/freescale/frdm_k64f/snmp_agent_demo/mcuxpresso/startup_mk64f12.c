//Dependencies
#include "mk64f12.h"

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
void Reset_Handler                       (void);
void NMI_Handler                         (void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler                   (void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler                   (void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler                    (void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler                  (void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler                         (void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler                    (void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler                      (void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler                     (void) __attribute__((weak, alias("Default_Handler")));

//Peripheral handlers
void DMA0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA1_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA2_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA3_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA4_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA5_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA6_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA7_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA8_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA9_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DMA10_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void DMA11_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void DMA12_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void DMA13_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void DMA14_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void DMA15_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void DMA_Error_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void MCM_IRQHandler                      (void) __attribute__((weak, alias("Default_Handler")));
void FTFE_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void Read_Collision_IRQHandler           (void) __attribute__((weak, alias("Default_Handler")));
void LVD_LVW_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void LLWU_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void WDOG_EWM_IRQHandler                 (void) __attribute__((weak, alias("Default_Handler")));
void RNG_IRQHandler                      (void) __attribute__((weak, alias("Default_Handler")));
void I2C0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void I2C1_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void SPI0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void I2S0_Tx_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void I2S0_Rx_IRQHandler                  (void) __attribute__((weak, alias("Default_Handler")));
void UART0_LON_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void UART0_RX_TX_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void UART0_ERR_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void UART1_RX_TX_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void UART1_ERR_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void UART2_RX_TX_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void UART2_ERR_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void UART3_RX_TX_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void UART3_ERR_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void ADC0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void CMP0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void CMP1_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void FTM0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void FTM1_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void FTM2_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void CMT_IRQHandler                      (void) __attribute__((weak, alias("Default_Handler")));
void RTC_IRQHandler                      (void) __attribute__((weak, alias("Default_Handler")));
void RTC_Seconds_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void PIT0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void PIT1_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void PIT2_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void PIT3_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void PDB0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void USB0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void USBDCD_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void Reserved71_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void DAC0_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void MCG_IRQHandler                      (void) __attribute__((weak, alias("Default_Handler")));
void LPTMR0_IRQHandler                   (void) __attribute__((weak, alias("Default_Handler")));
void PORTA_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void PORTB_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void PORTC_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void PORTD_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void PORTE_IRQHandler                    (void) __attribute__((weak, alias("Default_Handler")));
void SWI_IRQHandler                      (void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void UART4_RX_TX_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void UART4_ERR_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void UART5_RX_TX_IRQHandler              (void) __attribute__((weak, alias("Default_Handler")));
void UART5_ERR_IRQHandler                (void) __attribute__((weak, alias("Default_Handler")));
void CMP2_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void FTM3_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void DAC1_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void ADC1_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void I2C2_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void CAN0_ORed_Message_buffer_IRQHandler (void) __attribute__((weak, alias("Default_Handler")));
void CAN0_Bus_Off_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void CAN0_Error_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));
void CAN0_Tx_Warning_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void CAN0_Rx_Warning_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void CAN0_Wake_Up_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void SDHC_IRQHandler                     (void) __attribute__((weak, alias("Default_Handler")));
void ENET_1588_Timer_IRQHandler          (void) __attribute__((weak, alias("Default_Handler")));
void ENET_Transmit_IRQHandler            (void) __attribute__((weak, alias("Default_Handler")));
void ENET_Receive_IRQHandler             (void) __attribute__((weak, alias("Default_Handler")));
void ENET_Error_IRQHandler               (void) __attribute__((weak, alias("Default_Handler")));

//Vector table
__attribute__((section(".vectors")))
const uint32_t vectorTable[260] =
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
   (uint32_t) DMA0_IRQHandler,                     //DMA Channel 0 Transfer Complete
   (uint32_t) DMA1_IRQHandler,                     //DMA Channel 1 Transfer Complete
   (uint32_t) DMA2_IRQHandler,                     //DMA Channel 2 Transfer Complete
   (uint32_t) DMA3_IRQHandler,                     //DMA Channel 3 Transfer Complete
   (uint32_t) DMA4_IRQHandler,                     //DMA Channel 4 Transfer Complete
   (uint32_t) DMA5_IRQHandler,                     //DMA Channel 5 Transfer Complete
   (uint32_t) DMA6_IRQHandler,                     //DMA Channel 6 Transfer Complete
   (uint32_t) DMA7_IRQHandler,                     //DMA Channel 7 Transfer Complete
   (uint32_t) DMA8_IRQHandler,                     //DMA Channel 8 Transfer Complete
   (uint32_t) DMA9_IRQHandler,                     //DMA Channel 9 Transfer Complete
   (uint32_t) DMA10_IRQHandler,                    //DMA Channel 10 Transfer Complete
   (uint32_t) DMA11_IRQHandler,                    //DMA Channel 11 Transfer Complete
   (uint32_t) DMA12_IRQHandler,                    //DMA Channel 12 Transfer Complete
   (uint32_t) DMA13_IRQHandler,                    //DMA Channel 13 Transfer Complete
   (uint32_t) DMA14_IRQHandler,                    //DMA Channel 14 Transfer Complete
   (uint32_t) DMA15_IRQHandler,                    //DMA Channel 15 Transfer Complete
   (uint32_t) DMA_Error_IRQHandler,                //DMA Error Interrupt
   (uint32_t) MCM_IRQHandler,                      //Normal Interrupt
   (uint32_t) FTFE_IRQHandler,                     //FTFE Command complete interrupt
   (uint32_t) Read_Collision_IRQHandler,           //Read Collision Interrupt
   (uint32_t) LVD_LVW_IRQHandler,                  //Low Voltage Detect, Low Voltage Warning
   (uint32_t) LLWU_IRQHandler,                     //Low Leakage Wakeup Unit
   (uint32_t) WDOG_EWM_IRQHandler,                 //WDOG Interrupt
   (uint32_t) RNG_IRQHandler,                      //RNG Interrupt
   (uint32_t) I2C0_IRQHandler,                     //I2C0 interrupt
   (uint32_t) I2C1_IRQHandler,                     //I2C1 interrupt
   (uint32_t) SPI0_IRQHandler,                     //SPI0 Interrupt
   (uint32_t) SPI1_IRQHandler,                     //SPI1 Interrupt
   (uint32_t) I2S0_Tx_IRQHandler,                  //I2S0 transmit interrupt
   (uint32_t) I2S0_Rx_IRQHandler,                  //I2S0 receive interrupt
   (uint32_t) UART0_LON_IRQHandler,                //UART0 LON interrupt
   (uint32_t) UART0_RX_TX_IRQHandler,              //UART0 Receive/Transmit interrupt
   (uint32_t) UART0_ERR_IRQHandler,                //UART0 Error interrupt
   (uint32_t) UART1_RX_TX_IRQHandler,              //UART1 Receive/Transmit interrupt
   (uint32_t) UART1_ERR_IRQHandler,                //UART1 Error interrupt
   (uint32_t) UART2_RX_TX_IRQHandler,              //UART2 Receive/Transmit interrupt
   (uint32_t) UART2_ERR_IRQHandler,                //UART2 Error interrupt
   (uint32_t) UART3_RX_TX_IRQHandler,              //UART3 Receive/Transmit interrupt
   (uint32_t) UART3_ERR_IRQHandler,                //UART3 Error interrupt
   (uint32_t) ADC0_IRQHandler,                     //ADC0 interrupt
   (uint32_t) CMP0_IRQHandler,                     //CMP0 interrupt
   (uint32_t) CMP1_IRQHandler,                     //CMP1 interrupt
   (uint32_t) FTM0_IRQHandler,                     //FTM0 fault, overflow and channels interrupt
   (uint32_t) FTM1_IRQHandler,                     //FTM1 fault, overflow and channels interrupt
   (uint32_t) FTM2_IRQHandler,                     //FTM2 fault, overflow and channels interrupt
   (uint32_t) CMT_IRQHandler,                      //CMT interrupt
   (uint32_t) RTC_IRQHandler,                      //RTC interrupt
   (uint32_t) RTC_Seconds_IRQHandler,              //RTC seconds interrupt
   (uint32_t) PIT0_IRQHandler,                     //PIT timer channel 0 interrupt
   (uint32_t) PIT1_IRQHandler,                     //PIT timer channel 1 interrupt
   (uint32_t) PIT2_IRQHandler,                     //PIT timer channel 2 interrupt
   (uint32_t) PIT3_IRQHandler,                     //PIT timer channel 3 interrupt
   (uint32_t) PDB0_IRQHandler,                     //PDB0 Interrupt
   (uint32_t) USB0_IRQHandler,                     //USB0 interrupt
   (uint32_t) USBDCD_IRQHandler,                   //USBDCD Interrupt
   (uint32_t) Reserved71_IRQHandler,               //Reserved interrupt
   (uint32_t) DAC0_IRQHandler,                     //DAC0 interrupt
   (uint32_t) MCG_IRQHandler,                      //MCG Interrupt
   (uint32_t) LPTMR0_IRQHandler,                   //LPTimer interrupt
   (uint32_t) PORTA_IRQHandler,                    //Port A interrupt
   (uint32_t) PORTB_IRQHandler,                    //Port B interrupt
   (uint32_t) PORTC_IRQHandler,                    //Port C interrupt
   (uint32_t) PORTD_IRQHandler,                    //Port D interrupt
   (uint32_t) PORTE_IRQHandler,                    //Port E interrupt
   (uint32_t) SWI_IRQHandler,                      //Software interrupt
   (uint32_t) SPI2_IRQHandler,                     //SPI2 Interrupt
   (uint32_t) UART4_RX_TX_IRQHandler,              //UART4 Receive/Transmit interrupt
   (uint32_t) UART4_ERR_IRQHandler,                //UART4 Error interrupt
   (uint32_t) UART5_RX_TX_IRQHandler,              //UART5 Receive/Transmit interrupt
   (uint32_t) UART5_ERR_IRQHandler,                //UART5 Error interrupt
   (uint32_t) CMP2_IRQHandler,                     //CMP2 interrupt
   (uint32_t) FTM3_IRQHandler,                     //FTM3 fault, overflow and channels interrupt
   (uint32_t) DAC1_IRQHandler,                     //DAC1 interrupt
   (uint32_t) ADC1_IRQHandler,                     //ADC1 interrupt
   (uint32_t) I2C2_IRQHandler,                     //I2C2 interrupt
   (uint32_t) CAN0_ORed_Message_buffer_IRQHandler, //CAN0 OR'd message buffers interrupt
   (uint32_t) CAN0_Bus_Off_IRQHandler,             //CAN0 bus off interrupt
   (uint32_t) CAN0_Error_IRQHandler,               //CAN0 error interrupt
   (uint32_t) CAN0_Tx_Warning_IRQHandler,          //CAN0 Tx warning interrupt
   (uint32_t) CAN0_Rx_Warning_IRQHandler,          //CAN0 Rx warning interrupt
   (uint32_t) CAN0_Wake_Up_IRQHandler,             //CAN0 wake up interrupt
   (uint32_t) SDHC_IRQHandler,                     //SDHC interrupt
   (uint32_t) ENET_1588_Timer_IRQHandler,          //Ethernet MAC IEEE 1588 Timer Interrupt
   (uint32_t) ENET_Transmit_IRQHandler,            //Ethernet MAC Transmit Interrupt
   (uint32_t) ENET_Receive_IRQHandler,             //Ethernet MAC Receive Interrupt
   (uint32_t) ENET_Error_IRQHandler,               //Ethernet MAC Error and miscelaneous Interrupt
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0xFFFFFFFF,
   (uint32_t) 0xFFFFFFFF,
   (uint32_t) 0xFFFFFFFF,
   (uint32_t) 0xFFFFFFFE
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
   src = (uint32_t *) &_sfixed;
   SCB->VTOR = ((uint32_t) src & SCB_VTOR_TBLOFF_Msk);

   //C library initialization
#if defined (__cplusplus)
   __libc_init_array();
#endif

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

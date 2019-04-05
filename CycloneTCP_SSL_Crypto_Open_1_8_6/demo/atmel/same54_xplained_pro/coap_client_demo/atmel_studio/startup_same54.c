//Dependencies
#include "same54.h"

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

//Cortex-M3 core handlers
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
void PM_Handler         (void) __attribute__((weak, alias("Default_Handler")));
void MCLK_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void OSCCTRL_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void OSCCTRL_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void OSCCTRL_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void OSCCTRL_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void OSCCTRL_4_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void OSC32KCTRL_Handler (void) __attribute__((weak, alias("Default_Handler")));
void SUPC_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void SUPC_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void WDT_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void RTC_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void EIC_0_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_1_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_2_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_3_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_4_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_5_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_6_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_7_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_8_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_9_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void EIC_10_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void EIC_11_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void EIC_12_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void EIC_13_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void EIC_14_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void EIC_15_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void FREQM_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void NVMCTRL_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void NVMCTRL_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void DMAC_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void DMAC_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void DMAC_2_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void DMAC_3_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void DMAC_4_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void EVSYS_0_Handler    (void) __attribute__((weak, alias("Default_Handler")));
void EVSYS_1_Handler    (void) __attribute__((weak, alias("Default_Handler")));
void EVSYS_2_Handler    (void) __attribute__((weak, alias("Default_Handler")));
void EVSYS_3_Handler    (void) __attribute__((weak, alias("Default_Handler")));
void EVSYS_4_Handler    (void) __attribute__((weak, alias("Default_Handler")));
void PAC_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TAL_0_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void TAL_1_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void RAMECC_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM0_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM0_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM0_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM0_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM1_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM1_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM1_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM1_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM2_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM2_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM2_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM2_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM3_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM3_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM3_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM3_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM4_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM4_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM4_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM4_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM5_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM5_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM5_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM5_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM6_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM6_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM6_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM6_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM7_0_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM7_1_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM7_2_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void SERCOM7_3_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void CAN0_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void CAN1_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void USB_0_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void USB_1_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void USB_2_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void USB_3_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void GMAC_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void TCC0_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC0_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC0_2_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC0_3_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC0_4_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC0_5_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC0_6_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC1_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC1_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC1_2_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC1_3_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC1_4_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC2_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC2_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC2_2_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC2_3_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC3_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC3_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC3_2_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC4_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC4_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TCC4_2_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void TC0_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC1_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC2_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC3_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC4_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC5_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC6_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TC7_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void PDEC_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void PDEC_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void PDEC_2_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void ADC0_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void ADC0_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void ADC1_0_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void ADC1_1_Handler     (void) __attribute__((weak, alias("Default_Handler")));
void AC_Handler         (void) __attribute__((weak, alias("Default_Handler")));
void DAC_0_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void DAC_1_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void DAC_2_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void DAC_3_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void DAC_4_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void I2S_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void PCC_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void AES_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void TRNG_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void ICM_Handler        (void) __attribute__((weak, alias("Default_Handler")));
void PUKCC_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void QSPI_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void SDHC0_Handler      (void) __attribute__((weak, alias("Default_Handler")));
void SDHC1_Handler      (void) __attribute__((weak, alias("Default_Handler")));

//Vector table
__attribute__((section(".vectors")))
const uint32_t vectorTable[153] =
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
   (uint32_t) PM_Handler,         //0 Power Manager
   (uint32_t) MCLK_Handler,       //1 Main Clock
   (uint32_t) OSCCTRL_0_Handler,  //2 OSCCTRL_XOSCFAIL_0, OSCCTRL_XOSCRDY_0
   (uint32_t) OSCCTRL_1_Handler,  //3 OSCCTRL_XOSCFAIL_1, OSCCTRL_XOSCRDY_1
   (uint32_t) OSCCTRL_2_Handler,  //4 OSCCTRL_DFLLLOCKC, OSCCTRL_DFLLLOCKF, OSCCTRL_DFLLOOB, OSCCTRL_DFLLRCS, OSCCTRL_DFLLRDY
   (uint32_t) OSCCTRL_3_Handler,  //5 OSCCTRL_DPLLLCKF_0, OSCCTRL_DPLLLCKR_0, OSCCTRL_DPLLLDRTO_0, OSCCTRL_DPLLLTO_0
   (uint32_t) OSCCTRL_4_Handler,  //6 OSCCTRL_DPLLLCKF_1, OSCCTRL_DPLLLCKR_1, OSCCTRL_DPLLLDRTO_1, OSCCTRL_DPLLLTO_1
   (uint32_t) OSC32KCTRL_Handler, //7 32kHz Oscillators Control
   (uint32_t) SUPC_0_Handler,     //8 SUPC_B12SRDY, SUPC_B33SRDY, SUPC_BOD12RDY, SUPC_BOD33RDY, SUPC_VCORERDY, SUPC_VREGRDY
   (uint32_t) SUPC_1_Handler,     //9 SUPC_BOD12DET, SUPC_BOD33DET
   (uint32_t) WDT_Handler,        //10 Watchdog Timer
   (uint32_t) RTC_Handler,        //11 Real-Time Counter
   (uint32_t) EIC_0_Handler,      //12 EIC_EXTINT_0
   (uint32_t) EIC_1_Handler,      //13 EIC_EXTINT_1
   (uint32_t) EIC_2_Handler,      //14 EIC_EXTINT_2
   (uint32_t) EIC_3_Handler,      //15 EIC_EXTINT_3
   (uint32_t) EIC_4_Handler,      //16 EIC_EXTINT_4
   (uint32_t) EIC_5_Handler,      //17 EIC_EXTINT_5
   (uint32_t) EIC_6_Handler,      //18 EIC_EXTINT_6
   (uint32_t) EIC_7_Handler,      //19 EIC_EXTINT_7
   (uint32_t) EIC_8_Handler,      //20 EIC_EXTINT_8
   (uint32_t) EIC_9_Handler,      //21 EIC_EXTINT_9
   (uint32_t) EIC_10_Handler,     //22 EIC_EXTINT_10
   (uint32_t) EIC_11_Handler,     //23 EIC_EXTINT_11
   (uint32_t) EIC_12_Handler,     //24 EIC_EXTINT_12
   (uint32_t) EIC_13_Handler,     //25 EIC_EXTINT_13
   (uint32_t) EIC_14_Handler,     //26 EIC_EXTINT_14
   (uint32_t) EIC_15_Handler,     //27 EIC_EXTINT_15
   (uint32_t) FREQM_Handler,      //28 Frequency Meter
   (uint32_t) NVMCTRL_0_Handler,  //29 NVMCTRL_0, NVMCTRL_1, NVMCTRL_2, NVMCTRL_3, NVMCTRL_4, NVMCTRL_5, NVMCTRL_6, NVMCTRL_7
   (uint32_t) NVMCTRL_1_Handler,  //30 NVMCTRL_10, NVMCTRL_8, NVMCTRL_9
   (uint32_t) DMAC_0_Handler,     //31 DMAC_SUSP_0, DMAC_TCMPL_0, DMAC_TERR_0
   (uint32_t) DMAC_1_Handler,     //32 DMAC_SUSP_1, DMAC_TCMPL_1, DMAC_TERR_1
   (uint32_t) DMAC_2_Handler,     //33 DMAC_SUSP_2, DMAC_TCMPL_2, DMAC_TERR_2
   (uint32_t) DMAC_3_Handler,     //34 DMAC_SUSP_3, DMAC_TCMPL_3, DMAC_TERR_3
   (uint32_t) DMAC_4_Handler,     //35 DMAC_SUSP_10, DMAC_SUSP_11, DMAC_SUSP_12, DMAC_SUSP_13, DMAC_SUSP_14, DMAC_SUSP_15, DMAC_SUSP_16, DMAC_SUSP_17, DMAC_SUSP_18, DMAC_SUSP_19, DMAC_SUSP_20, DMAC_SUSP_21, DMAC_SUSP_22, DMAC_SUSP_23, DMAC_SUSP_24, DMAC_SUSP_25, DMAC_SUSP_26, DMAC_SUSP_27, DMAC_SUSP_28, DMAC_SUSP_29, DMAC_SUSP_30, DMAC_SUSP_31, DMAC_SUSP_4, DMAC_SUSP_5, DMAC_SUSP_6, DMAC_SUSP_7, DMAC_SUSP_8, DMAC_SUSP_9, DMAC_TCMPL_10, DMAC_TCMPL_11, DMAC_TCMPL_12, DMAC_TCMPL_13, DMAC_TCMPL_14, DMAC_TCMPL_15, DMAC_TCMPL_16, DMAC_TCMPL_17, DMAC_TCMPL_18, DMAC_TCMPL_19, DMAC_TCMPL_20, DMAC_TCMPL_21, DMAC_TCMPL_22, DMAC_TCMPL_23, DMAC_TCMPL_24, DMAC_TCMPL_25, DMAC_TCMPL_26, DMAC_TCMPL_27, DMAC_TCMPL_28, DMAC_TCMPL_29, DMAC_TCMPL_30, DMAC_TCMPL_31, DMAC_TCMPL_4, DMAC_TCMPL_5, DMAC_TCMPL_6, DMAC_TCMPL_7, DMAC_TCMPL_8, DMAC_TCMPL_9, DMAC_TERR_10, DMAC_TERR_11, DMAC_TERR_12, DMAC_TERR_13, DMAC_TERR_14, DMAC_TERR_15, DMAC_TERR_16, DMAC_TERR_17, DMAC_TERR_18, DMAC_TERR_19, DMAC_TERR_20, DMAC_TERR_21, DMAC_TERR_22, DMAC_TERR_23, DMAC_TERR_24, DMAC_TERR_25, DMAC_TERR_26, DMAC_TERR_27, DMAC_TERR_28, DMAC_TERR_29, DMAC_TERR_30, DMAC_TERR_31, DMAC_TERR_4, DMAC_TERR_5, DMAC_TERR_6, DMAC_TERR_7, DMAC_TERR_8, DMAC_TERR_9
   (uint32_t) EVSYS_0_Handler,    //36 EVSYS_EVD_0, EVSYS_OVR_0
   (uint32_t) EVSYS_1_Handler,    //37 EVSYS_EVD_1, EVSYS_OVR_1
   (uint32_t) EVSYS_2_Handler,    //38 EVSYS_EVD_2, EVSYS_OVR_2
   (uint32_t) EVSYS_3_Handler,    //39 EVSYS_EVD_3, EVSYS_OVR_3
   (uint32_t) EVSYS_4_Handler,    //40 EVSYS_EVD_10, EVSYS_EVD_11, EVSYS_EVD_4, EVSYS_EVD_5, EVSYS_EVD_6, EVSYS_EVD_7, EVSYS_EVD_8, EVSYS_EVD_9, EVSYS_OVR_10, EVSYS_OVR_11, EVSYS_OVR_4, EVSYS_OVR_5, EVSYS_OVR_6, EVSYS_OVR_7, EVSYS_OVR_8, EVSYS_OVR_9
   (uint32_t) PAC_Handler,        //41 Peripheral Access Controller
   (uint32_t) TAL_0_Handler,      //42 TAL_BRK
   (uint32_t) TAL_1_Handler,      //43 TAL_IPS_0, TAL_IPS_1
   (uint32_t) Default_Handler,    //44 Reserved
   (uint32_t) RAMECC_Handler,     //45 RAM ECC
   (uint32_t) SERCOM0_0_Handler,  //46 SERCOM0_0
   (uint32_t) SERCOM0_1_Handler,  //47 SERCOM0_1
   (uint32_t) SERCOM0_2_Handler,  //48 SERCOM0_2
   (uint32_t) SERCOM0_3_Handler,  //49 SERCOM0_3, SERCOM0_4, SERCOM0_5, SERCOM0_6
   (uint32_t) SERCOM1_0_Handler,  //50 SERCOM1_0
   (uint32_t) SERCOM1_1_Handler,  //51 SERCOM1_1
   (uint32_t) SERCOM1_2_Handler,  //52 SERCOM1_2
   (uint32_t) SERCOM1_3_Handler,  //53 SERCOM1_3, SERCOM1_4, SERCOM1_5, SERCOM1_6
   (uint32_t) SERCOM2_0_Handler,  //54 SERCOM2_0
   (uint32_t) SERCOM2_1_Handler,  //55 SERCOM2_1
   (uint32_t) SERCOM2_2_Handler,  //56 SERCOM2_2
   (uint32_t) SERCOM2_3_Handler,  //57 SERCOM2_3, SERCOM2_4, SERCOM2_5, SERCOM2_6
   (uint32_t) SERCOM3_0_Handler,  //58 SERCOM3_0
   (uint32_t) SERCOM3_1_Handler,  //59 SERCOM3_1
   (uint32_t) SERCOM3_2_Handler,  //60 SERCOM3_2
   (uint32_t) SERCOM3_3_Handler,  //61 SERCOM3_3, SERCOM3_4, SERCOM3_5, SERCOM3_6
   (uint32_t) SERCOM4_0_Handler,  //62 SERCOM4_0
   (uint32_t) SERCOM4_1_Handler,  //63 SERCOM4_1
   (uint32_t) SERCOM4_2_Handler,  //64 SERCOM4_2
   (uint32_t) SERCOM4_3_Handler,  //65 SERCOM4_3, SERCOM4_4, SERCOM4_5, SERCOM4_6
   (uint32_t) SERCOM5_0_Handler,  //66 SERCOM5_0
   (uint32_t) SERCOM5_1_Handler,  //67 SERCOM5_1
   (uint32_t) SERCOM5_2_Handler,  //68 SERCOM5_2
   (uint32_t) SERCOM5_3_Handler,  //69 SERCOM5_3, SERCOM5_4, SERCOM5_5, SERCOM5_6
   (uint32_t) SERCOM6_0_Handler,  //70 SERCOM6_0
   (uint32_t) SERCOM6_1_Handler,  //71 SERCOM6_1
   (uint32_t) SERCOM6_2_Handler,  //72 SERCOM6_2
   (uint32_t) SERCOM6_3_Handler,  //73 SERCOM6_3, SERCOM6_4, SERCOM6_5, SERCOM6_6
   (uint32_t) SERCOM7_0_Handler,  //74 SERCOM7_0
   (uint32_t) SERCOM7_1_Handler,  //75 SERCOM7_1
   (uint32_t) SERCOM7_2_Handler,  //76 SERCOM7_2
   (uint32_t) SERCOM7_3_Handler,  //77 SERCOM7_3, SERCOM7_4, SERCOM7_5, SERCOM7_6
   (uint32_t) CAN0_Handler,       //78 Control Area Network 0
   (uint32_t) CAN1_Handler,       //79 Control Area Network 1
   (uint32_t) USB_0_Handler,      //80 USB_EORSM_DNRSM, USB_EORST_RST, USB_LPMSUSP_DDISC, USB_LPM_DCONN, USB_MSOF, USB_RAMACER, USB_RXSTP_TXSTP_0, USB_RXSTP_TXSTP_1, USB_RXSTP_TXSTP_2, USB_RXSTP_TXSTP_3, USB_RXSTP_TXSTP_4, USB_RXSTP_TXSTP_5, USB_RXSTP_TXSTP_6, USB_RXSTP_TXSTP_7, USB_STALL0_STALL_0, USB_STALL0_STALL_1, USB_STALL0_STALL_2, USB_STALL0_STALL_3, USB_STALL0_STALL_4, USB_STALL0_STALL_5, USB_STALL0_STALL_6, USB_STALL0_STALL_7, USB_STALL1_0, USB_STALL1_1, USB_STALL1_2, USB_STALL1_3, USB_STALL1_4, USB_STALL1_5, USB_STALL1_6, USB_STALL1_7, USB_SUSPEND, USB_TRFAIL0_TRFAIL_0, USB_TRFAIL0_TRFAIL_1, USB_TRFAIL0_TRFAIL_2, USB_TRFAIL0_TRFAIL_3, USB_TRFAIL0_TRFAIL_4, USB_TRFAIL0_TRFAIL_5, USB_TRFAIL0_TRFAIL_6, USB_TRFAIL0_TRFAIL_7, USB_TRFAIL1_PERR_0, USB_TRFAIL1_PERR_1, USB_TRFAIL1_PERR_2, USB_TRFAIL1_PERR_3, USB_TRFAIL1_PERR_4, USB_TRFAIL1_PERR_5, USB_TRFAIL1_PERR_6, USB_TRFAIL1_PERR_7, USB_UPRSM, USB_WAKEUP
   (uint32_t) USB_1_Handler,      //81 USB_SOF_HSOF
   (uint32_t) USB_2_Handler,      //82 USB_TRCPT0_0, USB_TRCPT0_1, USB_TRCPT0_2, USB_TRCPT0_3, USB_TRCPT0_4, USB_TRCPT0_5, USB_TRCPT0_6, USB_TRCPT0_7
   (uint32_t) USB_3_Handler,      //83 USB_TRCPT1_0, USB_TRCPT1_1, USB_TRCPT1_2, USB_TRCPT1_3, USB_TRCPT1_4, USB_TRCPT1_5, USB_TRCPT1_6, USB_TRCPT1_7
   (uint32_t) GMAC_Handler,       //84 Ethernet MAC
   (uint32_t) TCC0_0_Handler,     //85 TCC0_CNT_A, TCC0_DFS_A, TCC0_ERR_A, TCC0_FAULT0_A, TCC0_FAULT1_A, TCC0_FAULTA_A, TCC0_FAULTB_A, TCC0_OVF, TCC0_TRG, TCC0_UFS_A
   (uint32_t) TCC0_1_Handler,     //86 TCC0_MC_0
   (uint32_t) TCC0_2_Handler,     //87 TCC0_MC_1
   (uint32_t) TCC0_3_Handler,     //88 TCC0_MC_2
   (uint32_t) TCC0_4_Handler,     //89 TCC0_MC_3
   (uint32_t) TCC0_5_Handler,     //90 TCC0_MC_4
   (uint32_t) TCC0_6_Handler,     //91 TCC0_MC_5
   (uint32_t) TCC1_0_Handler,     //92 TCC1_CNT_A, TCC1_DFS_A, TCC1_ERR_A, TCC1_FAULT0_A, TCC1_FAULT1_A, TCC1_FAULTA_A, TCC1_FAULTB_A, TCC1_OVF, TCC1_TRG, TCC1_UFS_A
   (uint32_t) TCC1_1_Handler,     //93 TCC1_MC_0
   (uint32_t) TCC1_2_Handler,     //94 TCC1_MC_1
   (uint32_t) TCC1_3_Handler,     //95 TCC1_MC_2
   (uint32_t) TCC1_4_Handler,     //96 TCC1_MC_3
   (uint32_t) TCC2_0_Handler,     //97 TCC2_CNT_A, TCC2_DFS_A, TCC2_ERR_A, TCC2_FAULT0_A, TCC2_FAULT1_A, TCC2_FAULTA_A, TCC2_FAULTB_A, TCC2_OVF, TCC2_TRG, TCC2_UFS_A
   (uint32_t) TCC2_1_Handler,     //98 TCC2_MC_0
   (uint32_t) TCC2_2_Handler,     //99 TCC2_MC_1
   (uint32_t) TCC2_3_Handler,     //100 TCC2_MC_2
   (uint32_t) TCC3_0_Handler,     //101 TCC3_CNT_A, TCC3_DFS_A, TCC3_ERR_A, TCC3_FAULT0_A, TCC3_FAULT1_A, TCC3_FAULTA_A, TCC3_FAULTB_A, TCC3_OVF, TCC3_TRG, TCC3_UFS_A
   (uint32_t) TCC3_1_Handler,     //102 TCC3_MC_0
   (uint32_t) TCC3_2_Handler,     //103 TCC3_MC_1
   (uint32_t) TCC4_0_Handler,     //104 TCC4_CNT_A, TCC4_DFS_A, TCC4_ERR_A, TCC4_FAULT0_A, TCC4_FAULT1_A, TCC4_FAULTA_A, TCC4_FAULTB_A, TCC4_OVF, TCC4_TRG, TCC4_UFS_A
   (uint32_t) TCC4_1_Handler,     //105 TCC4_MC_0
   (uint32_t) TCC4_2_Handler,     //106 TCC4_MC_1
   (uint32_t) TC0_Handler,        //107 Basic Timer Counter 0
   (uint32_t) TC1_Handler,        //108 Basic Timer Counter 1
   (uint32_t) TC2_Handler,        //109 Basic Timer Counter 2
   (uint32_t) TC3_Handler,        //110 Basic Timer Counter 3
   (uint32_t) TC4_Handler,        //111 Basic Timer Counter 4
   (uint32_t) TC5_Handler,        //112 Basic Timer Counter 5
   (uint32_t) TC6_Handler,        //113 Basic Timer Counter 6
   (uint32_t) TC7_Handler,        //114 Basic Timer Counter 7
   (uint32_t) PDEC_0_Handler,     //115 PDEC_DIR_A, PDEC_ERR_A, PDEC_OVF, PDEC_VLC_A
   (uint32_t) PDEC_1_Handler,     //116 PDEC_MC_0
   (uint32_t) PDEC_2_Handler,     //117 PDEC_MC_1
   (uint32_t) ADC0_0_Handler,     //118 ADC0_OVERRUN, ADC0_WINMON
   (uint32_t) ADC0_1_Handler,     //119 ADC0_RESRDY
   (uint32_t) ADC1_0_Handler,     //120 ADC1_OVERRUN, ADC1_WINMON
   (uint32_t) ADC1_1_Handler,     //121 ADC1_RESRDY
   (uint32_t) AC_Handler,         //122 Analog Comparators
   (uint32_t) DAC_0_Handler,      //123 DAC_OVERRUN_A_x
   (uint32_t) DAC_1_Handler,      //124 DAC_EMPTY_0
   (uint32_t) DAC_2_Handler,      //125 DAC_EMPTY_1
   (uint32_t) DAC_3_Handler,      //126 DAC_RESRDY_0
   (uint32_t) DAC_4_Handler,      //127 DAC_RESRDY_1
   (uint32_t) I2S_Handler,        //128 Inter-IC Sound Interface
   (uint32_t) PCC_Handler,        //129 Parallel Capture Controller
   (uint32_t) AES_Handler,        //130 Advanced Encryption Standard
   (uint32_t) TRNG_Handler,       //131 True Random Generator
   (uint32_t) ICM_Handler,        //132 Integrity Check Monitor
   (uint32_t) PUKCC_Handler,      //133 PUblic-Key Cryptography Controller
   (uint32_t) QSPI_Handler,       //134 Quad SPI interface
   (uint32_t) SDHC0_Handler,      //135 SD/MMC Host Controller 0
   (uint32_t) SDHC1_Handler,      //136 SD/MMC Host Controller 1
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

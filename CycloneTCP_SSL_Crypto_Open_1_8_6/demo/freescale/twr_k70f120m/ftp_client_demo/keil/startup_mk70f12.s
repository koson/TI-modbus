;/*****************************************************************************
; * @file:    startup_MK70F12.s
; * @purpose: CMSIS Cortex-M4 Core Device Startup File for the
; *           MK70F12
; * @version: 1.5
; * @date:    2012-10-19
; *
; * Copyright: 1997 - 2012 Freescale Semiconductor, Inc. All Rights Reserved.
;*
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; *****************************************************************************/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

;Heap_Size       EQU     0x00000000

;                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
;__heap_base
;Heap_Mem        SPACE   Heap_Size
;__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp  ; Top of Stack
                DCD     Reset_Handler  ; Reset Handler
                DCD     NMI_Handler  ; NMI Handler
                DCD     HardFault_Handler  ; Hard Fault Handler
                DCD     MemManage_Handler  ; MPU Fault Handler
                DCD     BusFault_Handler  ; Bus Fault Handler
                DCD     UsageFault_Handler  ; Usage Fault Handler
                DCD     0  ; Reserved
                DCD     0  ; Reserved
                DCD     0  ; Reserved
                DCD     0  ; Reserved
                DCD     SVC_Handler  ; SVCall Handler
                DCD     DebugMon_Handler  ; Debug Monitor Handler
                DCD     0  ; Reserved
                DCD     PendSV_Handler  ; PendSV Handler
                DCD     SysTick_Handler  ; SysTick Handler

                ; External Interrupts
                DCD     DMA0_DMA16_IRQHandler  ; DMA channel 0/16 transfer complete interrupt
                DCD     DMA1_DMA17_IRQHandler  ; DMA channel 1/17 transfer complete interrupt
                DCD     DMA2_DMA18_IRQHandler  ; DMA channel 2/18 transfer complete interrupt
                DCD     DMA3_DMA19_IRQHandler  ; DMA channel 3/19 transfer complete interrupt
                DCD     DMA4_DMA20_IRQHandler  ; DMA channel 4/20 transfer complete interrupt
                DCD     DMA5_DMA21_IRQHandler  ; DMA channel 5/21 transfer complete interrupt
                DCD     DMA6_DMA22_IRQHandler  ; DMA channel 6/22 transfer complete interrupt
                DCD     DMA7_DMA23_IRQHandler  ; DMA channel 7/23 transfer complete interrupt
                DCD     DMA8_DMA24_IRQHandler  ; DMA channel 8/24 transfer complete interrupt
                DCD     DMA9_DMA25_IRQHandler  ; DMA channel 9/25 transfer complete interrupt
                DCD     DMA10_DMA26_IRQHandler  ; DMA channel 10/26 transfer complete interrupt
                DCD     DMA11_DMA27_IRQHandler  ; DMA channel 11/27 transfer complete interrupt
                DCD     DMA12_DMA28_IRQHandler  ; DMA channel 12/28 transfer complete interrupt
                DCD     DMA13_DMA29_IRQHandler  ; DMA channel 13/29 transfer complete interrupt
                DCD     DMA14_DMA30_IRQHandler  ; DMA channel 14/30 transfer complete interrupt
                DCD     DMA15_DMA31_IRQHandler  ; DMA channel 15/31 transfer complete interrupt
                DCD     DMA_Error_IRQHandler  ; DMA error interrupt
                DCD     MCM_IRQHandler  ; Normal interrupt
                DCD     FTFE_IRQHandler  ; FTFE interrupt
                DCD     Read_Collision_IRQHandler  ; Read collision interrupt
                DCD     LVD_LVW_IRQHandler  ; Low Voltage Detect, Low Voltage Warning
                DCD     LLW_IRQHandler  ; Low Leakage Wakeup
                DCD     Watchdog_IRQHandler  ; WDOG interrupt
                DCD     RNG_IRQHandler  ; RNGA interrupt
                DCD     I2C0_IRQHandler  ; I2C0 interrupt
                DCD     I2C1_IRQHandler  ; I2C1 interrupt
                DCD     SPI0_IRQHandler  ; SPI0 interrupt
                DCD     SPI1_IRQHandler  ; SPI1 interrupt
                DCD     SPI2_IRQHandler  ; SPI2 interrupt
                DCD     CAN0_ORed_Message_buffer_IRQHandler  ; CAN0 OR'd message buffers interrupt
                DCD     CAN0_Bus_Off_IRQHandler  ; CAN0 bus off interrupt
                DCD     CAN0_Error_IRQHandler  ; CAN0 error interrupt
                DCD     CAN0_Tx_Warning_IRQHandler  ; CAN0 Tx warning interrupt
                DCD     CAN0_Rx_Warning_IRQHandler  ; CAN0 Rx warning interrupt
                DCD     CAN0_Wake_Up_IRQHandler  ; CAN0 wake up interrupt
                DCD     I2S0_Tx_IRQHandler  ; I2S0 transmit interrupt
                DCD     I2S0_Rx_IRQHandler  ; I2S0 receive interrupt
                DCD     CAN1_ORed_Message_buffer_IRQHandler  ; CAN1 OR'd message buffers interrupt
                DCD     CAN1_Bus_Off_IRQHandler  ; CAN1 bus off interrupt
                DCD     CAN1_Error_IRQHandler  ; CAN1 error interrupt
                DCD     CAN1_Tx_Warning_IRQHandler  ; CAN1 Tx warning interrupt
                DCD     CAN1_Rx_Warning_IRQHandler  ; CAN1 Rx warning interrupt
                DCD     CAN1_Wake_Up_IRQHandler  ; CAN1 wake up interrupt
                DCD     Reserved59_IRQHandler  ; Reserved interrupt 59
                DCD     UART0_LON_IRQHandler  ; UART0 LON interrupt
                DCD     UART0_RX_TX_IRQHandler  ; UART0 receive/transmit interrupt
                DCD     UART0_ERR_IRQHandler  ; UART0 error interrupt
                DCD     UART1_RX_TX_IRQHandler  ; UART1 receive/transmit interrupt
                DCD     UART1_ERR_IRQHandler  ; UART1 error interrupt
                DCD     UART2_RX_TX_IRQHandler  ; UART2 receive/transmit interrupt
                DCD     UART2_ERR_IRQHandler  ; UART2 error interrupt
                DCD     UART3_RX_TX_IRQHandler  ; UART3 receive/transmit interrupt
                DCD     UART3_ERR_IRQHandler  ; UART3 error interrupt
                DCD     UART4_RX_TX_IRQHandler  ; UART4 receive/transmit interrupt
                DCD     UART4_ERR_IRQHandler  ; UART4 error interrupt
                DCD     UART5_RX_TX_IRQHandler  ; UART5 receive/transmit interrupt
                DCD     UART5_ERR_IRQHandler  ; UART5 error interrupt
                DCD     ADC0_IRQHandler  ; ADC0 interrupt
                DCD     ADC1_IRQHandler  ; ADC1 interrupt
                DCD     CMP0_IRQHandler  ; CMP0 interrupt
                DCD     CMP1_IRQHandler  ; CMP1 interrupt
                DCD     CMP2_IRQHandler  ; CMP2 interrupt
                DCD     FTM0_IRQHandler  ; FTM0 fault, overflow and channels interrupt
                DCD     FTM1_IRQHandler  ; FTM1 fault, overflow and channels interrupt
                DCD     FTM2_IRQHandler  ; FTM2 fault, overflow and channels interrupt
                DCD     CMT_IRQHandler  ; CMT interrupt
                DCD     RTC_IRQHandler  ; RTC interrupt
                DCD     RTC_Seconds_IRQHandler  ; RTC seconds interrupt
                DCD     PIT0_IRQHandler  ; PIT timer channel 0 interrupt
                DCD     PIT1_IRQHandler  ; PIT timer channel 1 interrupt
                DCD     PIT2_IRQHandler  ; PIT timer channel 2 interrupt
                DCD     PIT3_IRQHandler  ; PIT timer channel 3 interrupt
                DCD     PDB0_IRQHandler  ; PDB0 interrupt
                DCD     USB0_IRQHandler  ; USB0 interrupt
                DCD     USBDCD_IRQHandler  ; USBDCD interrupt
                DCD     ENET_1588_Timer_IRQHandler  ; Ethernet MAC IEEE 1588 timer interrupt
                DCD     ENET_Transmit_IRQHandler  ; Ethernet MAC transmit interrupt
                DCD     ENET_Receive_IRQHandler  ; Ethernet MAC receive interrupt
                DCD     ENET_Error_IRQHandler  ; Ethernet MAC error and miscelaneous interrupt
                DCD     Reserved95_IRQHandler  ; Reserved interrupt 95
                DCD     SDHC_IRQHandler  ; SDHC interrupt
                DCD     DAC0_IRQHandler  ; DAC0 interrupt
                DCD     DAC1_IRQHandler  ; DAC1 interrupt
                DCD     TSI0_IRQHandler  ; TSI0 interrupt
                DCD     MCG_IRQHandler  ; MCG interrupt
                DCD     LPTimer_IRQHandler  ; LPTimer interrupt
                DCD     Reserved102_IRQHandler  ; Reserved interrupt 102
                DCD     PORTA_IRQHandler  ; Port A interrupt
                DCD     PORTB_IRQHandler  ; Port B interrupt
                DCD     PORTC_IRQHandler  ; Port C interrupt
                DCD     PORTD_IRQHandler  ; Port D interrupt
                DCD     PORTE_IRQHandler  ; Port E interrupt
                DCD     PORTF_IRQHandler  ; Port F interrupt
                DCD     DDR_IRQHandler  ; DDR interrupt
                DCD     SWI_IRQHandler  ; Software interrupt
                DCD     NFC_IRQHandler  ; NAND flash controller interrupt
                DCD     USBHS_IRQHandler  ; USB high speed OTG interrupt
                DCD     LCD_IRQHandler  ; Graphical LCD interrupt
                DCD     CMP3_IRQHandler  ; CMP3 interrupt
                DCD     Reserved115_IRQHandler  ; Reserved interrupt 115
                DCD     Reserved116_IRQHandler  ; Reserved interrupt 116
                DCD     FTM3_IRQHandler  ; FTM3 fault, overflow and channels interrupt
                DCD     ADC2_IRQHandler  ; ADC2 interrupt
                DCD     ADC3_IRQHandler  ; ADC3 interrupt
                DCD     I2S1_Tx_IRQHandler  ; I2S1 transmit interrupt
                DCD     I2S1_Rx_IRQHandler  ; I2S1 receive interrupt
                DCD     DefaultISR  ; 122
                DCD     DefaultISR  ; 123
                DCD     DefaultISR  ; 124
                DCD     DefaultISR  ; 125
                DCD     DefaultISR  ; 126
                DCD     DefaultISR  ; 127
                DCD     DefaultISR  ; 128
                DCD     DefaultISR  ; 129
                DCD     DefaultISR  ; 130
                DCD     DefaultISR  ; 131
                DCD     DefaultISR  ; 132
                DCD     DefaultISR  ; 133
                DCD     DefaultISR  ; 134
                DCD     DefaultISR  ; 135
                DCD     DefaultISR  ; 136
                DCD     DefaultISR  ; 137
                DCD     DefaultISR  ; 138
                DCD     DefaultISR  ; 139
                DCD     DefaultISR  ; 140
                DCD     DefaultISR  ; 141
                DCD     DefaultISR  ; 142
                DCD     DefaultISR  ; 143
                DCD     DefaultISR  ; 144
                DCD     DefaultISR  ; 145
                DCD     DefaultISR  ; 146
                DCD     DefaultISR  ; 147
                DCD     DefaultISR  ; 148
                DCD     DefaultISR  ; 149
                DCD     DefaultISR  ; 150
                DCD     DefaultISR  ; 151
                DCD     DefaultISR  ; 152
                DCD     DefaultISR  ; 153
                DCD     DefaultISR  ; 154
                DCD     DefaultISR  ; 155
                DCD     DefaultISR  ; 156
                DCD     DefaultISR  ; 157
                DCD     DefaultISR  ; 158
                DCD     DefaultISR  ; 159
                DCD     DefaultISR  ; 160
                DCD     DefaultISR  ; 161
                DCD     DefaultISR  ; 162
                DCD     DefaultISR  ; 163
                DCD     DefaultISR  ; 164
                DCD     DefaultISR  ; 165
                DCD     DefaultISR  ; 166
                DCD     DefaultISR  ; 167
                DCD     DefaultISR  ; 168
                DCD     DefaultISR  ; 169
                DCD     DefaultISR  ; 170
                DCD     DefaultISR  ; 171
                DCD     DefaultISR  ; 172
                DCD     DefaultISR  ; 173
                DCD     DefaultISR  ; 174
                DCD     DefaultISR  ; 175
                DCD     DefaultISR  ; 176
                DCD     DefaultISR  ; 177
                DCD     DefaultISR  ; 178
                DCD     DefaultISR  ; 179
                DCD     DefaultISR  ; 180
                DCD     DefaultISR  ; 181
                DCD     DefaultISR  ; 182
                DCD     DefaultISR  ; 183
                DCD     DefaultISR  ; 184
                DCD     DefaultISR  ; 185
                DCD     DefaultISR  ; 186
                DCD     DefaultISR  ; 187
                DCD     DefaultISR  ; 188
                DCD     DefaultISR  ; 189
                DCD     DefaultISR  ; 190
                DCD     DefaultISR  ; 191
                DCD     DefaultISR  ; 192
                DCD     DefaultISR  ; 193
                DCD     DefaultISR  ; 194
                DCD     DefaultISR  ; 195
                DCD     DefaultISR  ; 196
                DCD     DefaultISR  ; 197
                DCD     DefaultISR  ; 198
                DCD     DefaultISR  ; 199
                DCD     DefaultISR  ; 200
                DCD     DefaultISR  ; 201
                DCD     DefaultISR  ; 202
                DCD     DefaultISR  ; 203
                DCD     DefaultISR  ; 204
                DCD     DefaultISR  ; 205
                DCD     DefaultISR  ; 206
                DCD     DefaultISR  ; 207
                DCD     DefaultISR  ; 208
                DCD     DefaultISR  ; 209
                DCD     DefaultISR  ; 210
                DCD     DefaultISR  ; 211
                DCD     DefaultISR  ; 212
                DCD     DefaultISR  ; 213
                DCD     DefaultISR  ; 214
                DCD     DefaultISR  ; 215
                DCD     DefaultISR  ; 216
                DCD     DefaultISR  ; 217
                DCD     DefaultISR  ; 218
                DCD     DefaultISR  ; 219
                DCD     DefaultISR  ; 220
                DCD     DefaultISR  ; 221
                DCD     DefaultISR  ; 222
                DCD     DefaultISR  ; 223
                DCD     DefaultISR  ; 224
                DCD     DefaultISR  ; 225
                DCD     DefaultISR  ; 226
                DCD     DefaultISR  ; 227
                DCD     DefaultISR  ; 228
                DCD     DefaultISR  ; 229
                DCD     DefaultISR  ; 230
                DCD     DefaultISR  ; 231
                DCD     DefaultISR  ; 232
                DCD     DefaultISR  ; 233
                DCD     DefaultISR  ; 234
                DCD     DefaultISR  ; 235
                DCD     DefaultISR  ; 236
                DCD     DefaultISR  ; 237
                DCD     DefaultISR  ; 238
                DCD     DefaultISR  ; 239
                DCD     DefaultISR  ; 240
                DCD     DefaultISR  ; 241
                DCD     DefaultISR  ; 242
                DCD     DefaultISR  ; 243
                DCD     DefaultISR  ; 244
                DCD     DefaultISR  ; 245
                DCD     DefaultISR  ; 246
                DCD     DefaultISR  ; 247
                DCD     DefaultISR  ; 248
                DCD     DefaultISR  ; 249
                DCD     DefaultISR  ; 250
                DCD     DefaultISR  ; 251
                DCD     DefaultISR  ; 252
                DCD     DefaultISR  ; 253
                DCD     DefaultISR  ; 254
                DCD     DefaultISR  ; 255
__Vectors_End

__Vectors_Size 	EQU     __Vectors_End - __Vectors

; <h> Flash Configuration
;   <i> 16-byte flash configuration field that stores default protection settings (loaded on reset)
;   <i> and security information that allows the MCU to restrict acces to the FTFL module.
;   <h> Backdoor Comparison Key
;     <o0>  Backdoor Key 0  <0x0-0xFF:2>
;     <o1>  Backdoor Key 1  <0x0-0xFF:2>
;     <o2>  Backdoor Key 2  <0x0-0xFF:2>
;     <o3>  Backdoor Key 3  <0x0-0xFF:2>
;     <o4>  Backdoor Key 4  <0x0-0xFF:2>
;     <o5>  Backdoor Key 5  <0x0-0xFF:2>
;     <o6>  Backdoor Key 6  <0x0-0xFF:2>
;     <o7>  Backdoor Key 7  <0x0-0xFF:2>
BackDoorK0      EQU     0xFF
BackDoorK1      EQU     0xFF
BackDoorK2      EQU     0xFF
BackDoorK3      EQU     0xFF
BackDoorK4      EQU     0xFF
BackDoorK5      EQU     0xFF
BackDoorK6      EQU     0xFF
BackDoorK7      EQU     0xFF
;   </h>
;   <h> Program flash protection bytes (FPROT)
;     <i> Each program flash region can be protected from program and erase operation by setting the associated PROT bit.
;     <i> Each bit protects a 1/32 region of the program flash memory.
;     <h> FPROT0
;       <i> Program flash protection bytes
;       <i> 1/32 - 8/32 region
;       <o.0>   FPROT0.0
;       <o.1>   FPROT0.1
;       <o.2>   FPROT0.2
;       <o.3>   FPROT0.3
;       <o.4>   FPROT0.4
;       <o.5>   FPROT0.5
;       <o.6>   FPROT0.6
;       <o.7>   FPROT0.7
nFPROT0         EQU     0x00
FPROT0          EQU     nFPROT0:EOR:0xFF
;     </h>
;     <h> FPROT1
;       <i> Program Flash Region Protect Register 1
;       <i> 9/32 - 16/32 region
;       <o.0>   FPROT1.0
;       <o.1>   FPROT1.1
;       <o.2>   FPROT1.2
;       <o.3>   FPROT1.3
;       <o.4>   FPROT1.4
;       <o.5>   FPROT1.5
;       <o.6>   FPROT1.6
;       <o.7>   FPROT1.7
nFPROT1         EQU     0x00
FPROT1          EQU     nFPROT1:EOR:0xFF
;     </h>
;     <h> FPROT2
;       <i> Program Flash Region Protect Register 2
;       <i> 17/32 - 24/32 region
;       <o.0>   FPROT2.0
;       <o.1>   FPROT2.1
;       <o.2>   FPROT2.2
;       <o.3>   FPROT2.3
;       <o.4>   FPROT2.4
;       <o.5>   FPROT2.5
;       <o.6>   FPROT2.6
;       <o.7>   FPROT2.7
nFPROT2         EQU     0x00
FPROT2          EQU     nFPROT2:EOR:0xFF
;     </h>
;     <h> FPROT3
;       <i> Program Flash Region Protect Register 3
;       <i> 25/32 - 32/32 region
;       <o.0>   FPROT3.0
;       <o.1>   FPROT3.1
;       <o.2>   FPROT3.2
;       <o.3>   FPROT3.3
;       <o.4>   FPROT3.4
;       <o.5>   FPROT3.5
;       <o.6>   FPROT3.6
;       <o.7>   FPROT3.7
nFPROT3         EQU     0x00
FPROT3          EQU     nFPROT3:EOR:0xFF
;     </h>
;   </h>
;   <h> Data flash protection byte (FDPROT)
;     <i> Each bit protects a 1/8 region of the data flash memory.
;     <i> (Program flash only devices: Reserved)
;     <o.0>   FDPROT.0
;     <o.1>   FDPROT.1
;     <o.2>   FDPROT.2
;     <o.3>   FDPROT.3
;     <o.4>   FDPROT.4
;     <o.5>   FDPROT.5
;     <o.6>   FDPROT.6
;     <o.7>   FDPROT.7
nFDPROT         EQU     0x00
FDPROT          EQU     nFDPROT:EOR:0xFF
;   </h>
;   <h> EEPROM protection byte (FEPROT)
;     <i> FlexNVM devices: Each bit protects a 1/8 region of the EEPROM.
;     <i> (Program flash only devices: Reserved)
;     <o.0>   FEPROT.0
;     <o.1>   FEPROT.1
;     <o.2>   FEPROT.2
;     <o.3>   FEPROT.3
;     <o.4>   FEPROT.4
;     <o.5>   FEPROT.5
;     <o.6>   FEPROT.6
;     <o.7>   FEPROT.7
nFEPROT         EQU     0x00
FEPROT          EQU     nFEPROT:EOR:0xFF
;   </h>
;   <h> Flash nonvolatile option byte (FOPT)
;     <i> Allows the user to customize the operation of the MCU at boot time.
;     <o.0>  LPBOOT
;       <0=> Low-power boot
;       <1=> normal boot
;     <o.1>  EZPORT_DIS
;       <0=> EzPort operation is enabled
;       <1=> EzPort operation is disabled
FOPT            EQU     0xFF
;   </h>
;   <h> Flash security byte (FSEC)
;     <i> WARNING: If SEC field is configured as "MCU security status is secure" and MEEN field is configured as "Mass erase is disabled",
;     <i> MCU's security status cannot be set back to unsecure state since Mass erase via the debugger is blocked !!!
;     <o.0..1> SEC
;       <2=> MCU security status is unsecure
;       <3=> MCU security status is secure
;         <i> Flash Security
;         <i> This bits define the security state of the MCU.
;     <o.2..3> FSLACC
;       <2=> Freescale factory access denied
;       <3=> Freescale factory access granted
;         <i> Freescale Failure Analysis Access Code
;         <i> This bits define the security state of the MCU.
;     <o.4..5> MEEN
;       <2=> Mass erase is disabled
;       <3=> Mass erase is enabled
;         <i> Mass Erase Enable Bits
;         <i> Enables and disables mass erase capability of the FTFL module
;     <o.6..7> KEYEN
;       <2=> Backdoor key access enabled
;       <3=> Backdoor key access disabled
;         <i> Backdoor key Security Enable
;         <i> These bits enable and disable backdoor key access to the FTFL module.
FSEC            EQU     0xFE
;   </h>
; </h>
                IF      :LNOT::DEF:RAM_TARGET
                AREA    |.ARM.__at_0x400|, CODE, READONLY
                DCB     BackDoorK0, BackDoorK1, BackDoorK2, BackDoorK3
                DCB     BackDoorK4, BackDoorK5, BackDoorK6, BackDoorK7
                DCB     FPROT0,     FPROT1,     FPROT2,     FPROT3
                DCB     FSEC,       FOPT,       FEPROT,     FDPROT
                ENDIF

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  DMA0_DMA16_IRQHandler     [WEAK]
                EXPORT  DMA1_DMA17_IRQHandler     [WEAK]
                EXPORT  DMA2_DMA18_IRQHandler     [WEAK]
                EXPORT  DMA3_DMA19_IRQHandler     [WEAK]
                EXPORT  DMA4_DMA20_IRQHandler     [WEAK]
                EXPORT  DMA5_DMA21_IRQHandler     [WEAK]
                EXPORT  DMA6_DMA22_IRQHandler     [WEAK]
                EXPORT  DMA7_DMA23_IRQHandler     [WEAK]
                EXPORT  DMA8_DMA24_IRQHandler     [WEAK]
                EXPORT  DMA9_DMA25_IRQHandler     [WEAK]
                EXPORT  DMA10_DMA26_IRQHandler     [WEAK]
                EXPORT  DMA11_DMA27_IRQHandler     [WEAK]
                EXPORT  DMA12_DMA28_IRQHandler     [WEAK]
                EXPORT  DMA13_DMA29_IRQHandler     [WEAK]
                EXPORT  DMA14_DMA30_IRQHandler     [WEAK]
                EXPORT  DMA15_DMA31_IRQHandler     [WEAK]
                EXPORT  DMA_Error_IRQHandler     [WEAK]
                EXPORT  MCM_IRQHandler     [WEAK]
                EXPORT  FTFE_IRQHandler     [WEAK]
                EXPORT  Read_Collision_IRQHandler     [WEAK]
                EXPORT  LVD_LVW_IRQHandler     [WEAK]
                EXPORT  LLW_IRQHandler     [WEAK]
                EXPORT  Watchdog_IRQHandler     [WEAK]
                EXPORT  RNG_IRQHandler     [WEAK]
                EXPORT  I2C0_IRQHandler     [WEAK]
                EXPORT  I2C1_IRQHandler     [WEAK]
                EXPORT  SPI0_IRQHandler     [WEAK]
                EXPORT  SPI1_IRQHandler     [WEAK]
                EXPORT  SPI2_IRQHandler     [WEAK]
                EXPORT  CAN0_ORed_Message_buffer_IRQHandler     [WEAK]
                EXPORT  CAN0_Bus_Off_IRQHandler     [WEAK]
                EXPORT  CAN0_Error_IRQHandler     [WEAK]
                EXPORT  CAN0_Tx_Warning_IRQHandler     [WEAK]
                EXPORT  CAN0_Rx_Warning_IRQHandler     [WEAK]
                EXPORT  CAN0_Wake_Up_IRQHandler     [WEAK]
                EXPORT  I2S0_Tx_IRQHandler     [WEAK]
                EXPORT  I2S0_Rx_IRQHandler     [WEAK]
                EXPORT  CAN1_ORed_Message_buffer_IRQHandler     [WEAK]
                EXPORT  CAN1_Bus_Off_IRQHandler     [WEAK]
                EXPORT  CAN1_Error_IRQHandler     [WEAK]
                EXPORT  CAN1_Tx_Warning_IRQHandler     [WEAK]
                EXPORT  CAN1_Rx_Warning_IRQHandler     [WEAK]
                EXPORT  CAN1_Wake_Up_IRQHandler     [WEAK]
                EXPORT  Reserved59_IRQHandler     [WEAK]
                EXPORT  UART0_LON_IRQHandler     [WEAK]
                EXPORT  UART0_RX_TX_IRQHandler     [WEAK]
                EXPORT  UART0_ERR_IRQHandler     [WEAK]
                EXPORT  UART1_RX_TX_IRQHandler     [WEAK]
                EXPORT  UART1_ERR_IRQHandler     [WEAK]
                EXPORT  UART2_RX_TX_IRQHandler     [WEAK]
                EXPORT  UART2_ERR_IRQHandler     [WEAK]
                EXPORT  UART3_RX_TX_IRQHandler     [WEAK]
                EXPORT  UART3_ERR_IRQHandler     [WEAK]
                EXPORT  UART4_RX_TX_IRQHandler     [WEAK]
                EXPORT  UART4_ERR_IRQHandler     [WEAK]
                EXPORT  UART5_RX_TX_IRQHandler     [WEAK]
                EXPORT  UART5_ERR_IRQHandler     [WEAK]
                EXPORT  ADC0_IRQHandler     [WEAK]
                EXPORT  ADC1_IRQHandler     [WEAK]
                EXPORT  CMP0_IRQHandler     [WEAK]
                EXPORT  CMP1_IRQHandler     [WEAK]
                EXPORT  CMP2_IRQHandler     [WEAK]
                EXPORT  FTM0_IRQHandler     [WEAK]
                EXPORT  FTM1_IRQHandler     [WEAK]
                EXPORT  FTM2_IRQHandler     [WEAK]
                EXPORT  CMT_IRQHandler     [WEAK]
                EXPORT  RTC_IRQHandler     [WEAK]
                EXPORT  RTC_Seconds_IRQHandler     [WEAK]
                EXPORT  PIT0_IRQHandler     [WEAK]
                EXPORT  PIT1_IRQHandler     [WEAK]
                EXPORT  PIT2_IRQHandler     [WEAK]
                EXPORT  PIT3_IRQHandler     [WEAK]
                EXPORT  PDB0_IRQHandler     [WEAK]
                EXPORT  USB0_IRQHandler     [WEAK]
                EXPORT  USBDCD_IRQHandler     [WEAK]
                EXPORT  ENET_1588_Timer_IRQHandler     [WEAK]
                EXPORT  ENET_Transmit_IRQHandler     [WEAK]
                EXPORT  ENET_Receive_IRQHandler     [WEAK]
                EXPORT  ENET_Error_IRQHandler     [WEAK]
                EXPORT  Reserved95_IRQHandler     [WEAK]
                EXPORT  SDHC_IRQHandler     [WEAK]
                EXPORT  DAC0_IRQHandler     [WEAK]
                EXPORT  DAC1_IRQHandler     [WEAK]
                EXPORT  TSI0_IRQHandler     [WEAK]
                EXPORT  MCG_IRQHandler     [WEAK]
                EXPORT  LPTimer_IRQHandler     [WEAK]
                EXPORT  Reserved102_IRQHandler     [WEAK]
                EXPORT  PORTA_IRQHandler     [WEAK]
                EXPORT  PORTB_IRQHandler     [WEAK]
                EXPORT  PORTC_IRQHandler     [WEAK]
                EXPORT  PORTD_IRQHandler     [WEAK]
                EXPORT  PORTE_IRQHandler     [WEAK]
                EXPORT  PORTF_IRQHandler     [WEAK]
                EXPORT  DDR_IRQHandler     [WEAK]
                EXPORT  SWI_IRQHandler     [WEAK]
                EXPORT  NFC_IRQHandler     [WEAK]
                EXPORT  USBHS_IRQHandler     [WEAK]
                EXPORT  LCD_IRQHandler     [WEAK]
                EXPORT  CMP3_IRQHandler     [WEAK]
                EXPORT  Reserved115_IRQHandler     [WEAK]
                EXPORT  Reserved116_IRQHandler     [WEAK]
                EXPORT  FTM3_IRQHandler     [WEAK]
                EXPORT  ADC2_IRQHandler     [WEAK]
                EXPORT  ADC3_IRQHandler     [WEAK]
                EXPORT  I2S1_Tx_IRQHandler     [WEAK]
                EXPORT  I2S1_Rx_IRQHandler     [WEAK]
                EXPORT  DefaultISR                      [WEAK]

DMA0_DMA16_IRQHandler
DMA1_DMA17_IRQHandler
DMA2_DMA18_IRQHandler
DMA3_DMA19_IRQHandler
DMA4_DMA20_IRQHandler
DMA5_DMA21_IRQHandler
DMA6_DMA22_IRQHandler
DMA7_DMA23_IRQHandler
DMA8_DMA24_IRQHandler
DMA9_DMA25_IRQHandler
DMA10_DMA26_IRQHandler
DMA11_DMA27_IRQHandler
DMA12_DMA28_IRQHandler
DMA13_DMA29_IRQHandler
DMA14_DMA30_IRQHandler
DMA15_DMA31_IRQHandler
DMA_Error_IRQHandler
MCM_IRQHandler
FTFE_IRQHandler
Read_Collision_IRQHandler
LVD_LVW_IRQHandler
LLW_IRQHandler
Watchdog_IRQHandler
RNG_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
CAN0_ORed_Message_buffer_IRQHandler
CAN0_Bus_Off_IRQHandler
CAN0_Error_IRQHandler
CAN0_Tx_Warning_IRQHandler
CAN0_Rx_Warning_IRQHandler
CAN0_Wake_Up_IRQHandler
I2S0_Tx_IRQHandler
I2S0_Rx_IRQHandler
CAN1_ORed_Message_buffer_IRQHandler
CAN1_Bus_Off_IRQHandler
CAN1_Error_IRQHandler
CAN1_Tx_Warning_IRQHandler
CAN1_Rx_Warning_IRQHandler
CAN1_Wake_Up_IRQHandler
Reserved59_IRQHandler
UART0_LON_IRQHandler
UART0_RX_TX_IRQHandler
UART0_ERR_IRQHandler
UART1_RX_TX_IRQHandler
UART1_ERR_IRQHandler
UART2_RX_TX_IRQHandler
UART2_ERR_IRQHandler
UART3_RX_TX_IRQHandler
UART3_ERR_IRQHandler
UART4_RX_TX_IRQHandler
UART4_ERR_IRQHandler
UART5_RX_TX_IRQHandler
UART5_ERR_IRQHandler
ADC0_IRQHandler
ADC1_IRQHandler
CMP0_IRQHandler
CMP1_IRQHandler
CMP2_IRQHandler
FTM0_IRQHandler
FTM1_IRQHandler
FTM2_IRQHandler
CMT_IRQHandler
RTC_IRQHandler
RTC_Seconds_IRQHandler
PIT0_IRQHandler
PIT1_IRQHandler
PIT2_IRQHandler
PIT3_IRQHandler
PDB0_IRQHandler
USB0_IRQHandler
USBDCD_IRQHandler
ENET_1588_Timer_IRQHandler
ENET_Transmit_IRQHandler
ENET_Receive_IRQHandler
ENET_Error_IRQHandler
Reserved95_IRQHandler
SDHC_IRQHandler
DAC0_IRQHandler
DAC1_IRQHandler
TSI0_IRQHandler
MCG_IRQHandler
LPTimer_IRQHandler
Reserved102_IRQHandler
PORTA_IRQHandler
PORTB_IRQHandler
PORTC_IRQHandler
PORTD_IRQHandler
PORTE_IRQHandler
PORTF_IRQHandler
DDR_IRQHandler
SWI_IRQHandler
NFC_IRQHandler
USBHS_IRQHandler
LCD_IRQHandler
CMP3_IRQHandler
Reserved115_IRQHandler
Reserved116_IRQHandler
FTM3_IRQHandler
ADC2_IRQHandler
ADC3_IRQHandler
I2S1_Tx_IRQHandler
I2S1_Rx_IRQHandler
DefaultISR

                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

;                IF      :DEF:__MICROLIB

;                EXPORT  __initial_sp
;                EXPORT  __heap_base
;                EXPORT  __heap_limit

;                ELSE

;                IMPORT  __use_two_region_memory
;                EXPORT  __user_initial_stackheap
;__user_initial_stackheap

;                LDR     R0, =  Heap_Mem
;                LDR     R1, =(Stack_Mem + Stack_Size)
;                LDR     R2, = (Heap_Mem +  Heap_Size)
;                LDR     R3, = Stack_Mem
;                BX      LR

;                ALIGN

;                ENDIF


                END

/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : IntBIOS.h
***********************************************************************/

#pragma once

#include "config.h"
#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif

//============================= Flash space allocation =================================

#define BIN_BASE                ((u32)(0x0802C000)) // Size < 68KB
#define PRM_BASE                BIN_BASE + 68*1024  // Size =  2KB
#define INF_BASE                BIN_BASE + 70*1024  // Size < 10KB
#define APP4_BASE               ((u32)(0x08024000)) // Size = 32KB
#define APP3_BASE               ((u32)(0x0801C000)) // Size = 32KB
#define APP2_BASE               ((u32)(0x08014000)) // Size = 32KB
#define APP1_BASE               ((u32)(0x0800C000)) // Size = 32KB
#define SYS_BASE                ((u32)(0x08004000)) // Size = 32KB
#define DFU_BASE                ((u32)(0x08000000)) // Size = 16KB

//====================== Function defined in the Set Object and Value ==================

#define CH_A_OFFSET       0     // A channel vertical displacement Value = 0 ~ 200
#define CH_B_OFFSET       1     // B-channel vertical displacement Value = 0 ~ 200
#define BACKLIGHT         2     // Backlight Brightness Value = 0 ~ 100
#define BEEP_VOLUME       3     // Buzzer Volume Value = 0 ~ 100
#define BETTERY_DT        4     // battery voltage detection Value = 1: start
#define ADC_MODE          5     // ADC operating mode Value = 1 / 0
#define FIFO_CLR          6     // FIFO Pointer Reset Value = 1 / 0: W_PTR / R_PTR
//#define R_PTR             0     // FIFO read address pointer is reset ( Note: Seems to be used for something else in FPGA V2.50)
#define W_PTR           1       // FIFO write & read address pointer is reset
#define T_BASE_PSC        7     // Time Base Prescale Value = 0 ~ 65535
#define T_BASE_ARR        8     // time base divider value Value = 0 ~ 65535
#define CH_A_COUPLE       9     // A channel coupling Value = 1 / 0: AC / DC
#define COUPLE_DC              0
#define COUPLE_AC              1
#define CH_A_RANGE       10     // A channel input range Value = 0 ~ 5
#define CH_B_COUPLE      11     // B-channel coupling Value = 1 / 0: AC / DC
#define CH_B_RANGE       12     // B channel input range Value = 0 ~ 5
#define ANALOG_ARR       13     // analog output divider value Value = 0 ~ 65535
#define ANALOG_PTR       14     // analog output pointer Value = 0 ~ 65535
#define ANALOG_CNT       15     // number of points per cycle synthesis Value = 0 ~ 65535
#define DIGTAL_PSC       16     // pulse output prescaler Value = 0 ~ 65535
#define DIGTAL_ARR       17     // pulse output divider value Value = 0 ~ 65535
#define DIGTAL_CCR       18     // pulse output duty cycle value Value = 0 ~ 65535
#define KEY_IF_RST       19     // timer interrupt flag is reset Value = 0
#define STANDBY          20     // waiting to enter the power-down Value = 0
#define FPGA_RST         31     // FPGA Reset Value = 0

#define TRIGG_MODE       32+0  // trigger mode Value = Mode
#define V_THRESHOLD      32+1  // voltage trigger threshold Value = 0 ~ 200
#define T_THRESHOLD      32+2  // pulse width trigger time threshold Value = 0 ~ 65535
#define ADC_CTRL         32+4  // ADC work status Set Value = 1 / 0 EN / DN
#define A_POSITION       32+5  // CH_A zero point Value = 0 ~ 200
#define B_POSITION       32+6  // CH_B zero point Value = 0 ~ 200
#define REG_ADDR         32+7  // the address determines which set of registers in the FPGA the data read by the MCU

/* Input channel resolutions */
#define ADC_50mV 0
#define ADC_100mV 1
#define ADC_200mV 2
#define ADC_500mV 3
#define ADC_1V 4
#define ADC_2V 5
#define ADC_5V 6
#define ADC_10V 7
#define ADC_RANGE_COUNT 8

//==================== Set the function defined in TRIGG_MODE =====================
/*
CH_A Trigger source & kind select =>
0x00: by Negedge;   0x01: by Posedge;   0x02: by low level; 0x03: by high level
0x04: TL < Delta_T; 0x05: TL > Delta_T; 0x06: TH < Delta_T; 0x07: TH > Delta_T;

CH_B Trigger source & kind select =>
0x08: by Negedge;   0x09: by Posedge;   0x0A: by low level; 0x0B: by high level
0x0C: TL < Delta_T; 0x0D: TL > Delta_T; 0x0E: TH < Delta_T; 0x0F: TH > Delta_T;

CH_C Trigger source & kind select =>
0x10: by Negedge;   0x11: by Posedge;   0x12: by low level; 0x13: by high level
0x04: TL < Delta_T; 0x05: TL > Delta_T; 0x06: TH < Delta_T; 0x07: TH > Delta_T;

CH_D Trigger source & kind select =>
0x18: by Negedge;   0x19: by Posedge;   0x1A: by low level; 0x1B: by high level
0x1C: TL < Delta_T; 0x1D: TL > Delta_T; 0x1E: TH < Delta_T; 0x1F: TH > Delta_T;

0x20~0xFF  =>  Unconditional trigger
*/
#define UNCONDITION       0x20        // Unconditional trigger sampling

//================ Function Set the Value in the definition ADC_CTRL & STANDBY ================

#define DN            0
#define EN            1

//===================== Set the Value function defined in ADC_MODE =====================

#define SEPARATE      0    // ADC independent sampling mode
#define INTERLACE     1    // ADC Alternate Sampling mode

//========================= Get the Kind in the definition of the function ============================

#define FIFO_DIGIT             0    // 16bits FIFO digital data
#define FIFO_EMPTY             1    // FIFO empty flag: 1 = empty
#define FIFO_START             2    // FIFO start flag: 1 = start
#define FIFO_FULL              3    // FIFO full flag: 1 = Full
#define KEY_STATUS             4    // Current keys status
  #define K_ITEM_D_STATUS      0x0008    // 0 = Key push on
  #define K_ITEM_S_STATUS      0x0040    // 0 = Key push on
  #define KEY3_STATUS          0x0100    // 0 = Key push on
  #define KEY4_STATUS          0x0200    // 0 = Key push on
  #define K_INDEX_D_STATUS     0x0400    // 0 = Key push on
  #define K_INDEX_I_STATUS     0x0800    // 0 = Key push on
  #define K_INDEX_S_STATUS     0x1000    // 0 = Key push on
  #define KEY2_STATUS          0x2000    // 0 = Key push on
  #define KEY1_STATUS          0x4000    // 0 = Key push on
  #define K_ITEM_I_STATUS      0x8000    // 0 = Key push on
  #define ALL_KEYS 0xFF48
#define USB_POWER              5    // USB power status: 1 = Power ON
#define V_BATTERY              6    // Battery voltage (mV)
#define VERTICAL               7    // pointer to the vertical channel properties
#define HORIZONTAL             8    // pointer to the level of channel properties
#define GLOBAL                 9    // pointer to the overall properties
#define TRIGGER                10   // pointer to trigger channel properties
#define FPGA_OK                11   // FPGA configuration is successful 1 = FPGA config ok
#define CHARGE         12       // battery charge status
#define HDWVER         13       // device hardware version
#define DFUVER         14       // DFU version number of program modules
#define SYSVER         15       // SYS version number of program modules
#define FPGAVER        16       // FPGA configuration program version number

#define ADC_DATA       32+0     // 0~7:ADC_CH_A 8~15:ADC_CH_B 16~17:CH_C&CH_D
#define PRE_SAMPLING   32+1     // 0~15:Pre-sampling depth
#define ALL_SAMPLING   32+2     // 0~15:Total sampling depth
#define CH_A_MIN_MAX   32+3     // 0~7:VMIN 8~15:VMAX
#define CH_A_V_SUM     32+4     // 0~15:CH_A voltage sum
#define CH_A_V_SSQ     32+5     // 0~15:CH_A voltage sum of squares
#define CH_A_NEDGE     32+6     // 0~15:CH_A number of edge
#define CH_A_FREQ      32+7     // 0~15:CH_A frequence
#define CH_A_PHW_MAX   32+8     // 0~15:CH_A pulse high width MAX
#define CH_A_PHW_MIN   32+9     // 0~15:CH_A pulse high width MIN
#define CH_A_PLW_MAX   32+10    // 0~15:CH_A pulse low width MAX
#define CH_A_PLW_MIN   32+11    // 0~15:CH_A pulse low width MIN

// =============================================================================

typedef struct  // hardware integrated properties
{
  unsigned short LCD_X;    // screen horizontal pixels
  unsigned short LCD_Y;    // screen vertical pixels
  unsigned short Yp_Max;   // vertical scale maximum
  unsigned short Xp_Max;   // maximum level stalls
  unsigned short Tg_Num;   // trigger the maximum stall
  unsigned short Yv_Max;   // maximum vertical displacement
  unsigned short Xt_Max;   // maximum horizontal displacement
  unsigned short Co_Max;   // maximum coupling
  unsigned char  Ya_Num;   // the number of analog channels
  unsigned char  Yd_Num;   // the number of digital channels
  unsigned char  INSERT;   // Start application gear interpolation
  unsigned short KpA1;     // A channel shift compensation factor 1
  unsigned short KpA2;     // A channel shift compensation factor 2
  unsigned short KpB1;     // B channel compensation coefficient of an offset
  unsigned short KpB2;     // B channel compensation coefficient 2 displacement
} G_attr ;

typedef struct  // vertical channel properties
{
  char STR[8];  // stall identification string
  short KA1;      // A channel displacement error correction factor 1
  unsigned short KA2;      // A channel slope error correction factor 2
  short KB1;      // B channel displacement error correction factor 1
  unsigned short KB2;      // B channel slope error correction factor 2
  unsigned int SCALE;    // vertical channel scale factor
} Y_attr ;

typedef struct  // horizontal channel properties
{
  char STR[8];  // stall identification string
  short PSC;      // prescaler
  unsigned short ARR;      // frequency coefficient
  unsigned short CCR;      // duty factor
  unsigned short KP;       // interpolation factor
  unsigned int SCALE;    // horizontal channel scale factor
} X_attr ;

typedef struct  // trigger the channel properties
{
  unsigned char  STR[8];   // trigger identification string
  unsigned char  CHx;      // trigger the channel number
  unsigned char  CMD;      // trigger control word
} T_attr ;

extern Y_attr *Y_Attr;
extern X_attr *X_Attr;
extern G_attr *G_Attr;
extern T_attr *T_Attr;

// CPU clock frequency is 72 MHz.
#define CPUFREQ 72000000

// Font size
#define FONT_HEIGHT 14
#define FONT_WIDTH 8

//==============================================================================
//                        System function entrance
//==============================================================================
 void __CTR_HP(void);     //USB_HP_Interrupt
 void __USB_Istr(void);   //USB_LP_Interrupt
 void __USB_Init(void);

 void __LCD_Initial(void);
 void __Clear_Screen(unsigned short Color);
 void __Point_SCR(unsigned short x0, unsigned short y0);
 void __LCD_SetPixl(unsigned short Color);
 unsigned short  __LCD_GetPixl(void);
 unsigned short  __Get_TAB_8x14(unsigned char Code, unsigned short Row);
 void __LCD_Set_Block(unsigned short x1, unsigned short x2, unsigned short y1, unsigned short y2);

 void __LCD_Copy(unsigned short const *pBuffer, unsigned short  NumPixel); // Send a row data to LCD
 void __LCD_Fill(unsigned short *pBuffer,unsigned short NumPixel);    // Fill number of pixel by DMA
 void __LCD_DMA_Ready(void);                    // Wait LCD data DMA ready

 void __Row_Copy(unsigned short const *S_Buffer,unsigned short *T_Buffer); // Copy one row base data to buffer
 void __Row_DMA_Ready(void);                    // Wait row base data DMA ready

 unsigned int  __Read_FIFO(void);                        // Read data from FIFO & Ptr+1
 void __Display_Str(unsigned short x0, unsigned short y0, unsigned short Color, unsigned char Mode, char const *s);

 unsigned int  __Input_Lic(unsigned short x0, unsigned char y0);               //Return: 32Bits Licence
 unsigned int  __GetDev_SN(void);                        // Get 32bits Device Serial Number

 unsigned char   __Ident(unsigned int Dev_ID, unsigned int Proj_ID, unsigned int Lic_No);

 void __Set(unsigned char Object, unsigned int Value);
 unsigned int  __Get(unsigned char Object, unsigned int Value);

 void __ExtFlash_PageRD(unsigned char *pBuffer, unsigned int ReadAddr, unsigned short NumByteToRead);
 void __ExtFlash_PageWR(unsigned char *pBuffer, unsigned int WriteAddr);
 unsigned char   __ReadDiskData(unsigned char *pBuffer, unsigned int ReadAddr, unsigned short Lenght);
 unsigned char   __ProgDiskPage(unsigned char *pBuffer, unsigned int ProgAddr);
// u8   __ClashSt(void);
// void __ClashClr(void);

 unsigned char   __FLASH_Erase(unsigned int Address);
 unsigned char   __FLASH_Prog(unsigned int Address, unsigned short Data);
 void __FLASH_Unlock(void);
 void __FLASH_Lock(void);

 unsigned char   __Chk_SYS(unsigned int Licence);
 unsigned char * __Chk_DFU(void);
 unsigned char * __Chk_HDW(void);

 // Note: filename should be 11-byte char array, like "FILE0001CSV"
 unsigned char __OpenFileWr(unsigned char *Buffer, char *FileName, unsigned short *Cluster, unsigned int *pDirAddr);
 unsigned char __OpenFileRd(unsigned char *Buffer, char *FileName, unsigned short *Cluster, unsigned int *pDirAddr);
 unsigned char __ReadFileSec(unsigned char *Buffer, unsigned short *Cluster);
 unsigned char __ProgFileSec(unsigned char *Buffer, unsigned short *Cluster);
 unsigned char __CloseFile(unsigned char *Buffer, unsigned int Lenght, unsigned short *Cluster, unsigned int *pDirAddr);
/**/

#ifdef __cplusplus
}
#endif

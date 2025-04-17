#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#ifndef LMX2820_H
#define LMX2820_H



//#undef _WIN32   // uncomment for general library usage, firmware or Windoze
#define DLL_IMPLEMENTATION  // comment out for dll_import usage

#ifdef _WIN32
  #ifdef DLL_IMPLEMENTATION
    #define DLL_MODIFIER __declspec(dllexport)
  #else
    #define DLL_MODIFIER __declspec(dllimport)
  #endif
#else
  // define empty to remove dllexport parameter for general library usage
  #define DLL_MODIFIER
#endif


#ifdef __cplusplus
extern "C" {
#endif





/*  define struct for 123x3 2D array "matrix"; use maximum registers, in case of all readbacks (R0<-->R122)   */
typedef struct {
  unsigned char numberRegisters;
  // avoiding pointers and malloc, calloc, free(), etc
  unsigned char dataBytes[123][3];
} Registers;


/***    debug helper functions     ***/
DLL_MODIFIER int add(int a, int b);   // "sanity check" example
DLL_MODIFIER const char *get_DLL_Version();
DLL_MODIFIER Registers get_Default_Registers();



/***    Global Variables Functions    ***/
// (7.6.2) Input Path Registers, reference clock crystal oscillator
DLL_MODIFIER void set_fOSC_freq(double new_fOSC_GHz);
DLL_MODIFIER double get_fOSC_freq();
// set new PLL_DEN, higher value equals smaller frequency of Delta-Sigma steps (max = 2^32-1, 0xFFFFFFFF)
DLL_MODIFIER void set_PLL_DEN(unsigned int pll_den);
DLL_MODIFIER unsigned int get_PLL_DEN();



/***  General Registers R0, R19, R70 (R/W)    ***/
//  R0: POWERDOWN, RESET, FCAL_EN, DBLR_CAL_EN, INSTCAL_SKIP_ACAL
typedef enum {POWERDOWN, RESET, FCAL_EN, DBLR_CAL_EN, INSTCAL_SKIP_ACAL} R0_fields;
//  FCAL_LPFD_ADJ, FCAL_HPFD_ADJ calculated automatically according to f_PD, f_OSC and Input Registers (Table 1-3)
DLL_MODIFIER void get_R0_register(unsigned char R0_reg[], R0_fields field, unsigned char value);
DLL_MODIFIER void calc_R0_register(unsigned char R0_reg[],
                                    int *POWERDOWN,
                                    int *RESET,
                                    int *FCAL_EN,
                                    int *FCAL_LPFD_ADJ,
                                    int *FCAL_HPFD_ADJ,
                                    int *DBLR_CAL_EN,
                                    int *INSTCAL_SKIP_ACAL);


/***    Input Path Registers (R/W)    ***/
DLL_MODIFIER void calc_R11_register(unsigned char R11_reg[], int *OSC_2X);
DLL_MODIFIER void calc_R12_register(unsigned char R12_reg[], int *MULT);
DLL_MODIFIER void calc_R13_register(unsigned char R13_reg[], int *PLL_R);
DLL_MODIFIER void calc_R14_register(unsigned char R14_reg[], int *PLL_R_PRE);
DLL_MODIFIER Registers get_Input_Path_registers(unsigned char R0_reg[],
                                                  bool OSC_2X,
                                                  unsigned char MULT,
                                                  unsigned char PLL_R,
                                                  int PLL_R_PRE);


/***  Charge Pump Register (R16) (R/W)    ***/
DLL_MODIFIER void get_R16_register(unsigned char R16_reg[], unsigned char CPG);
DLL_MODIFIER void calc_R16_register(unsigned char R16_reg[], int *CPG);


/***  VCO Calibration Registers (R/W)     ***/
//  R1: INSTCAL_EN, INSTCAL_DBLR_EN, LD_VTUNE_EN, PHASE_SYNC_EN
typedef enum {INSTCAL_EN, INSTCAL_DBLR_EN, LD_VTUNE_EN, PHASE_SYNC_EN} R1_fields;
DLL_MODIFIER void get_R1_register(unsigned char R1_reg[], R1_fields field, unsigned char value);
DLL_MODIFIER void calc_R1_register(unsigned char R1_reg[], int *INSTCAL_EN, int *INSTCAL_DBLR_EN, int *LD_VTUNE_EN, int *PHASE_SYNC_EN);


/***  Lock Detect Registers (R/W)   ***/
DLL_MODIFIER void get_R17_register(unsigned char R18_reg[], int LD_TYPE);
DLL_MODIFIER void calc_R17_register(unsigned char R18_reg[], int *LD_TYPE);
DLL_MODIFIER void get_R18_register(unsigned char R18_reg[], int LD_DLY);
DLL_MODIFIER void calc_R18_register(unsigned char R18_reg[], int *LD_DLY);


/***  N Divider, PLL Registers (R/W)  ***/
//  R35: MASHSEED_EN, MASH_ORDER, MASH_RESET_N
typedef enum {MASHSEED_EN, MASH_ORDER, MASH_RESET_N} R35_fields;
DLL_MODIFIER void get_R35_register(unsigned char R35_reg[], R35_fields field, unsigned char value);
DLL_MODIFIER void calc_R35_register(unsigned char R35_reg[], int *MASHSEED_EN, int *MASH_ORDER, int *MASH_RESET_N);


/***  Outputs Registers    ***/
//  R32: CHDIVA, CHDIVB
typedef enum {CHDIVA, CHDIVB} R32_fields;
DLL_MODIFIER void get_R32_register(unsigned char R32_reg[],  R32_fields field, unsigned char value);
DLL_MODIFIER void calc_R32_register(unsigned char R32_reg[], int *CHDIVA, int *CHDIVB);
//  R77: PINMUTE_POL
DLL_MODIFIER void get_R77_register(unsigned char R77_reg[], unsigned char PINMUTE_POL);
DLL_MODIFIER void calc_R77_register(unsigned char R77_reg[], unsigned char *PINMUTE_POL);
//  R78: OUTA_MUX, OUTA_PD
typedef enum {OUTA_MUX, OUTA_PD} R78_fields;
DLL_MODIFIER void get_R78_register(unsigned char R78_reg[],  R78_fields field, unsigned char value);
DLL_MODIFIER void calc_R78_register(unsigned char R78_reg[], int *OUTA_MUX, int *OUTA_PD);
//  R79: OUTA_PWR, OUTB_MUX, OUTB_PD
typedef enum {OUTA_PWR, OUTB_MUX, OUTB_PD} R79_fields;
DLL_MODIFIER void get_R79_register(unsigned char R79_reg[],  R79_fields field, unsigned char value);
DLL_MODIFIER void calc_R79_register(unsigned char R79_reg[], int *OUTA_PWR, int *OUTB_MUX, int *OUTB_PD);
//  R80: OUTB_PWR
DLL_MODIFIER void get_R80_register(unsigned char R80_reg[], unsigned char OUTB_PWR);
DLL_MODIFIER void calc_R80_register(unsigned char R80_reg[], int *OUTB_PWR);


/***  Readback Registers  (read-only)    ***/
DLL_MODIFIER void calc_R74_register(unsigned char R74_reg[], int *rb_VCO_SEL, int *rb_VCO_CAPCTRL, int *rb_LD);
DLL_MODIFIER void calc_R75_register(unsigned char R75_reg[], int *rb_VCO_DACISET);
DLL_MODIFIER void calc_R76_register(unsigned char R76_reg[], int *rb_TEMP_SENS);
DLL_MODIFIER double get_TEMP_SENS(unsigned char R76_reg[]);


/***  (7.3.7) Frequency functions    ***/
// return pll_N, pll_DEN, pll_NUM, registers; calculates R78: OUTA_MUX, R1: INSTCAL_DBLR_EN, R32: CHDIVA for calling SPI function
DLL_MODIFIER Registers get_Frequency_Registers(double new_freq, unsigned char MASH_ORDER, int *OUTA_MUX, bool *DBLR_CAL_EN, bool *INSTCAL_DBLR_EN, int *CHDIVA);





#ifdef __cplusplus
}
#endif

#endif


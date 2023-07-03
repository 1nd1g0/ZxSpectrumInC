// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /zxspectrum.h
*
* (copyleft)2023 1nd1g0
* Fast vector graphics test for ZX Spectrum 48K
*
* Targets SDCC, being ported to C89 compilers, so it
* ASSUMES: No external code, TRUE = 1.
*   short = int16, char = int8, >> duplicates sign bit
*
*
*****************************************************/

/* SDCC Z80 main data types */
typedef unsigned short size_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
typedef   signed char   int8_t;

/* SDCC specific Z80 extensions wrappers */
#define PORT(p) volatile __sfr __banked __at (p)
#define MEMADR(m) __at (m)

/* Platform specific */

/* BORDER HW */
  enum {Black,Blue,Red,Magenta,Green,Cyan,Yellow,White} zx_colors;
  PORT(0xFFFE) BORDER;

/* VRAM HW */
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192
#define ATTRS_HEIGHT 24
#define ATTRS_WIDTH 32

  uint8_t MEMADR(0x4000) SCREEN[SCREEN_HEIGHT][ATTRS_WIDTH];
  uint8_t MEMADR(0x5800)  ATTRS[ATTRS_HEIGHT] [ATTRS_WIDTH];

/*  Colour attribute pallette without flash bit variants.
    Single capital letters encode Brightness + Paper + Ink.
    blac_K,B_lue,R_ed,M_agenta,G_reen,C_yan,Y_ellow,W_hite */
  enum { 
     KK, KB, KR, KM, KG, KC, KY, KW,
     BK, BB, BR, BM, BG, BC, BY, BW,
     RK, RB, RR, RM, RG, RC, RY, RW,
     MK, MB, MR, MM ,MG ,MC, MY, MW,
     GK, GB, GR, GM, GG, GC, GY, GW,
     CK, CB, CR, CM, CG, CC, CY, CW,
     YK, YB, YR, YM, YG, YC, YY, YW,
     WK, WB, WR, WM, WG, WC, WY, WW,
    BKK,BKB,BKR,BKM,BKG,BKC,BKY,BKW,
    BBK,BBB,BBR,BBM,BBG,BBC,BBY,BBW,
    BRK,BRB,BRR,BRM,BRG,BRC,BRY,BRW,
    BMK,BMB,BMR,BMM,BMG,BMC,BMY,BMW,
    BGK,BGB,BGR,BGM,BGG,BGC,BGY,BGW,
    BCK,BCB,BCR,BCM,BCG,BCC,BCY,BCW,
    BYK,BYB,BYR,BYM,BYG,BYC,BYY,BYW,
    BWK,BWB,BWR,BWM,BWG,BWC,BWY,BWW
  };

/* KEYBOARD HW */
  PORT(0x00FE) ROW_ANY;  enum {KEY_ANY=31};
  PORT(0xF7FE) ROW_15;   enum {KEY_5=16,KEY_4=8,KEY_3=4,KEY_2=2,KEY_1=1};
  PORT(0xEFFE)   ROW_60; enum {KEY_6=16,KEY_7=8,KEY_8=4,KEY_9=2,KEY_0=1};
  PORT(0xFBFE) ROW_QT;   enum {KEY_T=16,KEY_R=8,KEY_E=4,KEY_W=2,KEY_Q=1};
  PORT(0xDFFE)   ROW_YP; enum {KEY_Y=16,KEY_U=8,KEY_I=4,KEY_O=2,KEY_P=1};
  PORT(0xFDFE) ROW_AG;   enum {KEY_G=16,KEY_F=8,KEY_D=4,KEY_S=2,KEY_A=1};
  PORT(0xBFFE)   ROW_HEN;enum {KEY_H=16,KEY_J=8,KEY_K=4,KEY_L=2,KEY_EN=1};
  PORT(0xFEFE) ROW_SHV;  enum {KEY_V=16,KEY_C=8,KEY_X=4,KEY_Z=2,KEY_SH=1};
  PORT(0x7FFE)   ROW_BSP;enum {KEY_B=16,KEY_N=8,KEY_M=4,KEY_SY=2,KEY_SP=1};

inline uint8_t ytolines (uint8_t y_screen_coordinate);
inline uint8_t key (uint8_t keyboard_row_port_address, uint8_t bitmask_of_keys);
void debug_step_on_key_s(void);
void pixels_fill (uint8_t rightmost_pixel_x, uint8_t pixel_line_y, uint8_t pixel_count);
uint8_t* bytes_fill (uint8_t* video_ram_address, uint8_t byte_count, uint8_t mask2fill_with);
void screen_clear (uint8_t filling_pixel_mask);
void attrs_clear (uint8_t filling_color_attribute);
void attrs_checkboard (void);

/* Portable functions */
uint8_t int8_abs(int8_t signed_number2process);


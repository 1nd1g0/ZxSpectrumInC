// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /zxspectrum.c
*
* ZX SPECTRUM 48K HARDWARE ABSTRACTION LAYER
*
* (copyleft)2023 1nd1g0
* Fast vector graphics test for ZX Spectrum 48K
*
* Targets SDCC, being ported to C89 compilers, so it
* ASSUMES: No external code, TRUE = 1.
*   short = int16, char = int8, >> duplicates sign bit
*
*****************************************************/

//#link "zxspectrum.c"

/* BORDER HW */
  enum {Black,Blue,Red,Magenta,Green,Cyan,Yellow,White};
  volatile __sfr __at (0xFFFE) BORDER;

/* VRAM HW
    Colour attribute pallette without flash bit variants.
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
  uint8_t __at (0x4000) SCREEN[192][32];
  uint8_t __at (0x5800)  ATTRS[24] [32];

  inline uint8_t ytolines(uint8_t y){
    return (y & 0xc0)|((y & 7)<<3)|((y & 0x38)>>3);
  }

/* BIT TO BYTE BITMAPS */
  const uint8_t SolidPixelTailRight [8] = {0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF};
  const uint8_t SolidPixelTailLeft  [8] = {0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01};
  const uint8_t SolidPixelTailLeftM [8] = {0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};

/* KEYBOARD HW */
  volatile __sfr __banked __at (0x00FE) ROW_ANY;
  enum {KEY_ANY=0x1F};
  volatile __sfr __banked __at (0xF7FE) ROW_15;
  enum {KEY_5=0x10,KEY_4=8,KEY_3=4,KEY_2=2,KEY_1=1};
  volatile __sfr __banked __at (0xEFFE) ROW_60;
  enum {KEY_6=0x10,KEY_7=8,KEY_8=4,KEY_9=2,KEY_0=1};
  volatile __sfr __banked __at (0xFBFE) ROW_QT;
  enum {KEY_T=0x10,KEY_R=8,KEY_E=4,KEY_W=2,KEY_Q=1};
  volatile __sfr __banked __at (0xDFFE) ROW_YP;
  enum {KEY_Y=0x10,KEY_U=8,KEY_I=4,KEY_O=2,KEY_P=1};
  volatile __sfr __banked __at (0xFDFE) ROW_AG;
  enum {KEY_G=0x10,KEY_F=8,KEY_D=4,KEY_S=2,KEY_A=1};
  volatile __sfr __banked __at (0xBFFE) ROW_HEN;
  enum {KEY_H=0x10,KEY_J=8,KEY_K=4,KEY_L=2,KEY_EN=1};
  volatile __sfr __banked __at (0xFEFE) ROW_SHV;
  enum {KEY_V=0x10,KEY_C=8,KEY_X=4,KEY_Z=2,KEY_SH=1};
  volatile __sfr __banked __at (0x7FFE) ROW_BSP;
  enum {KEY_B=0x10,KEY_N=8,KEY_M=4,KEY_SY=2,KEY_SP=1};

  inline uint8_t key(uint8_t row,uint8_t mask){
    return (~row & mask) > 0;
  }

  void debug_step_on_key_s(void){
    while (!key(ROW_ANY,KEY_ANY));
    while (key(ROW_AG,KEY_S));
     __asm ei halt __endasm;
  }

uint8_t* bytes_fill (uint8_t* addr, uint8_t count, uint8_t mask){
  register uint8_t m = mask;
  register uint8_t* a = addr;
  switch (count) { /* TRICK: Duff's device in action */
    case 32: *a-- = m;case 31: *a-- = m;case 30: *a-- = m;case 29: *a-- = m;
    case 28: *a-- = m;case 27: *a-- = m;case 26: *a-- = m;case 25: *a-- = m;
    case 24: *a-- = m;case 23: *a-- = m;case 22: *a-- = m;case 21: *a-- = m;
    case 20: *a-- = m;case 19: *a-- = m;case 18: *a-- = m;case 17: *a-- = m;
    case 16: *a-- = m;case 15: *a-- = m;case 14: *a-- = m;case 13: *a-- = m;
    case 12: *a-- = m;case 11: *a-- = m;case 10: *a-- = m;case  9: *a-- = m;
    case  8: *a-- = m;case  7: *a-- = m;case  6: *a-- = m;case  5: *a-- = m;
    case  4: *a-- = m;case  3: *a-- = m;case  2: *a-- = m;case  1: *a-- = m;
  }
  return a;
}

void pixels_fill (uint8_t rx, uint8_t y, uint8_t count) {
  uint8_t* pen_address;
  static uint8_t last_y;
  
  #ifdef DEBUG
  debug_step_on_key_s();
  #endif
  //clip...
  if ((y > SCREEN_HEIGHT-1) || (!count) || (last_y == y))
    return; //...bottom
  last_y = y;  
  if (rx < (count-1))
    count = rx + 1; //...left
  pen_address = & SCREEN [ytolines(y)] [(rx >> 3)];
  
    if ((rx & 7) >= (count - 1)){
      *pen_address = SolidPixelTailRight[(rx & 7)]
                   & SolidPixelTailLeft [(rx & 7) - (count)];
    } else {
      *pen_address = SolidPixelTailRight[(rx & 7)];
      pen_address--;
      count -= (rx & 7) + 1;
      if (count>>3)
        pen_address = bytes_fill (pen_address,count>>3,0xFF);
      *pen_address = SolidPixelTailLeftM [((count)&7)];
    }

}
  

void screen_clear (uint8_t fill_mask){
  
  uint8_t* i = & SCREEN[191][31];
  
  while (i != (& SCREEN[0][0])-1) {
    i=bytes_fill(i,32,fill_mask);
  }  
}


void attrs_clear (uint8_t fill_attribute) {
 
  uint8_t* i = & ATTRS[23][31];
  while (i != & ATTRS[0][0]-1){
    i=bytes_fill(i,32,fill_attribute);
  }
  
}

void attrs_checkboard() {
  uint16_t i;
  uint8_t* p;
  
  p = & ATTRS[23][31];
  
  for (i=sizeof ATTRS;i>0;i--) {
    *p = *p ^ (( ((((uint16_t)p)>>5)&1)
            ^ ((     (uint8_t)p)&1))<<3);
    p--;
  }

}

/* Math */
uint8_t int8_abs(int8_t s){
  uint8_t const mask = s>>((sizeof s)*8-1);
  return (s + mask) ^ mask;
}


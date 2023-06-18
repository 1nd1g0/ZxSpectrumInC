// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /zxspectrum.c
*
* (copyleft)2023 1nd1g0
* Fast vector graphics test for ZX Spectrum 48K
*
* Targets SDCC, being ported to C89 compilers, so it
* ASSUMES: No (u)int##_t support, TRUE = 1.
*   short = int16, char = int8, >> duplicates sign bit
*
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
  enum {KK,KB,KR,KM,KG,KC,KY,KW,
        BK,BB,BR,BM,BG,BC,BY,BW,
        RK,RB,RR,RM,RG,RC,RY,RW,
        MK,MB,MR,MM,MG,MC,MY,MW,
        GK,GB,GR,GM,GG,GC,GY,GW,
        CK,CB,CR,CM,CG,CC,CY,CW,
        YK,YB,YR,YM,YG,YC,YY,YW,
        WK,WB,WR,WM,WG,WC,WY,WW,
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
  uint8_t __at (0x5800) ATTRS[24][32];

  inline uint8_t ytolines(uint8_t y){
    return (y & 0xc0)|((y & 7)<<3)|((y & 0x38)>>3);
  }

/* BIT TO BYTE BITMAPS */
  const uint8_t PointsR [8] = {0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF};
  const uint8_t PointsL [8] = {0xFF,0x7F,0x3F,0x1F,0xF,7,3,1};

/* KEYBOARD HW */
  inline uint8_t key(uint8_t row,uint8_t mask){
    return (~row&mask)>0;
  }
  volatile __sfr __banked __at (0x00FE) ROWANY;
  enum {KEYANY=0x1F};
  volatile __sfr __banked __at (0xF7FE) ROW15;
  enum {KEY5=0x10,KEY4=8,KEY3=4,KEY2=2,KEY1=1};
  volatile __sfr __banked __at (0xEFFE) ROW60;
  enum {KEY6=0x10,KEY7=8,KEY8=4,KEY9=2,KEY0=1};
  volatile __sfr __banked __at (0xFBFE) ROWQT;
  enum {KEYT=0x10,KEYR=8,KEYE=4,KEYW=2,KEYQ=1};
  volatile __sfr __banked __at (0xDFFE) ROWYP;
  enum {KEYY=0x10,KEYU=8,KEYI=4,KEYO=2,KEYP=1};
  volatile __sfr __banked __at (0xFDFE) ROWAG;
  enum {KEYG=0x10,KEYF=8,KEYD=4,ROW=2,KEYA=1};
  volatile __sfr __banked __at (0xBFFE) ROWHEN;
  enum {KEYH=0x10,KEYJ=8,KEYK=4,KEYL=2,KEYEN=1};
  volatile __sfr __banked __at (0xFEFE) ROWSHV;
  enum {KEYV=0x10,KEYC=8,KEYX=4,KEYZ=2,KEYSH=1};
  volatile __sfr __banked __at (0x7FFE) ROWBSP;
  enum {KEYB=0x10,KEYN=8,KEYM=4,KEYSY=2,KEYSP=1};

uint8_t* bytes_fill(uint8_t* addr, uint8_t count, uint8_t mask){
  register uint8_t m = mask;
  register uint8_t* a = addr;
  switch(count){ /* TRICK: Duff's device in action */
    case 32: *a-- = m;case 31: *a-- = m;case 30: *a-- = m;case 29: *a-- = m;
    case 28: *a-- = m;case 27: *a-- = m;case 26: *a-- = m;case 25: *a-- = m;
    case 24: *a-- = m;case 23: *a-- = m;case 22: *a-- = m;case 21: *a-- = m;
    case 20: *a-- = m;case 19: *a-- = m;case 18: *a-- = m;case 17: *a-- = m;
    case 16: *a-- = m;case 15: *a-- = m;case 14: *a-- = m;case 13: *a-- = m;
    case 12: *a-- = m;case 11: *a-- = m;case 10: *a-- = m;case 9: *a-- = m;
    case 8: *a-- = m;case 7: *a-- = m;case 6: *a-- = m;case 5: *a-- = m;
    case 4: *a-- = m;case 3: *a-- = m;case 2: *a-- = m;case 1: *a-- = m;
  }
  return a;
}

void screen_clear (uint8_t fill_mask, uint8_t fill_colors){
  uint8_t* i=&ATTRS[23][31];
  while (i!=&ATTRS[0][0]-1){
    i=bytes_fill(i,32,fill_colors);
  }
  
  while (i!=(&SCREEN[0][0])-1) {
    i=bytes_fill(i,32,fill_mask);
  }  
}

/* Math */
uint8_t abs(int8_t s){
  uint8_t const mask = s>>((sizeof s)*8-1);
  return (s+mask)^mask;
}

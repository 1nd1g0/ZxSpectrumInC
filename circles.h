// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /circles.h
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

/* Shape specific functions */

void bits_fill (uint8_t center_x, uint8_t center_y, uint8_t delta_x, int8_t delta_y);
void circle_paint (uint8_t center_x, uint8_t center_y, uint8_t radius);

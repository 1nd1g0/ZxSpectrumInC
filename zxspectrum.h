// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /zxspectrum.h
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

/* Z80 main data types */
#define uint16_t unsigned short
#define uint8_t unsigned char
#define int8_t signed char

#define SCREEN_HEIGHT 192
#define SCREEN_WIDTH 256

/* Platform specific functions */
inline uint8_t ytolines (uint8_t y_screen_coordinate);
inline uint8_t key (uint8_t keyboard_row_port_address, uint8_t bitmask_of_keys);
void pixels_fill (uint8_t rightmost_pixel_x, uint8_t pixel_line_y, uint8_t pixel_count);
uint8_t* bytes_fill (uint8_t* video_ram_address, uint8_t byte_count, uint8_t mask2fill_with);
void screen_clear (uint8_t filling_pixel_mask);
void attrs_clear (uint8_t filling_color_attribute);
void attrs_checkboard ();

/* Portable functions */
uint8_t int8_abs(int8_t signed_number2process);


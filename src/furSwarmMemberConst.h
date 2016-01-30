/*
 
 furSwarmMemberConst.h
 
 $Date: 2013-08-12 07:32:46 -0700 (Mon, 12 Aug 2013) $
 $Revision: 1073 $
 $Author: mauricio $
 
 Copyright (c) 2012, Mauricio Bustos
 All rights reserved.
 
 Redistribution and use in source and binary forms,
 with or without modification, are permitted provided
 that the following conditions are met:
 
 - Redistributions must not be for profit.  This software in source or binary form,
 with or without modification, cannot be included in anyproduct that is then sold.
 Please contact Mauricio Bustos m@bustos.org if you are interested in a for-profit license.
 - Redistributions of source code or in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef furSwarm_furSwarmMemberConst_h
#define furSwarm_furSwarmMemberConst_h

#include "furSwarmPatternConst.h"

#ifdef NOT_EMBEDDED
#define PROGMEM
typedef uint8_t PROGMEM prog_uint8_t;
uint8_t pgm_read_byte(uint8_t* address) {
    return *address;
}
typedef uint16_t PROGMEM prog_uint16_t;
uint8_t* pgm_read_word(uint8_t** address) {
    return *address;
}
#elif TEENSY
#else
#include <avr/pgmspace.h>
#include "Arduino.h"
#endif

const uint8_t LED_PANEL_SIZE = 10;
const uint8_t LED_BACK_ROWS = 6;
const uint8_t LED_BACK_COLUMNS = 5;
const uint8_t LED_PANEL_COLUMNS = 2;
/*
 Row / Column to LED mapping
 This table provides the mapping between the row column mapping for the
 various pattern specifications and the TCL LED position. 0xFF is used as the don't care.
 */
#ifdef FS_TOWER_EYE
const uint8_t LED_BYTEMAP = 117;  // Number of bytes using the row / column scheme (2 don't care bytes)
PROGMEM prog_uint8_t rcLEDMap[] = {
    0xff, 0xff, 0x03, 0x02, 0x01, 0x00, 0xff, 0xff, 0xff,
    0xff, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0xff,
    0xff, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0xff,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
    0x23, 0x22, 0x21, 0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b,
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
    0x35, 0x34, 0x33, 0x32, 0x31, 0x30, 0x2f, 0x2e, 0x2d,
    0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
    0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40, 0x3f,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
    0xff, 0x57, 0x56, 0x55, 0x54, 0x53, 0x52, 0x51, 0xff,
    0xff, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0xff,
    0xff, 0xff, 0x63, 0x62, 0x61, 0x60, 0x5f, 0xff, 0xff
};
#else
const uint8_t LED_BYTEMAP = 54;  // Number of bytes using the row / column scheme (2 don't care bytes)
PROGMEM prog_uint8_t rcLEDMap[] = {
    0x00, 0x09, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x2c, 0x2d,
    0x01, 0x08, 0x0e, 0x17, 0x16, 0x15, 0x14, 0x2b, 0x2e,
    0x02, 0x07, 0x0d, 0x18, 0x19, 0x1a, 0x1b, 0x2a, 0x2f,
    0x03, 0x06, 0x0c, 0x1f, 0x1e, 0x1d, 0x1c, 0x29, 0x30,
    0x04, 0x05, 0x0b, 0x20, 0x23, 0x24, 0x27, 0x28, 0x31,
    0xff, 0xff, 0x0a, 0x21, 0x22, 0x25, 0x26, 0xff, 0xff
};
#endif
/*
 Bitmap LED mapping
 This table provides the mapping between the bitmap specification
 and LED position.
 */
#ifdef FS_TOWER_EYE
const uint8_t LED_BMMAP = 117;  // Number of bytes using the bitmap scheme
PROGMEM prog_uint8_t bmLEDMap[] = {
    0xff, 0xff, 0x03, 0x02, 0x01, 0x00, 0xff, 0xff, 0xff,
    0xff, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0xff,
    0xff, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0xff,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
    0x23, 0x22, 0x21, 0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b,
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
    0x35, 0x34, 0x33, 0x32, 0x31, 0x30, 0x2f, 0x2e, 0x2d,
    0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
    0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40, 0x3f,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
    0xff, 0x57, 0x56, 0x55, 0x54, 0x53, 0x52, 0x51, 0xff,
    0xff, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0xff,
    0xff, 0xff, 0x63, 0x62, 0x61, 0x60, 0x5f, 0xff, 0xff
};
#else
const uint8_t LED_BMMAP = 72;  // Number of bytes using the bitmap scheme
PROGMEM prog_uint8_t bmLEDMap[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0xff, 0xff, 0xff,
    0x09, 0x08, 0x07, 0x06, 0x05, 0xff, 0xff, 0xff,
    0x13, 0x14, 0x1b, 0x1c, 0x27, 0x26, 0xff, 0xff,
    0x12, 0x15, 0x1a, 0x1d, 0x24, 0x25, 0xff, 0xff,
    0x11, 0x16, 0x19, 0x1e, 0x23, 0x22, 0xff, 0xff,
    0x10, 0x17, 0x18, 0x1f, 0x20, 0x21, 0xff, 0xff,
    0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0xff, 0xff,
    0x2c, 0x2b, 0x2a, 0x29, 0x28, 0xff, 0xff, 0xff,
    0x2d, 0x2e, 0x2f, 0x30, 0x31, 0xff, 0xff, 0xff
};
#endif
/*
 Cylon pattern mapping
 This table provides the mapping between the array representation in
 the Cylon pattern and LED position.
 */
#ifdef FS_TOWER_EYE
const uint8_t LED_CYMAP = 100;  // Number of bytes using the bitmap scheme
#else
const uint8_t LED_CYMAP = 50;  // Number of bytes using the bitmap scheme
PROGMEM prog_uint8_t cyLEDMap[] = {
    0x00, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09,
    
    0x0a, 0x13, 0x14, 0x1d, 0x1e, 0x27,
    0x26, 0x25, 0x24, 0x23, 0x22, 0x21,
    0x20, 0x1f, 0x1c, 0x1b, 0x1a, 0x19,
    0x18, 0x17, 0x16, 0x15, 0x12, 0x0b,
    0x0c, 0x11, 0x10, 0x0d, 0x0e, 0x0f,
    
    0x28, 0x29, 0x2a, 0x2b, 0x2c,
    0x2d, 0x2e, 0x2f, 0x30, 0x31
};
#endif

/*
 The bitmap pattern is based on a 5x6 back panel and 2-2x5 front panels.
 There are 9 bytes needed for each color to specify the pattern. 2 bytes for
 each panel and 5 bytes for the back panel.
 Each byte specfies the column bitmap selection (1=on, 0=off)
 The bits after row 0x04 on the side panels and after row 0x05 on the back
 are don't care.
 
 0 1 2 3 4 5 6 7 8
 ___________________
 0  |   |         |   |
 1  |   |         |   |
 2  | 10|   30    |10 |
 3  |   |         |   |
 4  |___|         |___|
 5      |_________|
 
 */
/*
 This is the bitmap pattern for the Apis Eyes
 
 0 1 2 3 4 5 6 7 8
 ______
 0     ----1     ---
 1     |           |
 2   --             --
 3  |                 |
 4  |                 |
 5  |                 |
 6  |                 |
 7  |                 |
 8  |                 |
 9   --             --
 10     |           |
 11     ----    x----
 12         -----
 */
PROGMEM prog_uint8_t characterA[] = {0x00, 0x00, // Front right panel
    0b11111100, 0b00001010, 0b00001001, 0b00001010, 0b11111100, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterB[] = {0x00, 0x00, // Front right panel
    0b00011010, 0b00100101, 0b00100101, 0b00100101, 0b11111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterC[] = {0x00, 0x00, // Front right panel
    0b00010010, 0b00100001, 0b00100001, 0b00100001, 0b00011110, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterD[] = {0x00, 0x00, // Front right panel
    0b00011110, 0b00100001, 0b00100001, 0b00100001, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterE[] = {0x00, 0x00, // Front right panel
    0b00100001, 0b00100101, 0b00100101, 0b00100101, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterF[] = {0x00, 0x00, // Front right panel
    0b00000001, 0b00000101, 0b00000101, 0b00000101, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterG[] = {0x00, 0x00, // Front right panel
    0b00010010, 0b00101001, 0b00100001, 0b00100001, 0b00011110, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterH[] = {0x00, 0x00, // Front right panel
    0b00111111, 0b00000100, 0b00000100, 0b00000100, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterI[] = {0x00, 0x00, // Front right panel
    0b00100001, 0b00100001, 0b00111111, 0b00100001, 0b00100001, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterJ[] = {0x00, 0x00, // Front right panel
    0b00000001, 0b00011111, 0b00100001, 0b00100001, 0b00010000, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterK[] = {0x00, 0x00, // Front right panel
    0b00100001, 0b00010010, 0b00010100, 0b00001000, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterL[] = {0x00, 0x00, // Front right panel
    0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterM[] = {0x00, 0x00, // Front right panel
    0xFF, 0x02, 0x04, 0x02, 0xFF, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterN[] = {0x00, 0x00, // Front right panel
    0b00111111, 0b00001000, 0b00000100, 0b00000010, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterO[] = {0x00, 0x00, // Front right panel
    0b00011110, 0b00100001, 0b00100001, 0b00100001, 0b00011110, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterP[] = {0x00, 0x00, // Front right panel
    0b00000110, 0b00001001, 0b00001001, 0b00001001, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterQ[] = {0x00, 0x00, // Front right panel
    0b00111110, 0b00100001, 0b00110001, 0b00100001, 0b00011110, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterR[] = {0x00, 0x00, // Front right panel
    0b00100010, 0b00010101, 0b00010101, 0b00001101, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterS[] = {0x00, 0x00, // Front right panel
    0b00011001, 0b00100101, 0b00100101, 0b00100101, 0b00100110, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterT[] = {0x00, 0x00, // Front right panel
    0b00000001, 0b00000001, 0b00111111, 0b00000001, 0b00000001, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterU[] = {0x00, 0x00, // Front right panel
    0b00011111, 0b00100000, 0b00100000, 0b00100000, 0b00011111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterV[] = {0x00, 0x00, // Front right panel
    0b00001111, 0b00010000, 0b00100000, 0b00010000, 0b00001111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterW[] = {0x00, 0x00, // Front right panel
    0b00111111, 0b00010000, 0b00001000, 0b00010000, 0b00111111, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterX[] = {0x00, 0x00, // Front right panel
    0x31, 0x0a, 0x04, 0x0a, 0x31, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterY[] = {0x00, 0x00, // Front right panel
    0b00000011, 0b00000100, 0b00111000, 0b00000100, 0b00000011, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t characterZ[] = {0x00, 0x00, // Front right panel
    0b00100001, 0b00100011, 0b00101001, 0b00110001, 0b00100001, // Back panel
    0x00, 0x00}; // Front left panel
PROGMEM prog_uint8_t *characterTable[] =
{
    characterA,
    characterB,
    characterC,
    characterD,
    characterE,
    characterF,
    characterG,
    characterH,
    characterI,
    characterJ,
    characterK,
    characterL,
    characterM,
    characterN,
    characterO,
    characterP,
    characterQ,
    characterR,
    characterS,
    characterT,
    characterU,
    characterV,
    characterW,
    characterX,
    characterY,
    characterZ
};

#ifdef FS_TOWER_EYE
PROGMEM prog_uint8_t cylonVertical1[] = {
    0b00111100,0b10000100,0b00001001,0b00001010,0b00011000,0b00110000,0b01100000,0b11000000,0b10000000,0b00000001,0b00000011,0b00001010,0b00010010,0b11000100,0b00000111
};
PROGMEM prog_uint8_t cylonVertical2[] = {
    0b00000000,0b01111000,0b00010000,0b00010001,0b00100100,0b01001000,0b10010000,0b00100000,0b01000001,0b10000010,0b00000100,0b00010001,0b11100001,0b00000011,0b00000000
};
PROGMEM prog_uint8_t cylonVertical3[] = {
    0b00000000,0b00000000,0b11100000,0b00100000,0b01000010,0b10000100,0b00001000,0b00010001,0b00100010,0b01000100,0b10001000,0b11100000,0b00000000,0b00000000,0b00000000
};
PROGMEM prog_uint8_t cylonVertical4[] = {
    0b00000000,0b00000000,0b00000000,0b11000000,0b10000001,0b00000010,0b00000101,0b00001010,0b00010100,0b00101000,0b01110000,0b00000000,0b00000000,0b00000000,0b00000000
};
PROGMEM prog_uint8_t cylonVertical5[] = {
    0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b00000010,0b00000100,0b00001000,0b00010000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000
};
const int cylonVerticalFrameCount = 8;
PROGMEM prog_uint8_t *cylonVerticalTable[] =
{
    cylonVertical1,
    cylonVertical2,
    cylonVertical3,
    cylonVertical4,
    cylonVertical5,
    cylonVertical4,
    cylonVertical3,
    cylonVertical2
};
#else
PROGMEM prog_uint8_t cylonVertical1[] = {0xAA, 0xAA, // Front right panel
    0b00111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // Back panel
    0x55, 0x55}; // Front left panel
PROGMEM prog_uint8_t cylonVertical2[] = {0xA5, 0xA5, // Front right panel
    0b00000000, 0b00111111, 0b00000000, 0b00000000, 0b00000000, // Back panel
    0x5A, 0x5A}; // Front left panel
PROGMEM prog_uint8_t cylonVertical3[] = {0x5A, 0x5A, // Front right panel
    0b00000000, 0b00000000, 0b00111111, 0b00000000, 0b00000000, // Back panel
    0xAA, 0xAA}; // Front left panel
PROGMEM prog_uint8_t cylonVertical4[] = {0x55, 0x55, // Front right panel
    0b00000000, 0b00000000, 0b00000000, 0b00111111, 0b00000000, // Back panel
    0x5A, 0x5A}; // Front left panel
PROGMEM prog_uint8_t cylonVertical5[] = {0xA5, 0xA5, // Front right panel
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00111111, // Back panel
    0xAA, 0xAA}; // Front left panel
const int cylonVerticalFrameCount = 8;
PROGMEM prog_uint8_t *cylonVerticalTable[] =
{
    cylonVertical1,
    cylonVertical2,
    cylonVertical3,
    cylonVertical4,
    cylonVertical5,
    cylonVertical4,
    cylonVertical3,
    cylonVertical2
};
#endif

#ifdef FS_TOWER_EYE
PROGMEM prog_uint8_t cylonPong1[] = {
    0b10000000,0b10000000,0b10000000,0b10000000,0b10000000,0b10000000,0b10000000,0b00000000,0b00000001,0b00000010,0b00001000,0b00100000,0b10000000,0b00000000,0b00000010
};
PROGMEM prog_uint8_t cylonPong2[] = {
    0b10000000,0b00000000,0b00000001,0b00000001,0b00000001,0b00000001,0b00000001,0b00000001,0b00000001,0b00000010,0b00000100,0b00010000,0b01000000,0b00000000,0b00000001
};
PROGMEM prog_uint8_t cylonPong3[] = {
    0b10000000,0b00000000,0b00000001,0b00000010,0b00000010,0b00000010,0b00000010,0b00000010,0b00000010,0b00000010,0b00000100,0b00001000,0b00100000,0b10000000,0b00000000
};
PROGMEM prog_uint8_t cylonPong4[] = {
    0b01000000,0b00000000,0b00000001,0b00000010,0b00000100,0b00000100,0b00000100,0b00000100,0b00000100,0b00000100,0b00000100,0b00001000,0b00010000,0b01000000,0b00000000
};
PROGMEM prog_uint8_t cylonPong5[] = {
    0b00100000,0b10000000,0b00000000,0b00000010,0b00000100,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,0b00010000,0b00100000,0b00000000
};
PROGMEM prog_uint8_t cylonPong6[] = {
    0b00010000,0b01000000,0b00000000,0b00000001,0b00000100,0b00001000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00100000,0b00000000
};
PROGMEM prog_uint8_t cylonPong7[] = {
    0b00001000,0b00100000,0b10000000,0b00000000,0b00000010,0b00001000,0b00010000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00000000
};
PROGMEM prog_uint8_t cylonPong8[] = {
    0b00000100,0b00010000,0b01000000,0b00000000,0b00000001,0b00000100,0b00010000,0b00100000,0b01000000,0b01000000,0b01000000,0b01000000,0b01000000,0b01000000,0b00000000
};
PROGMEM prog_uint8_t cylonPong9[] = {
    0b00000010,0b00001000,0b00100000,0b10000000,0b00000000,0b00000010,0b00001000,0b00100000,0b01000000,0b10000000,0b10000000,0b10000000,0b10000000,0b10000000,0b00000000
};
PROGMEM prog_uint8_t cylonPong10[] = {
    0b00000010,0b00000100,0b00010000,0b01000000,0b00000000,0b00000001,0b00000100,0b00010000,0b01000000,0b10000000,0b00000000,0b00000001,0b00000001,0b00000001,0b00000001
};
PROGMEM prog_uint8_t cylonPong11[] = {
    0b00000010,0b00000100,0b00001000,0b00100000,0b10000000,0b00000000,0b00000010,0b00001000,0b00100000,0b10000000,0b00000000,0b00000001,0b00000010,0b00000010,0b00000010
};
PROGMEM prog_uint8_t cylonPong12[] = {
    0b00000100,0b00000100,0b00001000,0b00010000,0b01000000,0b00000000,0b00000001,0b00000100,0b00010000,0b01000000,0b00000000,0b00000001,0b00000010,0b00000100,0b00000100
};
PROGMEM prog_uint8_t cylonPong13[] = {
    0b00001000,0b00001000,0b00001000,0b00010000,0b00100000,0b10000000,0b00000000,0b00000010,0b00001000,0b00100000,0b10000000,0b00000000,0b00000010,0b00000100,0b00001000
};
PROGMEM prog_uint8_t cylonPong14[] = {
    0b00010000,0b00010000,0b00010000,0b00010000,0b00100000,0b01000000,0b00000000,0b00000001,0b00000100,0b00010000,0b01000000,0b00000000,0b00000001,0b00000100,0b00001000
};
PROGMEM prog_uint8_t cylonPong15[] = {
    0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b01000000,0b10000000,0b00000000,0b00000010,0b00001000,0b00100000,0b10000000,0b00000000,0b00000010,0b00001000
};
PROGMEM prog_uint8_t cylonPong16[] = {
    0b01000000,0b01000000,0b01000000,0b01000000,0b01000000,0b01000000,0b10000000,0b00000000,0b00000001,0b00000100,0b00010000,0b01000000,0b00000000,0b00000001,0b00000100
};
const int cylonPongFrameCount = 16;
PROGMEM prog_uint8_t *cylonPongTable[] =
{
    cylonPong1,
    cylonPong2,
    cylonPong3,
    cylonPong4,
    cylonPong5,
    cylonPong6,
    cylonPong7,
    cylonPong8,
    cylonPong9,
    cylonPong10,
    cylonPong11,
    cylonPong12,
    cylonPong13,
    cylonPong14,
    cylonPong15,
    cylonPong16
};
#else
PROGMEM prog_uint8_t cylonPong1[] = {0xAA, 0xAA, // Front right panel
    0b00000001, 0b00000010, 0b00000100, 0b00101000, 0b00010000, // Back panel
    0xA5, 0xA5}; // Front left panel
PROGMEM prog_uint8_t cylonPong2[] = {0x55, 0x55, // Front right panel
    0b00000010, 0b00000101, 0b00001000, 0b00010000, 0b00100000, // Back panel
    0xA5, 0xA5}; // Front left panel
PROGMEM prog_uint8_t cylonPong3[] = {0xA5, 0xA5, // Front right panel
    0b00000100, 0b00001010, 0b00010001, 0b00100000, 0b00000000, // Back panel
    0x55, 0x55}; // Front left panel
PROGMEM prog_uint8_t cylonPong4[] = {0x5A, 0x5A, // Front right panel
    0b00001000, 0b00010100, 0b00100010, 0b00000001, 0b00000000, // Back panel
    0x5A, 0x5A}; // Front left panel
PROGMEM prog_uint8_t cylonPong5[] = {0x55, 0x55, // Front right panel
    0b00010000, 0b00101000, 0b00000100, 0b00000010, 0b00000001, // Back panel
    0xAA, 0xAA}; // Front left panel
PROGMEM prog_uint8_t cylonPong6[] = {0xA5, 0xA5, // Front right panel
    0b00100000, 0b00010000, 0b00001000, 0b00000101, 0b00000010, // Back panel
    0x5A, 0x5A}; // Front left panel
PROGMEM prog_uint8_t cylonPong7[] = {0x5A, 0x5A, // Front right panel
    0b00000000, 0b00100000, 0b00010001, 0b00001010, 0b00000100, // Back panel
    0x55, 0x55}; // Front left panel
PROGMEM prog_uint8_t cylonPong8[] = {0x55, 0x55, // Front right panel
    0b00000000, 0b00000001, 0b00100010, 0b00010100, 0b00001000, // Back panel
    0xAA, 0xAA}; // Front left panel
const int cylonPongFrameCount = 8;
PROGMEM prog_uint8_t *cylonPongTable[] =
{
    cylonPong1,
    cylonPong2,
    cylonPong3,
    cylonPong4,
    cylonPong5,
    cylonPong6,
    cylonPong7,
    cylonPong8
};
#endif

PROGMEM prog_uint8_t spiral1[] = {0xAA, 0xAA, // Front right panel
    0b00000000, 0b00000010, 0b00010101, 0b00001000, 0b00000000, // Back panel
    0xA5, 0xA5}; // Front left panel
PROGMEM prog_uint8_t spiral2[] = {0x55, 0x55, // Front right panel
    0b00000010, 0b00000101, 0b00000100, 0b00010100, 0b00001000, // Back panel
    0xA5, 0xA5}; // Front left panel
PROGMEM prog_uint8_t spiral3[] = {0xA5, 0xA5, // Front right panel
    0b00000110, 0b00000100, 0b00000100, 0b00000100, 0b00001100, // Back panel
    0x55, 0x55}; // Front left panel
PROGMEM prog_uint8_t spiral4[] = {0x5A, 0x5A, // Front right panel
    0b00000100, 0b00000100, 0b00000100, 0b00000100, 0b00000100, // Back panel
    0x5A, 0x5A}; // Front left panel
PROGMEM prog_uint8_t spiral5[] = {0x55, 0x55, // Front right panel
    0b00000100, 0b00001000, 0b00000100, 0b00000010, 0b00000100, // Back panel
    0xAA, 0xAA}; // Front left panel
PROGMEM prog_uint8_t spiral6[] = {0xA5, 0xA5, // Front right panel
    0b00001000, 0b00010000, 0b00001110, 0b00000001, 0b00000010, // Back panel
    0x5A, 0x5A}; // Front left panel
PROGMEM prog_uint8_t spiral7[] = {0x5A, 0x5A, // Front right panel
    0b00000000, 0b00010000, 0b00111111, 0b00000001, 0b00000000, // Back panel
    0x55, 0x55}; // Front left panel
PROGMEM prog_uint8_t spiral8[] = {0x55, 0x55, // Front right panel
    0b00000000, 0b00000000, 0b00011111, 0b00000000, 0b00000000, // Back panel
    0xAA, 0xAA}; // Front left panel
const int sprialFrameCount = 8;
PROGMEM prog_uint8_t *spiralTable[] =
{
    spiral1,
    spiral2,
    spiral3,
    spiral4,
    spiral5,
    spiral6,
    spiral7,
    spiral8
};

#ifdef FS_TOWN_CENTER
PROGMEM prog_uint8_t triggerPattern0[] = {FS_ID_SOUND_ACTIVATE, 128, 200, 0, 40, 128};
PROGMEM prog_uint8_t triggerPattern1[] = {FS_ID_DANCING, 100, 200, 0, 40, 0};
PROGMEM prog_uint8_t triggerPattern2[] = {FS_ID_RADIO_TOWER, 0, 228, 0, 0, 228};

const int triggerPatternsCount = 3;
PROGMEM prog_uint8_t *triggerPatterns[] =
{
    triggerPattern0,
    triggerPattern1,
    triggerPattern2
};
#else
PROGMEM prog_uint8_t triggerPattern0[] = {FS_ID_RADIO_TOWER, 0, 228, 0, 0, 228};
PROGMEM prog_uint8_t triggerPattern1[] = {FS_ID_FULL_COLOR, 0, 228, 0, 0, 228};
PROGMEM prog_uint8_t triggerPattern2[] = {FS_ID_SPARKLE, 100, 255, 255, 255, 0};
PROGMEM prog_uint8_t triggerPattern3[] = {FS_ID_HEART, 165, 165, 0, 50, 250};
PROGMEM prog_uint8_t triggerPattern4[] = {FS_ID_DESCEND, 20, 165, 165, 50, 250};
PROGMEM prog_uint8_t triggerPattern5[] = {FS_ID_BREATHE, 100, 20, 0, 200, 150};
PROGMEM prog_uint8_t triggerPattern6[] = {FS_ID_ORGANIC, 10, 0, 0, 0, 0};
PROGMEM prog_uint8_t triggerPattern7[] = {FS_ID_CYLON, 50, 200, 100, 0, 0};
PROGMEM prog_uint8_t triggerPattern8[] = {FS_ID_DROP, 10, 0, 0, 0, 0};
PROGMEM prog_uint8_t triggerPattern9[] = {FS_ID_CHARACTER, 220, 200, 0, 200, 1};
PROGMEM prog_uint8_t triggerPattern10[] = {FS_ID_CYLON_VERTICAL, 100, 0, 200, 0, 0};
PROGMEM prog_uint8_t triggerPattern11[] = {FS_ID_SPARKLER, 180, 88, 161, 0, 255};
PROGMEM prog_uint8_t triggerPattern12[] = {FS_ID_CYLON, 189, 100, 255, 43, 255};
PROGMEM prog_uint8_t triggerPattern13[] = {FS_ID_CYLON_PONG, 100, 200, 0, 40, 250};
PROGMEM prog_uint8_t triggerPattern14[] = {FS_ID_SOUND_ACTIVATE, 128, 200, 0, 40, 128};
PROGMEM prog_uint8_t triggerPattern15[] = {FS_ID_PRISM, 100, 200, 0, 40, 150};
PROGMEM prog_uint8_t triggerPattern16[] = {FS_ID_MATRIX, 100, 250, 0, 80, 200};
PROGMEM prog_uint8_t triggerPattern17[] = {FS_ID_RAINBOW_CHASE, 250, 0, 0, 0, 150};
PROGMEM prog_uint8_t triggerPattern18[] = {FS_ID_RANDOM_FLASH, 250, 200, 10, 130, 200};
PROGMEM prog_uint8_t triggerPattern19[] = {FS_ID_STARFIELD, 10, 0, 0, 0, 0};
PROGMEM prog_uint8_t triggerPattern20[] = {FS_ID_SPIRAL, 100, 200, 0, 40, 0};
PROGMEM prog_uint8_t triggerPattern21[] = {FS_ID_TILT, 100, 200, 0, 40, 0};
PROGMEM prog_uint8_t triggerPattern22[] = {FS_ID_DANCING, 100, 200, 0, 40, 0};

const int triggerPatternsCount = 23;
PROGMEM prog_uint8_t *triggerPatterns[] =
{
    triggerPattern0,
    triggerPattern1,
    triggerPattern2,
    triggerPattern3,
    triggerPattern4,
    triggerPattern5,
    triggerPattern6,
    triggerPattern7,
    triggerPattern8,
    triggerPattern9,
    triggerPattern10,
    triggerPattern11,
    triggerPattern12,
    triggerPattern13,
    triggerPattern14,
    triggerPattern15,
    triggerPattern16,
    triggerPattern17,
    triggerPattern18,
    triggerPattern19,
    triggerPattern20,
    triggerPattern21,
    triggerPattern22
};
#endif

/*
 The image pattern is based on a 5x6 back panel and 2-2x5 front panels.
 There are 50 bytes needed for each color to specify the pattern (1 for each pixel).
 Each byte is the color intensity for that position.
 Indexing starts at [0, 0] and ends at [5, 6].  The bottom rows of
 the front panels is skipped / don't care.  This includes [5, 0], [5, 1],
 [5, 7] & [5, 8].
 
 0 1 2 3 4 5 6 7 8
 __________________
 0  |   |         |   |
 1  |   |         |   |
 2  | 10|   30    |10 |
 3  |   |         |   |
 4  |___|         |___|
 5      |_________|
 
 */
PROGMEM prog_uint8_t dropRed[]  = {0x00, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00, // Row 0
    0x10, 0x20, 0x40, 0x80, 0xaf, 0x80, 0x40, 0x20, 0x10, // Row 1
    0x20, 0x40, 0x80, 0xaf, 0xff, 0xaf, 0x80, 0x40, 0x20, // Row 2
    0x20, 0x40, 0x80, 0xaf, 0xff, 0xaf, 0x80, 0x40, 0x20, // Row 3
    0x10, 0x20, 0x40, 0x80, 0xaf, 0x80, 0x40, 0x20, 0x10, // Row 4
    0x00, 0x00, 0x20, 0x40, 0x80, 0x40, 0x20}; // Row 5 - First 2 bytes are `don't care'
PROGMEM prog_uint8_t dropGreen[]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Row 0
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, // Row 1
    0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, // Row 2
    0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, // Row 3
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, // Row 4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Row 5 - First 2 bytes are `don't care'
PROGMEM prog_uint8_t dropBlue[]  = {0x00, 0x00, 0x00, 0x50, 0x50, 0x50, 0x00, 0x00, 0x00, // Row 0
    0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, // Row 1
    0x00, 0x00, 0x50, 0x00, 0x8f, 0x00, 0x50, 0x00, 0x00, // Row 2
    0x00, 0x00, 0x50, 0x00, 0x8f, 0x00, 0x50, 0x00, 0x00, // Row 3
    0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, // Row 4
    0x00, 0x00, 0x00, 0x50, 0x50, 0x50, 0x00}; // Row 5 - First 2 bytes are `don't care'

#ifdef FS_TOWER_EYE
const int matrixFrameCount = 13;
PROGMEM prog_uint8_t matrixBitmap[]  = {
    0x0f, 0x00, 0x00, 0x1f, 0x00, 0x2f, 0x0f, 0x0f, 0xff, // Row 0
    0x1f, 0x00, 0x00, 0x2f, 0x0f, 0xff, 0x1f, 0x1f, 0x00, // Row 1
    0x2f, 0x0f, 0x0f, 0xff, 0x1f, 0x00, 0x2f, 0x2f, 0x0f, // Row 2
    0xff, 0x1f, 0x1f, 0x00, 0x2f, 0x0f, 0xff, 0xff, 0x1f, // Row 3
    0x00, 0x2f, 0x2f, 0x00, 0xff, 0x0f, 0x00, 0x00, 0x2f, // Row 4
    0x00, 0xff, 0xff, 0x0f, 0x00, 0x1f, 0x0f, 0x00, 0xff, // Row 5
    0x0f, 0x00, 0x00, 0x1f, 0x00, 0x2f, 0x0f, 0x0f, 0x00, // Row 6
    0x1f, 0x00, 0x00, 0x2f, 0x0f, 0xff, 0x1f, 0x1f, 0x00, // Row 7
    0x2f, 0x0f, 0x0f, 0xff, 0x1f, 0x00, 0x2f, 0x2f, 0x0f, // Row 8
    0xff, 0x1f, 0x1f, 0x00, 0x2f, 0x0f, 0xff, 0xff, 0x1f, // Row 9
    0x00, 0x2f, 0x2f, 0x00, 0xff, 0x0f, 0x00, 0x00, 0x2f, // Row 10
    0x00, 0xff, 0xff, 0x0f, 0x00, 0x1f, 0x0f, 0x00, 0xff, // Row 11
    0x00, 0x00, 0xff, 0x0f, 0x00, 0x1f, 0x0f, 0x00, 0xff}; // Row 12
#else
const int matrixFrameCount = 6;
PROGMEM prog_uint8_t matrixBitmap[]  = {
    0x0f, 0xff, 0x00, 0x1f, 0x00, 0x2f, 0x0f, 0x0f, 0xff, // Row 0
    0x1f, 0x00, 0x00, 0x2f, 0x0f, 0xff, 0x1f, 0x1f, 0x00, // Row 1
    0x2f, 0x0f, 0x0f, 0xff, 0x1f, 0x00, 0x2f, 0x2f, 0x0f, // Row 2
    0xff, 0x1f, 0x1f, 0x00, 0x2f, 0x0f, 0xff, 0xff, 0x1f, // Row 3
    0x00, 0x2f, 0x2f, 0x00, 0xff, 0x0f, 0x00, 0x00, 0x2f, // Row 4
    0x00, 0x00, 0xff, 0x0f, 0x00, 0x1f, 0x0f}; // Row 5 - First 2 bytes are `don't care'
#endif

#endif

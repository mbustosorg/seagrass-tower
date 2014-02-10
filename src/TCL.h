/*****************************************************************************
 * TCL.h
 *
 * Copyright 2011-2012 Christpher De Vries
 * This program is distributed under the Artistic License 2.0, a copy of which
 * is included in the file LICENSE.txt
 ****************************************************************************/
// Choose between bit bang (any 2 dio pins) and SPI modes
//#define TCL_DIO
//#define TCL_SPI

#ifndef TCL_h
#define TCL_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef TCL_SPI
#include "SPI.h"
#endif

#define TCL_POT1 A0
#define TCL_POT2 A1
#define TCL_POT3 A2
#define TCL_POT4 A3
#define TCL_MOMENTARY1 4
#define TCL_MOMENTARY2 5
#define TCL_SWITCH1 6
#define TCL_SWITCH2 7

#ifdef TCL_DIO
#define TCL_CLOCKPIN 2
#define TCL_DATAPIN 3
#define TCL2_CLOCKPIN 4
#endif

class TclClass {
  public:
    static void begin();
    static void setupDeveloperShield();
    static void end();
    static void sendFrame(uint8_t flag, uint8_t red, uint8_t green, uint8_t blue);
    static void sendColor(uint8_t red, uint8_t green, uint8_t blue);
    static void sendEmptyFrame();
    static uint8_t makeFlag(uint8_t red, uint8_t green, uint8_t blue);
    static void setAll(int num_leds, uint8_t red, uint8_t green, uint8_t blue);
  private:
#ifdef TCL_DIO
    static uint8_t datapinmask, clkpinmask;
    static volatile uint8_t *dataport, *clkport;
    static void dioWrite(uint8_t c);
#endif
};

extern TclClass TCL;
#endif

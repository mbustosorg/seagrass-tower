/*****************************************************************************
 * Tcl.cpp
 *
 * Copyright 2011-2012 Christpher De Vries
 * This program is distributed under the Artistic License 2.0, a copy of which
 * is included in the file LICENSE.txt
 ****************************************************************************/

#include "TCL.h"

TclClass TCL;

#ifdef TCL_DIO
uint8_t TclClass::datapinmask, TclClass::clkpinmask;
volatile uint8_t *TclClass::dataport, *TclClass::clkport;
#endif


void TclClass::begin() {
#ifdef TCL_SPI
  // Set the SPI parameters
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
#endif
#ifdef TCL_DIO
  pinMode(TCL_DATAPIN, OUTPUT);
  pinMode(TCL_CLOCKPIN, OUTPUT);
  pinMode(TCL2_CLOCKPIN, OUTPUT);
  //clkport     = portOutputRegister(digitalPinToPort(TCL_CLOCKPIN));
  //clkpinmask  = digitalPinToBitMask(TCL_CLOCKPIN);
  //dataport    = portOutputRegister(digitalPinToPort(TCL_DATAPIN));
  //datapinmask = digitalPinToBitMask(TCL_DATAPIN);
  //*clkport   &= ~clkpinmask;
  //*dataport  &= ~datapinmask;
#endif
}

void TclClass::setupDeveloperShield() {
  pinMode(TCL_MOMENTARY1, INPUT);
  pinMode(TCL_MOMENTARY2, INPUT);
  pinMode(TCL_SWITCH1, INPUT);
  pinMode(TCL_SWITCH2, INPUT);
  
  digitalWrite(TCL_MOMENTARY1, HIGH);
  digitalWrite(TCL_MOMENTARY2, HIGH);
  digitalWrite(TCL_SWITCH1, HIGH);
  digitalWrite(TCL_SWITCH2, HIGH);
}

void TclClass::end() {
#ifdef TCL_SPI
  SPI.end();
#endif
}

uint8_t TclClass::makeFlag(uint8_t red, uint8_t green, uint8_t blue) {
  uint8_t flag = 0;

  flag = (red&0xC0)>>6;
  flag |= ((green&0xC0)>>4);
  flag |= ((blue&0xC0)>>2);
  return ~flag;
}

#ifdef TCL_DIO
void TclClass::dioWrite(uint8_t c) {
  for (uint8_t bit = 0x80; bit; bit >>= 1) {
    if (c & bit) {
      digitalWrite(TCL_DATAPIN, HIGH);
    } else {
      digitalWrite(TCL_DATAPIN, LOW);
    }
    digitalWrite(TCL_CLOCKPIN, HIGH);
    digitalWrite(TCL_CLOCKPIN, LOW);
    digitalWrite(TCL2_CLOCKPIN, HIGH);
    digitalWrite(TCL2_CLOCKPIN, LOW);
  }
}
#endif

void TclClass::sendFrame(uint8_t flag, uint8_t red, uint8_t green, uint8_t blue) {
#ifdef TCL_SPI
  SPI.transfer(flag);
  SPI.transfer(blue);
  SPI.transfer(green);
  SPI.transfer(red);
#endif
#ifdef TCL_DIO
  dioWrite(flag);
  dioWrite(blue);
  dioWrite(green);
  dioWrite(red);
#endif
}

void TclClass::sendColor(uint8_t red, uint8_t green, uint8_t blue) {
  uint8_t flag;

  flag = makeFlag(red,green,blue);

  sendFrame(flag,red,green,blue);
}

void TclClass::sendEmptyFrame() {
  sendFrame(0x00,0x00,0x00,0x00);
}

void TclClass::setAll(int num_leds, uint8_t red, uint8_t green, uint8_t blue) {
  sendEmptyFrame();
  for (int i=0; i<num_leds; i++) {
    sendColor(red, green, blue);
  }
  sendEmptyFrame();
}

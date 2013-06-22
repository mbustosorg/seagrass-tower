//
//  furSwarmMember.ino
//
//  $Date: 2013-06-21 22:33:10 -0700 (Fri, 21 Jun 2013) $
//  $Rev: 1011 $
//  $Author: mauricio $
//
//  Copyright (c) 2012, Mauricio Bustos
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, 
//  with or without modification, are permitted provided 
//  that the following conditions are met:
//
//  - Redistributions must not be for profit.  This software in source or binary form, 
//      with or without modification, cannot be included in anyproduct that is then sold.  
//      Please contact Mauricio Bustos m@bustos.org if you are interested in a for-profit license.
//  - Redistributions of source code or in binary form must reproduce the above copyright notice, 
//      this list of conditions and the following disclaimer in the documentation 
//      and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
//  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

//
// To run the simulator:
// TOWER      : -DTEENSY -DFS_TOWER
// VEST       : -DUSE_TCL -DFS_VEST 
// HAT        : -DFS_HAT
// TOWER VEST : -DUSE_TCL -DFS_VEST -DFS_TOWER_VEST
//

#include "furSwarmPatterns.h"
#include "furSwarmPatternConst.h"
#include "furSwarmControlMachine.h"
#include "xBeeConfiguration.h"
#include <EEPROM.h>
#ifdef TEENSY
#include "oledControl.h"
#include "towerPatterns.h"
int displayUpdateCount = 0;
#else
#include "furSwarmPatterns.h"
#endif

#define Endian32_Swap(value) \
         ((((uint32_t)((value) & 0x000000FF)) << 24) | \
         (((uint32_t)((value) & 0x0000FF00)) << 8) | \
         (((uint32_t)((value) & 0x00FF0000)) >> 8) | \
         (((uint32_t)((value) & 0xFF000000)) >> 24))

// System configuration
#define versionId 0x14
const bool legacyPro = false;
#ifdef FS_VEST
const uint8_t memberType = 0x01; // Vest
#elif FS_HAT
const uint8_t memberType = 0x02; // Hat
#elif FS_TOWER
const uint8_t memberType = 0x03; // Tower
#endif

// Heartbeat message layout
uint8_t heartbeatPayload[] = { 
  0x01,       // Byte 0: Message Type ID (1 byte)
  versionId,  // Byte 1: Version ID (1 byte)
  0,          // Byte 2: Frame location (2 bytes)
  0,
  0,          // Byte 4: Current Pattern
  0,          // Byte 5: Battery Voltage (2 bytes)
  0, 
  0,          // Byte 7: Frame Rate (1 byte) 
  memberType, // Byte 8: Member type
  0,          // Byte 9: Failed messages (2 bytes)
  0,
#ifdef TEENSY
  0,          // Byte 11: Lattitude (2 bytes)
  0,
  0,          // Byte 13: Longitude (2 bytes)
  0
#endif
};
#define frameCountPosition (2)
#define patternPosition (4)
#define voltagePosition (5)
#define frameRatePosition (7)
#define failedMessagePosition (9)

// XBee data
XBee xbee = XBee();
const uint32_t XBeeBaudRate = 19200;
const uint32_t XBeePANID = 0x2014;
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000); // Coord ID free
ZBTxRequest heartbeatMessage = ZBTxRequest(addr64, heartbeatPayload, sizeof(heartbeatPayload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
ZBRxResponse rxResponse = ZBRxResponse();
unsigned long heartbeatTimestamp = 0;
const unsigned long defaultHeartbeatPeriod = 6000;
const unsigned long hifreqHeartbeatPeriod = 2000;
unsigned long heartbeatPeriod = defaultHeartbeatPeriod;

// Analog pin definitions
int batteryVoltagePin;

// Miscellaneous
#define statusLed 13
// Digital pin definitions
int commandPatternPin;
int commandOffPin;

// FurSwarm member data
uint16_t frameCount = 0;
unsigned long frameRateCount = 0;
int frameStarted = 0;

#ifdef TEENSY
towerPatterns Control;
uint32_t heartbeatTimestampStart;
int frameLate = 0;
#else
furSwarmPatterns Control;
#endif

// Power management
//const uint16_t batteryLimit = 839; // 8.0V
//const uint16_t batteryLimit = 661; // 6.3V
#ifdef TEENSY
const uint16_t batteryLimit = 546; // 6.3V
#else
const uint16_t batteryLimit = 624; // 6.0V
#endif
const unsigned long lowBatteryTimeThreshold = 30000;
//const unsigned long lowBatteryTimeThreshold = 300000;
unsigned long lowBatteryTime = 0;
bool powerShutdown = false;

#ifdef USE_TCL
#include "TCL.h"
#elif USE_WS2801
// Adafruit RGB bulbs
Adafruit_WS2801 strip = Adafruit_WS2801(lpdCount, lpdDataPin, lpdClockPin);
#else
// Adafruit RGB strip
#endif

//! Prototypes
void updateDisplay();
void setupRadio();
void setupTimers();
void setStartupPattern();
void sendHeartbeat();
void processIncoming();
void processRXResponse();

//! Initialize the system
void setup() {
  if (legacyPro) {
	batteryVoltagePin = 1;
	commandOffPin = 5;
	commandPatternPin = 6;
	Control.FS_BREATHE_MULTIPLIER = 50.0;
	Control.randomSeedPin = 0;
	Control.audioAnalogPin = 0;
  } else {
	batteryVoltagePin = 0;
	commandOffPin = 5;
	commandPatternPin = 6;
	Control.FS_BREATHE_MULTIPLIER = 50.0;
	Control.randomSeedPin = 8;
#ifdef TEENSY
	batteryVoltagePin = 15;
	Control.audioAnalogPin = 14;
	pinMode(6, INPUT_PULLUP); // Down
	pinMode(10, INPUT_PULLUP); // Up
	pinMode(11, INPUT_PULLUP); // Left
	pinMode(12, INPUT_PULLUP); // Right
	pinMode(13, INPUT_PULLUP); // Center
#else
	Control.audioAnalogPin = 9;
#endif
  }

  pinMode(commandPatternPin, INPUT);
  digitalWrite(commandPatternPin, HIGH);
  pinMode(commandOffPin, OUTPUT);
  digitalWrite(commandOffPin, HIGH);
  pinMode(statusLed, OUTPUT);

  // Initialize client heartbeat timestamp
  heartbeatTimestamp = millis();
  // Initialize the LED strand object
#ifdef USE_TCL
  TCL.begin();
#elif USE_WS2801
  // Adafruit bulbs
  strip.begin();
  strip.show();
#else
  // Adafruit strip
#endif
#ifdef TEENSY
  setupOLEDdisplay();
  Control.randomSeedPin = 12;
#endif
  // Initialize the radio
  setupRadio();
  // Initialize generic pattern attributes
  Control.heartbeatPumpShortStart = millis();
  Control.heartbeatPumpShortPeriod = 500;
  Control.heartbeatIntensity = 150;
  Control.characterIndex = 1;
  Control.frameIndex = 1;
  Control.initializeRandom(160, 255, 0xff, 0xff, 0xff, false); 
  setStartupPattern();
  setupTimers();
}

#ifdef TEENSY
//! Set up the frame rate timers
void setupTimers() {
  // Do nothing
}

extern "C" {
//! Frame rate interrupt handler running at 60Hz
void pit2_isr(void)
{
  if (frameStarted) {
	frameLate++;
  }
  frameStarted = 1;
  frameCount++;
  frameRateCount++;
  if (displayUpdateCount >= 60) {
	displayUpdateCount = 0;
  } else {
	displayUpdateCount++;
  }
  PIT_TFLG2 = 1;
}

//! Audio input interrupt handler running at kHz
void pit3_isr(void)
{
  int sample;
  sample = analogRead (Control.audioAnalogPin);
  Control.audioSampleInput[Control.audioSampleInputIndex * 2] = (float32_t)(sample - 512.0) / 10.0;
  Control.audioSampleInputIndex++;
  if (Control.audioSampleInputIndex >= TEST_LENGTH_SAMPLES / 2) {
	Control.audioSampleInputIndex = 0;
	PIT_TCTRL3 &= ~(1 << 0);
  }
  //digitalWrite(13, HIGH);
  //digitalWrite(13, LOW);
  PIT_TFLG3 = 1;
}

void startup_late_hook(void) {
  // This is called from mk20dx128.c
  //Turn on interrupts:
  SIM_SCGC6 |= SIM_SCGC6_PIT;
  // turn on PIT
  PIT_MCR = 0x00;
  NVIC_ENABLE_IRQ(IRQ_PIT_CH2);
  NVIC_ENABLE_IRQ(IRQ_PIT_CH3);
  
  PIT_LDVAL2 = 800000 - 1; // setup timer 2 for frame timer period (60Hz) = 48MHz / 60Hz
  PIT_TCTRL2 = 0x2; // enable Timer 2 interrupts
  PIT_TCTRL2 |= 0x1; // start Timer 2
  PIT_TFLG2 |= 1;

  PIT_LDVAL3 = 1250 - 1; // setup timer 2 for frame timer period (40kHz) = 48MHz / 40kHz
  PIT_TCTRL3 = 0x2; // enable Timer 3 interrupts
  PIT_TCTRL3 |= 0x1; // start Timer 3
  PIT_TFLG3 |= 1;
}
}

#else // not TEENSY

//! Frame rate interrupt handler
ISR(TIMER1_COMPA_vect) {
  frameStarted = 1;
  frameCount++;
  frameRateCount++;
}

//! Set up the frame rate timers
void setupTimers() {

  cli();

  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B
  TCNT1  = 0;  //initialize counter value to 0
  // set compare match register for 60hz increments
  OCR1A = 33333 - 1;  // = (16*10^6) / (60 * 8) - 1 (must be < 65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 bits for 8 prescaler
  TCCR1B |= (1 << CS11);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
}
#endif

//! Configure the XBee
void setupRadio() {
  xBeeConfiguration XBconfiguration;
#ifdef NOT_EMBEDDED
#else
#ifdef TEENSY
  HardwareSerial *radioSerial = &Serial1;
#else
  HardwareSerial *radioSerial = &Serial;
#endif
#endif
  radioSerial->begin(XBeeBaudRate);
  xbee.setSerial(*radioSerial);
  XBconfiguration.setRadio(&xbee);
  bool connected = XBconfiguration.connected();
  bool needPersist = false;
  if (!connected) {
	uint8_t baudRateData[] = {FS_ID_FULL_COLOR, 128, 10, 0, 10};
	for (int i = 0; i < numberOfBaudRates && !connected; i++) {
#ifdef TEENSY
	  display.clearDisplay();
	  display.setTextSize(2);
	  display.setTextColor(WHITE);
	  display.setCursor(0,0);
	  display.print("Baud:");
	  display.println(baudRates[i]);
	  display.display();
#endif
	  baudRateData[2] = baudRateData[2] + 30;
	  baudRateData[4] = baudRateData[4] + 30;
	  Control.initializePattern(baudRateData, 5);
	  radioSerial->begin(baudRates[i]);
	  connected = XBconfiguration.connected();
	  if (connected && baudRates[i] != XBeeBaudRate) {
		XBconfiguration.setBaudRate(XBeeBaudRate);
		needPersist = true;
	  }
	}        
	uint8_t data[] = {FS_ID_FULL_COLOR, 128, 255, 255, 0, 200};
	Control.initializePattern(data, 6);
	delay (2000);
  }
  if (XBeePANID != Endian32_Swap(XBconfiguration.panID()) || needPersist) {
#ifdef TEENSY
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0,0);
	display.print("PANID:");
	display.println(XBeePANID);
	display.display();
#endif
	uint8_t data[] = {FS_ID_FULL_COLOR, 128, 0, 255, 255, 200};
	Control.initializePattern(data, 6);
	XBconfiguration.setPanID(Endian32_Swap(XBeePANID));
	XBconfiguration.persistConfiguration();  
	delay (2000);
  }
  uint8_t dataRed[] = {FS_ID_FULL_COLOR, 128, 255, 0, 0, 255};
  Control.initializePattern(dataRed, 6);
  delay (500);
  uint8_t dataGreen[] = {FS_ID_FULL_COLOR, 128, 0, 255, 0, 255};
  Control.initializePattern(dataGreen, 6);
  delay (500);
  uint8_t dataBlue[] = {FS_ID_FULL_COLOR, 128, 0, 0, 255, 255};
  Control.initializePattern(dataBlue, 6);
  delay (500);
}

//! Setup the last set startup pattern
void setStartupPattern() {
  // Cycle patterns at startup
  //pattern = EEPROM.read(0);
  //EEPROM.write(0, pattern);
  //uint8_t data[] = {FS_ID_RAINBOW_CHASE, 250, 200, 100, 130, 150};
  //Control.initializePattern(data, 6);
  //uint8_t data[] = {FS_ID_RANDOM_FLASH, 250, 200, 10, 130, 200};
  //Control.initializePattern(data, 6);
  //uint8_t data[] = {FS_ID_SOUND_ACTIVATE, 128, 200, 200, 200, 128};
  //Control.initializePattern(data, 6);
  //uint8_t data[] = {FS_ID_IMAGE_SCROLL, 20, 200, 10, 130, 200};
  //Control.initializePattern(data, 6);
  //uint8_t data[] = {FS_ID_ORGANIC, 10};
  //Control.initializePattern(data, 2);
  //uint8_t data[] = {FS_ID_STARFIELD, 10, 100, 100, 100};
  //Control.initializePattern(data, 5);
  //uint8_t data[] = {FS_ID_SPIRAL, 100, 200, 0, 40};
  //Control.initializePattern(data, 5);
  //uint8_t data[] = {FS_ID_TILT, 100, 200, 0, 40};
  //Control.initializePattern(data, 5);
#ifdef TEENSY
  //uint8_t data[] = {FS_ID_SPIRAL, 10, 100, 100, 100, 100};
  uint8_t data[] = {FS_ID_SPECTRUM_ANALYZER, 100, 200, 0, 40, 100};
  Control.initializePattern(data, 6);
#else
  uint8_t data[] = {FS_ID_STARFIELD, 10, 100, 100, 100, 100};
  Control.initializePattern(data, 6);
#endif
}

//! Main loop
void loop() {   
  if (frameStarted) {
	Control.continuePatternDisplay();
	updateDisplay();
	sendHeartbeat();
	frameStarted = 0;
  }
  processIncoming();
}

//! Update the status display
void updateDisplay() {
#ifdef TEENSY
  if (displayUpdateCount == 0) {
	if (Control.pattern == FS_ID_TILT) {
	  int hue = atan2(Control.zTilt, Control.yTilt) * 360.0 / (2.0 * 3.14159);
	  int saturation = sqrt((Control.yTilt / 200) * (Control.yTilt / 200) + (Control.zTilt / 200) * (Control.zTilt / 200)) * 100;
	  displayTiltParameters(hue, saturation, Control.isShaking, Control.zTiltCal != 0);
	} else {
	  unsigned long timeStamp = millis();
	  float frameRate = (float) frameRateCount / (float) (timeStamp - heartbeatTimestamp) * 1000.0 + 0.5;
	  float Vtemp = analogRead(38) * 0.0029296875;
	  float Temp1;
	  if (Vtemp >= 0.7012) {
		Temp1 = 25 - ((Vtemp - 0.7012) / 0.001646);
	  } else {
		Temp1 = 25 - ((Vtemp - 0.7012) / 0.001749);
	  }
	  displayOperatingDetails(Control.pattern, timeStamp / 1000, frameRate, frameLate);//Temp1);
	}
  }
#endif
}

//!Process incoming commands
void processIncoming() {
  int commandMode = clickCount(commandPatternPin);
  xbee.readPacket();
  if (xbee.getResponse().isAvailable() && !powerShutdown) {
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      processRXResponse();
    } else if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      if (txStatus.getDeliveryStatus() == SUCCESS) {
      } 
      else {
		Control.failedMessageCount++;
      }
    } else if (xbee.getResponse().isError()) {
	  Control.failedMessageCount++;
    } 
  } else if (commandMode == 4) {
	digitalWrite(commandOffPin, LOW);
#if TEENSY
  } else if (digitalRead(12) == 0) {
	digitalWrite(commandOffPin, LOW);
#endif
  } else if (powerShutdown) {
	// In shutdown mode, set all to zero
	for (int i = 0; i < LED_COUNT; i++) {
	  Control.ledRed[i] = 0;
	  Control.ledBlue[i] = 0;
	  Control.ledGreen[i] = 0;
	}
	Control.ledRed[0] = 30;
	Control.ledRed[LED_COUNT - 1] = 30;
	Control.displayData(true, true, true);
	delay(60000);
	digitalWrite(commandOffPin, LOW);
  } else if (commandMode == 1) {
	Control.triggerPatternChange();
  } else if (commandMode == 2) {
	Control.advancePatternSpeed();
  } else if (commandMode == 3) {
	Control.advancePatternIntensity(Control.pattern);
  }
}

//! Process the incoming Coordinator message
void processRXResponse() {
  uint8_t* data;
  uint8_t dataLength;
  xbee.getResponse().getZBRxResponse(rxResponse);
  // TODO: Check data length for potential errors
  data = rxResponse.getData();
  dataLength = rxResponse.getDataLength();
  frameCount = 0;
  if (data[0] == FS_ID_PRISM_DISTANCE && data[1] == 128) {
	heartbeatPeriod = hifreqHeartbeatPeriod;
  } else {
	heartbeatPeriod = defaultHeartbeatPeriod;
  }
  Control.initializePattern(data, dataLength);
}

//! Broadcast a heartbeat if it's been long enough since the last one
void sendHeartbeat() {
  float frameRate = 0.0;
  uint8_t frameRateByte;
  unsigned long currentTimestamp = millis();
  unsigned long sinceLastHeartbeat = currentTimestamp - heartbeatTimestamp;
  if (sinceLastHeartbeat > heartbeatPeriod) {
	frameRate = (float) frameRateCount / (float) (sinceLastHeartbeat) * 1000.0;
	frameRateByte = (uint8_t) (frameRate + 0.5);
	uint16_t batteryVoltage = analogRead(batteryVoltagePin);
	if (batteryVoltage < batteryLimit) {
	  if (lowBatteryTime == 0) {
		lowBatteryTime = millis();
	  } else {
		unsigned long lowBatteryNow = millis();
		if (lowBatteryNow - lowBatteryTime > lowBatteryTimeThreshold) {
		  powerShutdown = true;
		}
	  }
	}
    memcpy (&heartbeatPayload[frameCountPosition], &frameCount, sizeof(frameCount));
    memcpy (&heartbeatPayload[patternPosition], &Control.pattern, sizeof(Control.pattern));
	memcpy (&heartbeatPayload[voltagePosition], &batteryVoltage, sizeof(batteryVoltage));
	memcpy (&heartbeatPayload[frameRatePosition], &frameRateByte, sizeof(frameRateByte));
	memcpy (&heartbeatPayload[failedMessagePosition], &Control.failedMessageCount, sizeof(Control.failedMessageCount));
	xbee.send(heartbeatMessage);
	frameRateCount = 0;
#ifdef TEENSY
	heartbeatTimestampStart = currentTimestamp;
#endif
    heartbeatTimestamp = millis();
  }
}


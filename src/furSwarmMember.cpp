//
//  furSwarmMember.ino
//
//  $Date: 2013-11-23 17:07:15 -0800 (Sat, 23 Nov 2013) $
//  $Rev: 1132 $
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
#include "TinyGPS.h"  
#include "gps2rtc.h"
int displayUpdateCount = 0;
const int gps_1pps_pin = 9;
GPS2RTC gps2rtc;
unsigned long gpsTimeStamp = 0;
#define GPS_DISPLAY_TIME (10000)
#else
#include "furSwarmPatterns.h"
#endif

#define Endian32_Swap(value) \
         ((((uint32_t)((value) & 0x000000FF)) << 24) | \
         (((uint32_t)((value) & 0x0000FF00)) << 8) | \
         (((uint32_t)((value) & 0x00FF0000)) >> 8) | \
         (((uint32_t)((value) & 0xFF000000)) >> 24))

// System configuration
#define MESSAGE_TYPE_FULL 0x01
#define MESSAGE_TYPE_SHORT_VERSION 0x02
#define versionId 0x15
const bool legacyPro = false;
#ifdef FS_VEST
const uint8_t memberType = FS_TYPE_VEST;
#elif FS_HAT
const uint8_t memberType = FS_TYPE_HAT;
#elif FS_TOWER
const uint8_t memberType = FS_TYPE_TOWER;
#endif

// Heartbeat message layout
uint8_t heartbeatPayload[] = { 
  MESSAGE_TYPE_FULL,      // Byte 0: Message Type ID (1 byte)
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
  0,          // Byte 11: Latitude (4 bytes)
  0,
  0,
  0,
  0,          // Byte 15: Longitude (4 bytes)
  0,
  0,
  0
#endif
};
uint8_t heartbeatCount = 0;
#define FULL_HEARTBEAT_PERIOD (2)
#define frameCountPosition (2)
#define patternPosition (4)
#define voltagePosition (5)
#define frameRatePosition (7)
#define failedMessagePosition (9)
#define latitudePosition (11)
#define longitudePosition (15)
// XBee data
XBee xbee = XBee();
const uint32_t XBeeBaudRate = 57600;
const uint32_t XBeePANID = 0x2014;
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000); // Coord ID free
//XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x407C73CD); // Coord ID free
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
bool daytimeShutdown = false;
bool allowDaytimeShutdown = false;
#define TEN_MINUTES (600) // 10 Minutes in seconds
#define DORMANT_TIME_LIMIT (1200) // 20 Minutes in seconds

#ifdef TEENSY
towerPatterns Control;
uint32_t heartbeatTimestampStart;
int frameLate = 0;
const int latitudeStartByte = 0;
const int longitudeStartByte = 4;
unsigned long lastMessageReceipt = 0;
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
unsigned long readEepromLong(const int startByte);
void updateDisplay();
void setupRadio();
void setupTimers();
void setStartupPattern();
void sendHeartbeat();
void processIncoming();
void processRXResponse();
void synchronizeToRTC();

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
	Control.latitude = readEepromLong(latitudeStartByte);
	Control.longitude = readEepromLong(longitudeStartByte);
	// Fix to work when we don't have accurate GPS data available
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
  gps2rtc.begin(&Serial3, 9600, gps_1pps_pin);      // Required.
  lastMessageReceipt = rtc_get();
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

//! Synchronize to the second boundary
void synchronizeToRTC()
{
  unsigned long rtcTime = rtc_get();
  while (rtc_get() == rtcTime) {}
}

//! Set up the frame rate timers
void setupTimers() {
  // Do nothing
}

//! Long value from EEPROM starting at `startByte'
unsigned long readEepromLong(const int startByte) {
  unsigned long storedValue = 0;
  uint8_t retrievedByte; 
  retrievedByte = EEPROM.read (startByte);
  memcpy ((char *)&storedValue, &retrievedByte, sizeof (uint8_t));
  retrievedByte = EEPROM.read (startByte + 1);
  memcpy ((char *)&storedValue + 1, &retrievedByte, sizeof (uint8_t));
  retrievedByte = EEPROM.read (startByte + 2);
  memcpy ((char *)&storedValue + 2, &retrievedByte, sizeof (uint8_t));
  retrievedByte = EEPROM.read (startByte + 3);
  memcpy ((char *)&storedValue + 3, &retrievedByte, sizeof (uint8_t));
  return storedValue;
}

//! Write long value to EEPROM starting at `startByte'
int writeEepromLong(unsigned long writeValue, const int startByte) {
  uint8_t writeByte = 0;
  memcpy (&writeByte, (char *)&writeValue, sizeof (uint8_t));
  EEPROM.write (startByte, writeByte);
  memcpy (&writeByte, (char *)&writeValue + 1, sizeof (uint8_t));
  EEPROM.write (startByte + 1, writeByte);
  memcpy (&writeByte, (char *)&writeValue + 2, sizeof (uint8_t));
  EEPROM.write (startByte + 2, writeByte);
  memcpy (&writeByte, (char *)&writeValue + 3, sizeof (uint8_t));
  EEPROM.write (startByte + 3, writeByte);
  if (readEepromLong(startByte) == writeValue) {
	return 1;
  } else {
	return 0;
  }
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

//! Audio input interrupt handler running at 40kHz
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
  
  PIT_LDVAL2 = CPU_SPEED / FRAME_RATE - 1; // setup timer 2 for frame timer period (60Hz) = 48MHz / 60Hz
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

//! Synchronize to the second boundary
void synchronizeToRTC()
{
  // Do nothing
}

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
  if (false) {
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
  //uint8_t data[] = {FS_ID_SOUND_ACTIVATE, 128, 200, 200, 200, 128};
  uint8_t data[] = {FS_ID_SPIRAL, 100, 200, 0, 40, 120};
  //uint8_t data[] = {FS_ID_SPECTRUM_ANALYZER, 128, 200, 200, 200, 128};
  //uint8_t data[] = {FS_ID_ORGANIC, 10, 200, 200, 200, 128};
  //uint8_t data[] = {FS_ID_FLAME, 20, 1, 255, 130, 0, 0};
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

#ifdef TEENSY
//! Check if GPS data is available
void updateGPSdata() {
  if (gps2rtc.rtc_time_set) {
	if (writeEepromLong (gps2rtc.latitude, latitudeStartByte) && writeEepromLong (gps2rtc.longitude, longitudeStartByte)) {
	  Control.latitude = readEepromLong(latitudeStartByte);
	  Control.longitude = readEepromLong(longitudeStartByte);
	  gpsTimeStamp = millis();
	} else {
	  displayMessage ("Could not write GPS data");
	}
  } else {
	displayMessage ("Waiting for 1pps GPS data");
  }
}
#endif

//! Update the status display
void updateDisplay() {
#ifdef TEENSY
  if (displayUpdateCount == 0) {
	unsigned long timeStamp = millis();
	if (Control.pattern == FS_ID_TILT) {
	  int hue = atan2(Control.zTilt, Control.yTilt) * 360.0 / (2.0 * 3.14159);
	  int saturation = sqrt((Control.yTilt / 200) * (Control.yTilt / 200) + (Control.zTilt / 200) * (Control.zTilt / 200)) * 100;
	  displayTiltParameters(hue, saturation, Control.isShaking, Control.zTiltCal != 0);
	} else if (gpsTimeStamp != 0 && timeStamp - gpsTimeStamp < GPS_DISPLAY_TIME) {
 	  unsigned long rtcTime = rtc_get();
	  displayGPSdata ((float) Control.latitude / 100000.0, (float) Control.longitude / 100000.0, rtcTime);
	} else if (gps2rtc.receiving_serial_data && timeStamp - gps2rtc.last_sentence_receipt < GPS_DISPLAY_TIME / 2 && 
			   gpsTimeStamp == 0 && gps2rtc.last_sentence_receipt != 0) {
	  updateGPSdata();
	} else {
	  float frameRate = (float) frameRateCount / (float) (timeStamp - heartbeatTimestamp) * 1000.0 + 0.5;
	  //float Vtemp = analogRead(38) * 0.0029296875;
	  //float Temp1;
	  //if (Vtemp >= 0.7012) {
	  //Temp1 = 25 - ((Vtemp - 0.7012) / 0.001646);
	  //} else {
	  //Temp1 = 25 - ((Vtemp - 0.7012) / 0.001749);
	  //}
	  unsigned long rtcTime = rtc_get();
	  unsigned long days = rtcTime / (3600 * 24);
	  unsigned long running = rtcTime - days * 3600 * 24;
	  //unsigned long hours = running / 3600;
	  //unsigned long minutes = ((float) running / 3600.0 - hours) * 60.0;
	  unsigned long onTime = 2 * 3600 + 30 * 60; // 02:30 UTC == 19:30 PDT
	  unsigned long offTime = 12 * 3600 + 30 * 60; // 12:30 UTC == 05:30 PDT
	  //unsigned long offTime = 20 * 3600 + 30 * 60;
	  displayOperatingDetails(Control.pattern, timeStamp / 1000, frameRate, rtcTime, Control.latitude / 100000.0, Control.longitude / 100000.0);
	  if (daytimeShutdown) {
		if (Control.pattern != FS_ID_OFF) {
		  uint8_t data[] = {FS_ID_OFF};
		  Control.initializePattern(data, 1);
		}
		if ((running > onTime) && (running < offTime)) {
		  daytimeShutdown = false;
		  uint8_t data[] = {FS_ID_FULL_COLOR, 100, 0, 50, 200, 170};
		  Control.initializePattern(data, 6);
		}
	  } else {
		if ((running < onTime) || (running > offTime)) {
		  if (allowDaytimeShutdown) {
			uint8_t data[] = {FS_ID_OFF, 100, 100, 100, 100, 100};
			Control.initializePattern(data, 6);
			daytimeShutdown = true;
			Control.animations.isAnimating = false;
		  }
		} else if (rtcTime - lastMessageReceipt > DORMANT_TIME_LIMIT && rtcTime % TEN_MINUTES == 0 && Control.pattern != FS_ID_ANIMATE_1) {
		  //uint8_t data[] = {FS_ID_ANIMATE_1, 100, 100, 100, 100, 100};
		  uint8_t data[] = {FS_ID_OFF, 100, 100, 100, 100, 100};
		  Control.initializePattern(data, 2);
		}
	  }
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
	Control.latitude = random (-100, 100);
	Control.longitude = random (-100, 100);
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
#ifdef TEENSY
  if (!daytimeShutdown) {
	lastMessageReceipt = rtc_get();
	Control.animations.isAnimating = data[0] == FS_ID_ANIMATE_1;
  }
#endif
  Control.setPatternData(data, dataLength);
}

//! Broadcast a heartbeat if it's been long enough since the last one
void sendHeartbeat() {
  float frameRate = 0.0;
  uint8_t frameRateByte;
  unsigned long currentTimestamp = millis();
  unsigned long sinceLastHeartbeat = currentTimestamp - heartbeatTimestamp;
  if (sinceLastHeartbeat > heartbeatPeriod) {
	heartbeatCount++;
	if (heartbeatCount % FULL_HEARTBEAT_PERIOD != 0) {
	  uint8_t heartbeatShortPayload[] = {MESSAGE_TYPE_SHORT_VERSION, versionId};  // Short heartbeat
	  ZBTxRequest heartbeatShortMessage = ZBTxRequest(addr64, heartbeatShortPayload, sizeof(heartbeatShortPayload));
	  xbee.send(heartbeatShortMessage);
	} else {
	  frameRate = (float) frameRateCount / (float) (sinceLastHeartbeat) * 1000.0;
	  frameRateByte = (uint8_t) (frameRate + 0.5);
	  uint16_t batteryVoltage = analogRead(batteryVoltagePin);
	  if (batteryVoltage < batteryLimit / 2) {
		// Probably running off external power
	  } else if (batteryVoltage < batteryLimit) {
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
#ifdef TEENSY
	  heartbeatTimestampStart = currentTimestamp;
	  memcpy (&heartbeatPayload[latitudePosition], &Control.latitude, sizeof(Control.latitude));
	  memcpy (&heartbeatPayload[longitudePosition], &Control.longitude, sizeof(Control.longitude));
#endif
	  xbee.send(heartbeatMessage);
	}
	frameRateCount = 0;
	heartbeatTimestamp = millis();
  }
}


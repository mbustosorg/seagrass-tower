//
//  furSwarmMember.ino
//
//  $Date: 2013-12-25 15:41:59 -0800 (Wed, 25 Dec 2013) $
//  $Rev: 1138 $
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
#include "towerPatterns.h"
#include "TinyGPS.h"  
#include "gps2rtc.h"
#include "statusLED.h"
#include <Wire.h>
const int gps_1pps_pin = 9;
const int gps_onoff_pin = 16;
GPS2RTC gps2rtc;
unsigned long gpsTimeStamp = 0;
timeStruct clock;
#define GPS_RESET_TIME (600000)
#define GPS_DISPLAY_TIME (5000)
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
#if defined (FS_VEST) || (FS_TOWER_VEST)
const uint8_t memberType = FS_TYPE_VEST;
#elif defined (FS_HAT) || (FS_TOWER_HAT)
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

// Digital pin definitions
int commandPatternPin;
int commandOffPin = 0;
int commandSpeedUpPin = 10;
int commandSpeedDownPin = 12;
int commandIntensityPin = 11;
int suicidePin;
const int AUDIO_INPUT_PIN = 14;        // Input ADC pin for audio data.
const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING = 16;  // Number of samples to average with each ADC reading.
const unsigned long FRAME_LENGTH = 16;

// Timing & frame execution control
volatile int frameNumber = 0;
volatile uint16_t frameCount = 0;
volatile unsigned long frameRateCount = 0;
volatile int frameStarted = 0;
bool daytimeShutdown = false;
#if defined(FS_TOWER) && !defined(FS_TOWER_EYE)
bool allowDaytimeShutdown = true;
#else
bool allowDaytimeShutdown = false;
#endif
#define TEN_MINUTES (600) // 10 Minutes in seconds
#define DORMANT_TIME_LIMIT (1200) // 20 Minutes in seconds

#ifdef TEENSY
towerPatterns Control;
uint32_t heartbeatTimestampStart;
const int latitudeStartByte = 0;
const int longitudeStartByte = 4;
unsigned long lastMessageReceipt = 0;
statusLED led;
#else
furSwarmPatterns Control;
#endif

// Power management
//const uint16_t batteryLimit = 839; // 8.0V
//const uint16_t batteryLimit = 661; // 6.3V
#ifdef TEENSY
const uint16_t batteryLimit = 540; // 6.4V
const uint16_t batteryWarningLimit = 560; // 6.7V
#else
const uint16_t batteryLimit = 624; // 6.0V
const uint16_t batteryWarningLimit = 624; // 6.0V
#endif
const unsigned long lowBatteryTimeThreshold = 30000;
//const unsigned long lowBatteryTimeThreshold = 300000;
unsigned long lowBatteryTime = 0;
unsigned long lowBatteryWarningTime = 0;
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

//! Initialize the system
void setup() {
  if (legacyPro) {
	batteryVoltagePin = 1;
	suicidePin = 5;
	commandPatternPin = 6;
	Control.FS_BREATHE_MULTIPLIER = 50.0;
	Control.randomSeedPin = 0;
	Control.audioAnalogPin = 0;
  } else {
	batteryVoltagePin = 0;
	suicidePin = 5;
	commandPatternPin = 6;
	Control.FS_BREATHE_MULTIPLIER = 50.0;
	Control.randomSeedPin = 8;
#ifdef TEENSY
	batteryVoltagePin = 15;
	Control.audioAnalogPin = AUDIO_INPUT_PIN;
	pinMode(AUDIO_INPUT_PIN, INPUT);
	analogReadResolution(ANALOG_READ_RESOLUTION);
	analogReadAveraging(ANALOG_READ_AVERAGING);
	commandSpeedUpPin = 10;
	commandIntensityPin = 11;
	commandSpeedDownPin = 12;
	commandOffPin = 13;
	pinMode(commandPatternPin, INPUT_PULLUP); // Down
	pinMode(commandSpeedUpPin, INPUT_PULLUP); // Up
	pinMode(commandIntensityPin, INPUT_PULLUP); // Left
	pinMode(commandSpeedDownPin, INPUT_PULLUP); // Right
	//pinMode(commandOffPin, INPUT_PULLUP); // Center
	Control.latitude = readEepromLong(latitudeStartByte);
	Control.longitude = readEepromLong(longitudeStartByte);
	// Fix to work when we don't have accurate GPS data available
	Control.randomSeedPin = 5;
	pinMode(7, INPUT);
	gps2rtc.begin(&Serial3, 9600, gps_1pps_pin, gps_onoff_pin);      // Required.  
	gps2rtc.state = synchronize_to_pps;
	lastMessageReceipt = millis();
	// Wire will be used for Accelerometer
	Wire.begin();
	Control.accel.startup();
	Control.accel.setFilterLength(10);
#ifdef FFT_DIAGNOSTICS
	Serial.begin(115200);
#endif
#else
	Control.audioAnalogPin = 9;
#endif
  }

  pinMode(commandPatternPin, INPUT);
  digitalWrite(commandPatternPin, HIGH);
  pinMode(suicidePin, OUTPUT);
  digitalWrite(suicidePin, HIGH);

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

//! Send out a display message
void displayMessage(const char* message) {
#ifdef SERIAL_DIAGNOSTICS
  Serial.println (message);
#endif
}

#ifdef TEENSY

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

//static void handle_gps_receiver_data(char a_char)
void serialEvent3()
  // Handle one character of data from the GPS receiver. 
  {
    gps2rtc.handle_gps_receiver_data(Serial3.read());
  }

extern "C" {
//! Frame rate interrupt handler running at 60Hz
void pit2_isr(void)
{
  frameStarted = 1;
  frameCount++;
  frameRateCount++;
  if (frameNumber >= 59) {
	frameNumber = 0;
  } else {
	frameNumber++;
  }
  PIT_TFLG2 = 1;
}

//! Audio input interrupt handler running at 40kHz
void pit3_isr(void)
{
  int sample;
  sample = analogRead (Control.audioAnalogPin);
  Control.audioSampleInput[Control.audioSampleInputIndex * 2] = (float32_t)(sample) - 512.0;
  Control.audioSampleInput[Control.audioSampleInputIndex * 2 + 1] = 0.0;
  Control.audioSampleInputIndex++;
  if (Control.audioSampleInputIndex >= TEST_LENGTH_SAMPLES / 2) {
	Control.audioSampleInputIndex = 0;
	// Turn off the timer and let the processor turn it back on once the data has been analyzed
	PIT_TCTRL3 &= ~(1 << 0);
  }
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
  
  PIT_LDVAL2 = F_CPU / FRAME_RATE - 1; // setup timer 2 for frame timer period (60Hz) = F_CPU (e.g. 96MHz) / 60Hz
  PIT_TCTRL2 = 0x2; // enable Timer 2 interrupts
  PIT_TCTRL2 |= 0x1; // start Timer 2
  PIT_TFLG2 |= 1;

  //PIT_LDVAL3 = 2400 - 1; // setup timer 2 for frame timer period (20kHz) = 48MHz / 20kHz, which results in a 20kHz / 256 = 78Hz frequency bucket
  //PIT_LDVAL3 = 1250 - 1; // setup timer 2 for frame timer period (40kHz) = 48MHz / 40kHz, which results in a 40kHz / 256 = 156Hz frequency bucket
  PIT_LDVAL3 = F_CPU / 40000 - 1; // setup timer 2 for frame timer period (40kHz) = 48MHz / 40kHz, which results in a 40kHz / 256 = 156Hz frequency bucket
  PIT_TCTRL3 = 0x2; // enable Timer 3 interrupts
  PIT_TCTRL3 |= 0x1; // start Timer 3
  PIT_TFLG3 |= 1;
}

void enableTimers() {
  startup_late_hook();
}

void disableTimers() {
  PIT_TCTRL2 = 0; // disable Timer 2
  PIT_TCTRL3 = 0; // Disable Timer 3
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
  if (false) {
	if (!connected) {
	  uint8_t baudRateData[] = {FS_ID_FULL_COLOR, 128, 10, 0, 10};
	  for (int i = 0; i < numberOfBaudRates && !connected; i++) {
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
	  uint8_t data[] = {FS_ID_FULL_COLOR, 128, 0, 255, 255, 200};
	  Control.initializePattern(data, 6);
	  XBconfiguration.setPanID(Endian32_Swap(XBeePANID));
	  XBconfiguration.persistConfiguration();  
	  delay (2000);
	}
  }
  uint8_t dataRed[] = {FS_ID_FULL_COLOR, 1, 255, 0, 0, 255};
  Control.initializePattern(dataRed, 6);
  led.pulse (50, 0, 0, 500, true);
  led.update();
  delay (500);
  uint8_t dataGreen[] = {FS_ID_FULL_COLOR, 1, 0, 255, 0, 255};
  Control.initializePattern(dataGreen, 6);
  led.pulse (0, 50, 0, 500, true);
  led.update();
  delay (500);
  uint8_t dataBlue[] = {FS_ID_FULL_COLOR, 1, 0, 0, 255, 255};
  Control.initializePattern(dataBlue, 6);
  led.pulse (0, 0, 50, 500, true);
  led.update();
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
  //uint8_t data[] = {FS_ID_SPIRAL, 100, 200, 0, 40, 120};
  //uint8_t data[] = {FS_ID_ORGANIC, 10, 200, 200, 200, 128};
  //uint8_t data[] = {FS_ID_FLAME, 20, 1, 255, 130, 0, 0};
#ifdef FFT_DIAGNOSTICS
  //uint8_t data[] = {FS_ID_SOUND_ACTIVATE, 128, 200, 200, 200, 128};
  uint8_t data[] = {FS_ID_SPECTRUM_ANALYZER, 128, 200, 200, 200, 128};
#elif FS_TOWER_VEST
  //uint8_t data[] = {FS_ID_FULL_COLOR, 100, 0, 50, 200, 170};
  uint8_t data[] = {FS_ID_DANCING, 100, 200, 0, 40, 0};
  Control.triggerPattern = 22;
  //uint8_t data[] = {FS_ID_RADIO_TOWER, 200, 0, 200, 0, 120};
#else
  //uint8_t data[] = {FS_ID_SPECTRUM_ANALYZER, 128, 200, 200, 200, 128};
  //uint8_t data[] = {FS_ID_RADIO_TOWER, 200, 0, 200, 0, 120};
  uint8_t data[] = {FS_ID_CYLON_VERTICAL, 200, 0, 200, 0, 120};
  //uint8_t data[] = {FS_ID_TILT, 100, 200, 0, 40, 120};
#endif
  Control.initializePattern(data, 6);
#else
  uint8_t data[] = {FS_ID_STARFIELD, 10, 100, 100, 100, 100};
  Control.initializePattern(data, 6);
#endif
}

//! Do we need to catchup to the 1PPS tick?
void catchup() {
#ifdef TEENSY
  unsigned long lastPPS = gps2rtc.lastPPSTime;
  if (lastPPS > 0) {
	unsigned long ppsGap = millis() - lastPPS;
	if (ppsGap > FRAME_LENGTH && ppsGap < 1000) {
	  unsigned long lastTime = millis();
	  while (lastTime > lastPPS && 
		 lastTime - lastPPS < 1000 && 
		 lastTime - lastPPS + 1000 > FRAME_LENGTH) {
	    while (millis() == lastTime) {}
	    lastTime = millis();
	  }
	  frameNumber = 0;
	  Control.setRadioTowerSyncTimestamp (millis());
	}
  }
#endif
}

//! Main loop
void loop() {   
  if (frameStarted) {
	Control.continuePatternDisplay();
#ifdef TEENSY
	Control.setFrameNumber(frameNumber);
#endif
	updateDisplay();
	if (frameNumber == 0) {
	  catchup();
	}
	sendHeartbeat();
	frameStarted = 0;
  }
  processIncoming();
}

#ifdef TEENSY
//! Check if GPS data is available
void updateGPSdata() {
  if (writeEepromLong (gps2rtc.latitude, latitudeStartByte) && writeEepromLong (gps2rtc.longitude, longitudeStartByte)) {
	Control.latitude = readEepromLong(latitudeStartByte);
	Control.longitude = readEepromLong(longitudeStartByte);
	gpsTimeStamp = millis();
  } else {
	displayMessage ("Could not write GPS data");
  }
}
#endif

//! Debug output the GPS details
void displayGPSdata(float lat, float lon) {
  Serial.print("lat:");
  Serial.print(lat);
  Serial.print(", lon:");
  Serial.print(lon);
  Serial.print(", time:");
  if (clock.hours < 10) Serial.print ("0");
  Serial.print(clock.hours); Serial.print(":"); 
  if (clock.minutes < 10) Serial.print ("0");
  Serial.print(clock.minutes); Serial.print(":"); 
  if (clock.seconds < 10) Serial.print ("0");
  Serial.println(clock.seconds); 
}

//! Update the status display
void updateDisplay() {
#ifdef TEENSY
  led.update();
  if (lowBatteryWarningTime != 0 && (frameNumber == 0 || frameNumber == 29)) {
	led.pulse (40, 0, 0, 100, true);
  } else if (frameNumber == 0) {
	unsigned long timeStamp = millis();
	if (gps2rtc.last_sentence_receipt == 0 || timeStamp - gps2rtc.last_sentence_receipt > GPS_DISPLAY_TIME) {
	  // No NMEA data
	  led.pulse (40, 0, 0, 500, true);
	} else if (gps2rtc.lastPPSTime == 0 || timeStamp - gps2rtc.lastPPSTime > GPS_DISPLAY_TIME) {
	  // NMEA data but no 1pps
	  led.pulse (40, 40, 0, 500, true);
	} else {
	  // NMEA data and 1pps
	  if (clock.seconds % 10 == 0) led.pulse (0, 40, 40, 500, true);
	  else led.pulse (0, 40, 0, 500, true);
	  // Reset GPS every GPS_RESET_TIME (10 minutes) to keep things fresh
	  if (gps2rtc.gps_time != 0 && gpsTimeStamp == 0) {
		updateGPSdata();
	  } else if (gpsTimeStamp != 0 && timeStamp - gpsTimeStamp > GPS_RESET_TIME) {
		gpsTimeStamp = 0;
	  }
	}
	//float Vtemp = analogRead(38) * 0.0029296875;
	//float Temp1;
	//if (Vtemp >= 0.7012) {
	//Temp1 = 25 - ((Vtemp - 0.7012) / 0.001646);
	//} else {
	//Temp1 = 25 - ((Vtemp - 0.7012) / 0.001749);
	//}
#ifdef SERIAL_DIAGNOSTICS
	  displayGPSdata(Control.latitude, Control.longitude);
#endif
	if (gps2rtc.gps_time > 0) {
	  unsigned long gpsTime = gps2rtc.gps_time;
	  clock.hours = gpsTime / 3600;
	  clock.minutes = (int) (((float) gpsTime / 3600.0 - clock.hours) * 60);
	  clock.seconds = gpsTime - clock.hours * 3600 - clock.minutes * 60;
	  Control.clock = clock;
	  unsigned long onTime = 1 * 3600 + 30 * 60; // 01:30 UTC == 18:30 PDT
	  unsigned long offTime = 12 * 3600 + 30 * 60; // 12:30 UTC == 05:30 PDT
	  if (daytimeShutdown) {
		led.pulse (50, 0, 50, 100, false);
		if (Control.pattern != FS_ID_OFF) {
		  uint8_t data[] = {FS_ID_OFF};
		  Control.initializePattern(data, 1);
		}
		if ((gpsTime > onTime) && (gpsTime < offTime)) {
		  daytimeShutdown = false;
		  uint8_t data[] = {FS_ID_FULL_COLOR, 100, 0, 50, 200, 170}; // Aquamarine at startup
		  Control.initializePattern(data, 6);
		}
	  } else {
	    if ((gpsTime < onTime) || (gpsTime > offTime)) {
		  if (allowDaytimeShutdown) {
		    uint8_t data[] = {FS_ID_OFF, 100, 100, 100, 100, 100};
		    Control.initializePattern(data, 6);
		    daytimeShutdown = true;
		    Control.animations.isAnimating = false;
		  }
	    } else if (gpsTime - lastMessageReceipt > DORMANT_TIME_LIMIT && gpsTime % TEN_MINUTES == 0 && !Control.animations.isAnimating) {
#ifdef FS_TOWER
	      uint8_t data[] = {FS_ID_ANIMATE_1, 100, 100, 100, 100, 100};
	      //uint8_t data[] = {FS_ID_OFF, 100, 100, 100, 100, 100};
	      Control.initializePattern(data, 2);
#endif
	    }
	  }
	}
  }
#endif
}

//!Process incoming commands
void processIncoming() {
  int commandPatternMode = clickCount(commandPatternPin);
  int commandOffMode = 0;
  int commandSpeedUpMode = clickCount(commandSpeedUpPin);
  int commandSpeedDownMode = clickCount(commandSpeedDownPin);
  int commandIntensityMode = clickCount(commandIntensityPin);
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
  } else if (commandOffMode == 3 || commandIntensityMode == 3) {
	digitalWrite(suicidePin, LOW);
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
	digitalWrite(suicidePin, LOW);
  } else if (commandPatternMode) {
	Control.triggerPatternChange(commandPatternMode == 1);
  } else if (commandSpeedUpMode || commandSpeedDownMode) {
	Control.advancePatternSpeed(digitalRead (commandSpeedUpPin) == LOW || digitalRead(commandSpeedDownPin) == LOW, commandSpeedUpMode);
#ifdef SERIAL_DIAGNOSTICS
	Serial.print ("Pattern Speed: ");
	Serial.println (Control.patternSpeed);
#endif
  } else if (commandIntensityMode) {
	Control.advancePatternIntensity(Control.pattern, commandIntensityMode == 1);
  }
}

//! Process the incoming Coordinator message
void processRXResponse() {
  uint8_t* data;
  uint8_t dataLength;
  xbee.getResponse().getZBRxResponse(rxResponse);
  led.pulse (50, 25, 0, 100, false);
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
	lastMessageReceipt = gps2rtc.gps_time;
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
	led.pulse (0, 0, 40, 100, false);
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
	  } else if (batteryVoltage < batteryWarningLimit) {
		if (lowBatteryWarningTime == 0) {
		  lowBatteryWarningTime = millis();
		}
	  }
	  uint16_t localFrameCount = frameCount;
	  memcpy (&heartbeatPayload[frameCountPosition], &localFrameCount, sizeof(frameCount));
	  memcpy (&heartbeatPayload[patternPosition], &Control.pattern, sizeof(Control.pattern));
	  memcpy (&heartbeatPayload[voltagePosition], &batteryVoltage, sizeof(batteryVoltage));
	  memcpy (&heartbeatPayload[frameRatePosition], &frameRateByte, sizeof(frameRateByte));
	  memcpy (&heartbeatPayload[failedMessagePosition], &Control.failedMessageCount, sizeof(Control.failedMessageCount));
#ifdef TEENSY
	  heartbeatTimestampStart = currentTimestamp;
	  memcpy (&heartbeatPayload[latitudePosition], &Control.latitude, sizeof(Control.latitude));
	  memcpy (&heartbeatPayload[longitudePosition], &Control.longitude, sizeof(Control.longitude));
	  if (Control.isShaking) {
		uint8_t shaking = FS_ID_SHAKING;
		memcpy (&heartbeatPayload[patternPosition], &shaking, sizeof(Control.pattern));
	  }
#endif
	  xbee.send(heartbeatMessage);
	}
	frameRateCount = 0;
	heartbeatTimestamp = millis();
  }
}


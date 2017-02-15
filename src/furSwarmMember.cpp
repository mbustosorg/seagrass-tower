/*

    Copyright (C) 2016 Mauricio Bustos (m@bustos.org)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

//
// To run the simulator:
// TOWER       : -DTEENSY -DUSE_TCL -DTCL_DIO -DADXL345 -DFS_TOWER
// TOWER_VEST  : -DTEENSY -DUSE_TCL -DTCL_DIO -DADXL345 -DFS_TOWER_VEST
// TOWER_EYE   : -DTEENSY -DUSE_TCL -DTCL_DIO -DADXL345 -DFS_TOWER_EYE
// TOWN_CENTER : -DTEENSY -DUSE_TCL -DTCL_DIO -DADXL345 -DFS_TOWN_CENTER
// TOWER_HAT   : -DTEENSY -DADX345 -FS_TOWER_HAT
// VEST        : -DUSE_TCL -DFS_VEST 
// HAT         : -DFS_HAT
//

#include "furSwarmPatternConst.h"
#include "furSwarmControlMachine.h"
#include "xBeeConfiguration.h"
#ifdef TEENSY
#include <EEPROM.h>
#include <Wire.h>
#include "TinyGPS.h"  
#include "gps2rtc.h"
#include "statusLED.h"
const int gps_1pps_pin = 9;
const int gps_onoff_pin = 16;
GPS2RTC gps2rtc;
unsigned long gpsTimeStamp = 0;
#define GPS_RESET_TIME (600000)
#define GPS_DISPLAY_TIME (5000)
statusLED led;
#include "towerPatterns.h"
#else
#include "furSwarmPatterns.h"
#endif
timeStruct clock;

#define Endian32_Swap(value) \
         ((((uint32_t)((value) & 0x000000FF)) << 24) | \
         (((uint32_t)((value) & 0x0000FF00)) << 8) | \
         (((uint32_t)((value) & 0x00FF0000)) >> 8) | \
         (((uint32_t)((value) & 0xFF000000)) >> 24))

// System configuration
#define versionId 0x20
const bool legacyPro = false;
#if defined (FS_VEST) || (FS_TOWER_VEST)
const uint8_t memberType = FS_TYPE_VEST;
#elif defined (FS_HAT) || (FS_TOWER_HAT)
const uint8_t memberType = FS_TYPE_HAT;
#elif FS_TOWER
const uint8_t memberType = FS_TYPE_TOWER;
#elif FS_TOWN_CENTER
const uint8_t memberType = FS_TYPE_TOWN_CENTER;
#elif FS_REEDS
const uint8_t memberType = FS_TYPE_REEDS;
#elif FS_WINDFLOWERS
const uint8_t memberType = FS_TYPE_WINDFLOWERS;
#elif FS_DRESS
const uint8_t memberType = FS_TYPE_DRESS;
#elif FS_ROTOFUEGO
const uint8_t memberType = FS_TYPE_ROTOFUEGO;
#elif FS_BUZZ_INN
const uint8_t memberType = FS_TYPE_BUZZ_INN;
#endif

// Heartbeat message layout
uint8_t heartbeatPayload[] = { 
  FS_ID_MESSAGE_TYPE_FULL,      // Byte 0: Message Type ID (1 byte)
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
int aux1pin = 27;
int aux2pin = 28;
int aux3pin = 29;
int aux4pin = 30;
const int AUDIO_INPUT_PIN = 14;        // Input ADC pin for audio data.
const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING = 16;  // Number of samples to average with each ADC reading.
const unsigned long FRAME_LENGTH = 16;

// Timing & frame execution control
#define POOFER_DEBOUNCE (50)
long pooferStartControl = 0;
volatile int frameNumber = 0;
volatile uint16_t frameCount = 0;
volatile unsigned long frameRateCount = 0;
volatile int frameStarted = 0;
bool daytimeShutdown = false;
#if (FS_TOWN_CENTER)
unsigned long OnTime = 10 * 3600; // 10:00 UTC == 3:00 PDT
unsigned long OffTime = 19 * 3600; // 19:00 UTC == 12:00 PDT
#else
unsigned long OnTime = 1 * 3600 + 30 * 60; // 01:30 UTC == 18:30 PDT
//unsigned long OnTime = 20 * 3600 + 35 * 60;
unsigned long OffTime = 12 * 3600 + 30 * 60; // 12:30 UTC == 05:30 PDT
#endif
#if defined(FS_TOWER) && !defined(FS_TOWER_EYE)
bool allowDaytimeShutdown = true;
#elif defined(FS_REEDS) || defined(FS_WINDFLOWERS)
bool allowDaytimeShutdown = true;
#elif defined(FS_TOWN_CENTER)
bool allowDaytimeShutdown = true;
#elif defined(FS_ROTOFUEGO)
bool allowDaytimeShutdown = true;
#else
bool allowDaytimeShutdown = false;
#endif
#define FIVE_MINUTES (300) // 5 Minutes in seconds
#define TEN_MINUTES (600) // 10 Minutes in seconds
#define THIRTY_MINUTES (1800) // 10 Minutes in seconds
#ifdef FS_TOWN_CENTER
#define DORMANT_TIME_LIMIT (300) // 5 Minutes in seconds
#elif FS_ROTOFUEGO
#define DORMANT_TIME_LIMIT (300) // 5 Minutes in seconds
#else
#define DORMANT_TIME_LIMIT (1200) // 20 Minutes in seconds
#endif

#ifdef TEENSY
towerPatterns Control;
uint32_t heartbeatTimestampStart;
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

//! Prototypes
unsigned long readEepromLong(const int startByte);
void updateDisplay();
void setupRadio();
void setupTimers();
void setStartupPattern();
void sendHeartbeat();
void processAuxCommands();
void processIncoming();
void processRXResponse();
void pulseStatusLED(uint8_t red, uint8_t green, uint8_t blue, int duration, bool fade);

//! Initialize the system
void setup() {
  suicidePin = 5;
  commandPatternPin = 6;
  Control.FS_BREATHE_MULTIPLIER = 50.0;
  if (legacyPro) {
	batteryVoltagePin = 1;
	Control.randomSeedPin = 0;
	Control.audioAnalogPin = 0;
  } else {
	batteryVoltagePin = 0;
	Control.randomSeedPin = 8;
#ifdef TEENSY
	batteryVoltagePin = 15;
	Control.audioAnalogPin = AUDIO_INPUT_PIN;
	commandSpeedUpPin = 10;
	commandIntensityPin = 11;
	commandSpeedDownPin = 12;
	commandOffPin = 13;
	pinMode(AUDIO_INPUT_PIN, INPUT);
	analogReadResolution(ANALOG_READ_RESOLUTION);
	analogReadAveraging(ANALOG_READ_AVERAGING);
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
  pinMode(aux1pin, INPUT);
  pinMode(aux2pin, INPUT);
  pinMode(aux3pin, INPUT);
  pinMode(aux4pin, INPUT);
  // Initialize client heartbeat timestamp
  heartbeatTimestamp = millis();
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
void serialEvent3() {
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
#ifdef TEENSY
  HardwareSerial *radioSerial = &Serial1;
#else
  HardwareSerial *radioSerial = &Serial;
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
  pulseStatusLED (50, 0, 0, 500, true);
  delay (500);
  uint8_t dataGreen[] = {FS_ID_FULL_COLOR, 1, 0, 255, 0, 255};
  Control.initializePattern(dataGreen, 6);
  pulseStatusLED (0, 50, 0, 500, true);
  delay (500);
  uint8_t dataBlue[] = {FS_ID_FULL_COLOR, 1, 0, 0, 255, 255};
  Control.initializePattern(dataBlue, 6);
  pulseStatusLED (0, 0, 50, 500, true);
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
  //uint8_t data[] = {FS_ID_DANCING, 100, 200, 0, 40, 0};
  Control.triggerPattern = 22;
  uint8_t data[] = {FS_ID_RADIO_TOWER, 200, 0, 200, 0, 120};
#elif FS_TOWN_CENTER
  uint8_t data[] = {FS_ID_RADIO_TOWER, 0, 228, 0, 0, 228};
#elif defined(FS_REEDS) || defined(FS_WINDFLOWERS)
  uint8_t data[] = {FS_ID_RADIO_TOWER, 0, 228, 0, 0, 228};
#elif FS_TOWER_EYE
  //uint8_t data[] = {FS_ID_POOF_2, 0, 228, 0, 0, 228};
  uint8_t data[] = {FS_ID_MATRIX, 170, 255, 255, 255, 255};
#elif FS_DRESS
  //uint8_t data[] = {FS_ID_POOF_2, 0, 228, 0, 0, 228};
  uint8_t data[] = {FS_ID_RAINBOW_CHASE, 250, 200, 100, 130, 150};
#else
  uint8_t data[] = {FS_ID_CYLON, 51, 255, 0, 153, 255, 0};
  //uint8_t data[] = {FS_ID_SPECTRUM_ANALYZER, 128, 200, 200, 200, 128};
  //uint8_t data[] = {FS_ID_RADIO_TOWER, 200, 0, 200, 0, 120};
  //uint8_t data[] = {FS_ID_RAINBOW_CHASE, 250, 200, 100, 130, 150};
  //uint8_t data[] = {FS_ID_CYLON_VERTICAL, 200, 0, 200, 0, 120};
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
    if (ppsGap > FRAME_LENGTH && ppsGap <= 1000 + FRAME_LENGTH) {
      while (millis() - lastPPS < 1000) {}
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
  processAuxCommands();
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
#ifdef TEENSY
  Serial.print("lat:");
  Serial.print(lat / 100000);
  Serial.print(", lon:");
  Serial.print(lon / 100000);
  Serial.print(", time:");
  if (clock.hours < 10) Serial.print ("0");
  Serial.print(clock.hours); Serial.print(":"); 
  if (clock.minutes < 10) Serial.print ("0");
  Serial.print(clock.minutes); Serial.print(":"); 
  if (clock.seconds < 10) Serial.print ("0");
  Serial.println(clock.seconds);
#endif
}

//! Update the status display
void updateDisplay() {
#ifdef TEENSY
  led.update();
  if (lowBatteryWarningTime != 0 && (frameNumber == 0 || frameNumber == 29)) {
	pulseStatusLED (40, 0, 0, 100, true);
  } else if (frameNumber == 0) {
    unsigned long timeStamp = millis();
    if (gps2rtc.last_sentence_receipt == 0 || timeStamp - gps2rtc.last_sentence_receipt > GPS_DISPLAY_TIME) {
      // No NMEA data
      pulseStatusLED (40, 0, 0, 500, true);
    } else if (gps2rtc.lastPPSTime == 0 || timeStamp - gps2rtc.lastPPSTime > GPS_DISPLAY_TIME) {
      // NMEA data but no 1pps
      pulseStatusLED (40, 40, 0, 500, true);
    } else {
      // NMEA data and 1pps
      if (clock.seconds % 10 == 0) pulseStatusLED (0, 40, 40, 500, true);
      else pulseStatusLED (0, 40, 0, 500, true);
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
#ifdef NOT_EMBEDDED
    unsigned long currentTime = millis() / 1000;
#else
    unsigned long currentTime = gps2rtc.gps_time;
#endif
    if (gps2rtc.gps_time > 0) {
      clock.hours = currentTime / 3600;
      clock.minutes = (int) (((float) currentTime / 3600.0 - clock.hours) * 60);
      clock.seconds = currentTime - clock.hours * 3600 - clock.minutes * 60;
      Control.clock = clock;
      bool dormant = false;
#ifdef FS_TOWER
      dormant = currentTime - lastMessageReceipt > DORMANT_TIME_LIMIT && currentTime % TEN_MINUTES == 0 && !Control.animations.isAnimating;
#elif FS_ROTOFUEGO
      dormant = currentTime - lastMessageReceipt > DORMANT_TIME_LIMIT && currentTime % FIVE_MINUTES == 0 && !Control.animations.isAnimating;
#elif FS_TOWN_CENTER
      dormant = currentTime - lastMessageReceipt > DORMANT_TIME_LIMIT && currentTime % THIRTY_MINUTES == 0 && !Control.animations.isAnimating;
#endif
      bool inTheZone = OnTime < OffTime ? (currentTime > OnTime) && (currentTime < OffTime) : (currentTime > OnTime) || (currentTime < OffTime);
      if (daytimeShutdown) {
	pulseStatusLED (50, 0, 50, 100, false);
	if (Control.pattern != FS_ID_OFF) {
	  uint8_t data[] = {FS_ID_OFF};
	  Control.initializePattern(data, 1);
	}
	if (inTheZone) {
	  daytimeShutdown = false;
	  uint8_t data[] = {FS_ID_FULL_COLOR, 100, 0, 50, 200, 170}; // Aquamarine at startup
	  Control.initializePattern(data, 6);
	}
      } else {
	if (!inTheZone && allowDaytimeShutdown) {
	  uint8_t data[] = {FS_ID_OFF, 100, 100, 100, 100, 100};
	  Control.initializePattern(data, 6);
	  daytimeShutdown = true;
	  Control.animations.isAnimating = false;
	} else if (dormant) {
	  uint8_t data[] = {FS_ID_ANIMATE_1, 100, 100, 100, 100, 100};
	  Control.initializePattern(data, 2);
	}
      }
    }
  }
#endif
}

//! Process incoming aux commands
void processAuxCommands() {
#ifdef FS_TOWER_EYE
  if (digitalRead(aux1pin) == HIGH) {
    if (pooferStartControl > 0 && millis() - pooferStartControl > POOFER_DEBOUNCE) {
      Control.pooferControl.startPattern(0);
      //Control.pooferControl.poof(0, digitalRead(aux1pin));
      pooferStartControl = 0;
    } else {
      pooferStartControl = millis();
    }
  } else {
    Control.pooferControl.poof(0, LOW);
  }
  if (digitalRead(aux2pin) == HIGH) {
    if (pooferStartControl > 0 && millis() - pooferStartControl > POOFER_DEBOUNCE) {
      Control.pooferControl.startPattern(1);
      //Control.pooferControl.poof(1, digitalRead(aux2pin));
      pooferStartControl = 0;
    } else {
      pooferStartControl = millis();
    }
  } else {
    Control.pooferControl.poof(1, LOW);
  }
  if (digitalRead(aux3pin) == HIGH) {
    if (pooferStartControl > 0 && millis() - pooferStartControl > POOFER_DEBOUNCE) {
      Control.pooferControl.startPattern(2);
      pooferStartControl = 0;
    } else {
      pooferStartControl = millis();
    }
  }
  if (digitalRead(aux4pin) == HIGH) {
    if (pooferStartControl > 0 && millis() - pooferStartControl > POOFER_DEBOUNCE) {
      Control.pooferControl.startPattern(3);
      pooferStartControl = 0;
    } else {
      pooferStartControl = millis();
    }
  }
#endif	
}

//! Process incoming radio commands
void processIncoming() {
  int commandPatternMode = clickCount(commandPatternPin);
  int commandOffMode = 0;
  int commandSpeedUpMode = clickCount(commandSpeedUpPin);
  int commandSpeedDownMode = clickCount(commandSpeedDownPin);
  int commandIntensityMode = clickCount(commandIntensityPin);
  xbee.readPacket();
  if (xbee.getResponse().isAvailable() && !powerShutdown) {
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE || xbee.getResponse().getApiId() == ZB_TX_REQUEST) {
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
#ifdef SERIAL_DIAGNOSTICS
    Serial.println ("Suicide");
#endif
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
  int apiId = xbee.getResponse().getApiId();
  if (apiId == ZB_TX_REQUEST) {
    // Probably a transparent interface, e.g. Bluetooth
    int TxRequestFrameHeaderLength = 13;
    data = xbee.getResponse().getFrameData() + TxRequestFrameHeaderLength;
    dataLength = xbee.getResponse().getFrameDataLength() - TxRequestFrameHeaderLength;
  } else {
    xbee.getResponse().getZBRxResponse(rxResponse);
    // TODO: Check data length for potential errors
    data = rxResponse.getData();
    dataLength = rxResponse.getDataLength();
  }
  pulseStatusLED (50, 25, 0, 100, false);
  frameCount = 0;
  if (data[0] == FS_ID_PRISM_DISTANCE && data[1] == 128) {
    heartbeatPeriod = hifreqHeartbeatPeriod;
  } else {
    heartbeatPeriod = defaultHeartbeatPeriod;
  }
#ifdef TEENSY
  if (!daytimeShutdown) {
#ifdef NOT_EMBEDDED
    lastMessageReceipt = gps2rtc.gps_time;
#else
    lastMessageReceipt = millis() / 1000;
#endif
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
	pulseStatusLED (0, 0, 40, 100, false);
	heartbeatCount++;
	if (heartbeatCount % FULL_HEARTBEAT_PERIOD != 0) {
	  uint8_t heartbeatShortPayload[] = {FS_ID_MESSAGE_TYPE_SHORT_VERSION, versionId};  // Short heartbeat
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
		  if (currentTimestamp - lowBatteryTime > lowBatteryTimeThreshold) {
			powerShutdown = true;
		  }
		}
	  } else if (batteryVoltage < batteryWarningLimit) {
		if (lowBatteryWarningTime == 0) {
		  lowBatteryWarningTime = currentTimestamp;
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

void pulseStatusLED(uint8_t red, uint8_t green, uint8_t blue, int duration, bool fade) {
#ifdef TEENSY
  led.pulse(red, green, blue, duration, fade);
  led.update();
#endif
}


#include "gps2rtc.h"

TinyGPS gps;

// Initialization of static variables.
bool GPS2RTC::handle_interrupts = true;
bool GPS2RTC::receiving_serial_data = false;
unsigned long GPS2RTC::last_sentence_receipt = 0;
GPS_TO_RTC_STATE GPS2RTC::state = looking_for_serial_data;
HardwareSerial* GPS2RTC::gps_serial = NULL;
unsigned long GPS2RTC::lastPPSTime = 0;
double GPS2RTC::errSum = 0.0;
double GPS2RTC::lastErr = 0.0;
TinyGPS GPS2RTC::gps = gps;
int GPS2RTC::gps_1pps_pin = 0;
int GPS2RTC::gps_onoff_pin = 0;
unsigned long GPS2RTC::last_misalign = 0;
int8_t GPS2RTC::pit_reset_count = 0;
int16_t GPS2RTC::tpr_counter = 0;
int GPS2RTC::gps_time = 0;			        // Seconds since midnight decoded from the GPS receiver stream.
unsigned long GPS2RTC::latitude = 0;			// In decimal degrees * 100000.
unsigned long GPS2RTC::longitude = 0;			// In decimal degrees * 100000.

#include "gps2rtc.h"

TinyGPS gps;

// Initialization of static variables.
int GPS2RTC::gps_time = 0;			        // Seconds since midnight decoded from the GPS receiver stream.
bool GPS2RTC::receiving_serial_data = false;
unsigned long GPS2RTC::last_sentence_receipt = -1;
bool GPS2RTC::rtc_time_set = false;
bool GPS2RTC::handle_interrupts = true;
GPS_TO_RTC_STATE GPS2RTC::state = looking_for_serial_data;
unsigned long GPS2RTC::latitude = 0;			// In decimal degrees * 100000.
unsigned long GPS2RTC::longitude = 0;			// In decimal degrees * 100000.
HardwareSerial* GPS2RTC::gps_serial = NULL;
TinyGPS GPS2RTC::gps = gps;
int GPS2RTC::gps_1pps_pin = 0;
 

#include "gps2rtc.h"

TinyGPS gps;

// Initialization of static variables.
bool GPS2RTC::handle_interrupts = true;
bool GPS2RTC::receiving_serial_data = false;
unsigned long GPS2RTC::last_sentence_receipt = -1;
bool GPS2RTC::rtc_time_set = false;
bool GPS2RTC::rtc_waiting_for_tcr_reset = false;
bool GPS2RTC::rtc_compensation_set = false;
GPS_TO_RTC_STATE GPS2RTC::state = looking_for_serial_data;
HardwareSerial* GPS2RTC::gps_serial = NULL;
TinyGPS GPS2RTC::gps = gps;
int GPS2RTC::gps_1pps_pin = 0;
uint8_t GPS2RTC::rtc_reset_count = 0;
int GPS2RTC::rtc_comp_time = 0;
int16_t GPS2RTC::tpr_counter = 0;
int GPS2RTC::gps_time = 0;			        // Seconds since midnight decoded from the GPS receiver stream.
unsigned long GPS2RTC::latitude = 0;			// In decimal degrees * 100000.
unsigned long GPS2RTC::longitude = 0;			// In decimal degrees * 100000.

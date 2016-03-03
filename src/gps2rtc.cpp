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

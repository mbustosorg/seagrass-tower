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

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

extern Adafruit_SSD1306 display;

void setupOLEDdisplay();
void clearDisplay();
void displayOperatingDetails(int pattern, int timeStamp, int frameRate, unsigned long rtcTime, float lat, float lon, int16_t compensation);
void displayTiltParameters(int hue, int saturation, bool isShaking, bool isCalibrated);
void displayGPSdata(float lat, float lon, unsigned long time, int16_t compensation);
void displayMessage(const char* message);

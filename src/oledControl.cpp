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

#include "oledControl.h"

#define OLED_DC 21
#define OLED_CS 19
#define OLED_CLK 22
#define OLED_MOSI 23
#define OLED_RESET 20

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//! Initialize the OLED Display
void setupOLEDdisplay() {
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
}

//! Clear the display
void clearDisplay() {
  display.clearDisplay();   // clears the screen and buffer
}

//! Display the current operating details
void displayOperatingDetails(int pattern, int timeStamp, int frameRate, unsigned long rtcTime, float lat, float lon, int16_t compensation) {
  unsigned long days = rtcTime / (3600 * 24);
  unsigned long running = rtcTime - days * 3600 * 24;
  unsigned long hours = running / 3600;
  unsigned long minutes = ((float) running / 3600.0 - hours) * 60.0;
  unsigned long seconds = running - hours * 3600 - minutes * 60;
  if (seconds == 60) {
	seconds = 0;
	minutes = minutes + 1;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Patt:");
  display.println(pattern);
  display.print("Time:");
  display.print(timeStamp);
  display.println("s");
  display.print("FRat:");
  display.print(frameRate);
  display.println("Hz");
  display.print("RTC:");
  if (hours < 10) display.print ("0");
  display.print(hours); display.print(":"); 
  if (minutes < 10) display.print ("0");
  display.print(minutes); display.print(":"); 
  if (seconds < 10) display.print ("0");
  display.println(seconds); 
  display.print("lat:");
  display.println(lat);
  display.print("lon:");
  display.println(lon);
  display.print("comp:");
  display.println(compensation);
  display.display();
}

//! Display tilt parameters
void displayTiltParameters(int hue, int saturation, bool isShaking, bool isCalibrated) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("hue:");
  display.println(hue);
  display.print("sat:");
  display.println(saturation);
  if (isCalibrated) {
	display.print("CAL");
  }
  if (isShaking) {
	display.print("SHK");
  }
  display.display();  
}

//! Display GPS data
void displayGPSdata(float lat, float lon, unsigned long time, int16_t compensation) {
  unsigned long days = time / (3600 * 24);
  unsigned long running = time - days * 3600 * 24;
  unsigned long hours = running / 3600;
  unsigned long minutes = ((float) running / 3600.0 - hours) * 60.0;
  unsigned long seconds = running - hours * 3600 - minutes * 60;
  if (seconds == 60) {
	seconds = 0;
	minutes = minutes + 1;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("lat:");
  display.println(lat);
  display.print("lon:");
  display.println(lon);
  display.print("time:");
  if (hours < 10) display.print ("0");
  display.print(hours); display.print(":"); 
  if (minutes < 10) display.print ("0");
  display.print(minutes); display.print(":"); 
  if (seconds < 10) display.print ("0");
  display.println(seconds); 
  display.print("comp:");
  display.println(compensation);
  display.display();  
}

//! Display message
void displayMessage (const char* message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(message);
  display.display();  
}

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


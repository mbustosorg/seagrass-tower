//
//  oledControl.cpp
//
//  $Date: 2013-01-10 23:46:38 -0800 (Thu, 10 Jan 2013) $
//  $Rev: 811 $
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
void displayOperatingDetails(int pattern, int timeStamp, int frameRate, int temp) {
  display.clearDisplay();
  display.setTextSize(2);
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
  display.print("Temp:");
  display.println(temp);
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

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


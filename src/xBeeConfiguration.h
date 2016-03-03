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

#ifndef xBeeConfiguration_h
#define xBeeConfiguration_h

#include <XBee.h>

const uint32_t baudRates[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
const uint8_t numberOfBaudRates = 8;

//! XBee configuration class
/*! 
 *  Class to facilitate programmatic
 *  configuration of XBee radios.
 */
class xBeeConfiguration {
	
public:
    xBeeConfiguration(); 
    ~xBeeConfiguration();

    void setRadio(XBee* anXbeeRadio); 

    bool connected();
    bool persistConfiguration();
    
    uint32_t panID();
    uint32_t baudRate();

    bool setPanID(uint32_t newPanID);
    bool setBaudRate(uint32_t newBaudRate);
    
private:
    uint8_t indexForBaudRate(uint32_t desiredRate);
    bool sendAndGetAck(AtCommandRequest atRequest);
    virtual void outputMessage(const char * newMessage, bool withEndl);
    virtual void outputInteger(int newInt, bool withEndl);
    
    //  XBee Attributes & Commands
    XBee*        xbeeRadio;
};

#endif

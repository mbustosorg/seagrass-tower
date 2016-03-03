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

#include "furSwarmMemberLinux.h"
#include "furSwarmPatternConst.h"
#include "embeddedInterface.h"

// System configuration
#define versionId 0x14
#ifdef FS_VEST
const uint8_t memberType = 0x01; // Vest
#elif FS_HAT
const uint8_t memberType = 0x02; // Hat
#elif FS_TOWER
const uint8_t memberType = 0x03; // Tower
#elif FS_TOWN_CENTER
const uint8_t memberType = 0x04; // Town Center
#endif

// Heartbeat message layout
uint8_t heartbeatPayload[] = {
  0x01,       // Byte 0: Message Type ID (1 byte)
  versionId,  // Byte 1: Version ID (1 byte)
  0,          // Byte 2: Frame location (2 bytes)
  0,
  0,          // Byte 4: Current Pattern
  0,          // Byte 5: Battery Voltage (2 bytes)
  0,
  0,          // Byte 7: Frame Rate (1 byte)
  memberType, // Byte 8: Member type
  0,          // Byte 9: Failed messages (2 bytes)
  0
};
#define frameCountPosition (2)
#define patternPosition (4)
#define voltagePosition (5)
#define frameRatePosition (7)
#define failedMessagePosition (9)

// FurSwarm member data
uint16_t frameCount = 0;
unsigned long frameRateCount = 0;
int frameStarted = 0;

void furSwarmMemberLinux::setup(){

  platforms.clear();
#ifdef FS_TOWER_EYE
  int numberOfTowers = 1;
#elif FS_TOWER
  int numberOfTowers = 10;
#elif FS_TOWN_CENTER
  int numberOfTowers = 8;
#else
  int numberOfTowers = 1;
#endif
  for(int i = 0; i < numberOfTowers; i++){
    platforms.push_back(new towerPatterns);
  }
  for(int i = 0; i < platforms.size(); i++){
    platforms[i]->FS_BREATHE_MULTIPLIER = 50.0;
  }
  // Speed, Red, Green, Blue, Intensity
  uint8_t sourceData[] = {FS_ID_ANIMATE_1, 120, 130, 100, 130, 240, 0};
  //uint8_t sourceData[] = {FS_ID_BROKEN, 120, 130, 100, 130, 240, 0};
  //uint8_t sourceData[] = {FS_ID_SEARCHING_EYE, 10, 250, 100, 130, 250, 0};
  //uint8_t sourceData[] = {FS_ID_RAINBOW_CHASE, 120, 130, 100, 130, 240, 0};
  //uint8_t sourceData[] = {FS_ID_PONG, 60, 1, numberOfTowers, 130, 20};
  memcpy (data, sourceData, 7);
  for(int i = 0; i < platforms.size(); i++){
    if (data[0] == FS_ID_PONG) {
      data[2] = i + 1;
    }
        
    platforms[i]->initializePattern(data, 7);
#ifdef FS_TOWER
    platforms[i]->latitude = 100 * sin (2 * PI * i / 10);
    platforms[i]->longitude = 100 * cos (2 * PI * i / 10);
#endif
  }
}

void furSwarmMemberLinux::update(){
  for(int i = 0; i < platforms.size(); i++){
    platforms[i]->continuePatternDisplay();
  }
  frameRateCount++;
  frameRateCount++; // To simulate 60Hz
}

void furSwarmMemberLinux::draw(){
}


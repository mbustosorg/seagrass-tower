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

// Heartbeat message layout
uint8_t heartbeatPayload[] = {
  0x01,       // Byte 0: Message Type ID (1 byte)
  0,          // Byte 1: Version ID (1 byte)
  0,          // Byte 2: Frame location (2 bytes)
  0,
  0,          // Byte 4: Current Pattern
  0,          // Byte 5: Battery Voltage (2 bytes)
  0,
  0,          // Byte 7: Frame Rate (1 byte)
  0,          // Byte 8: Member type
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

furSwarmMemberLinux::furSwarmMemberLinux() {
  test = false;
}

void furSwarmMemberLinux::setup(){
  platforms.clear();
  int numberOfTowers = 1;
  for(int i = 0; i < numberOfTowers; i++){
    platforms.push_back(new towerPatterns);
  }
  for(unsigned int i = 0; i < platforms.size(); i++){
    platforms[i]->FS_BREATHE_MULTIPLIER = 50.0;
  }
  uint8_t command[] = {FS_ID_RAINBOW_CHASE, 10, 130, 100, 130, 240, 0};
  setPattern(command);
}

void furSwarmMemberLinux::setPattern(const uint8_t command[]) {
  // Speed, Red, Green, Blue, Intensity
  memcpy (data, command, 7);
  for(unsigned int i = 0; i < platforms.size(); i++){
    if (data[0] == FS_ID_PONG) {
      data[2] = i + 1;
    }
    platforms[i]->initializePattern(data, 7);
  }
}

void furSwarmMemberLinux::update(){
  for(unsigned int i = 0; i < platforms.size(); i++){
    platforms[i]->continuePatternDisplay();
  }
}

void furSwarmMemberLinux::draw(){
}


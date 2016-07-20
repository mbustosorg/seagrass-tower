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
#include <plog/Log.h>
#include <string.h>

// Heartbeat message layout
uint8_t heartbeatPayload[11] = {
    0x01,       // Byte 0: Message Type ID (1 byte)
    0x01,       // Byte 1: Version ID (1 byte)
    0,       // Byte 2: Frame location (2 bytes)
    0,
    0,       // Byte 4: Current Pattern
    0,       // Byte 5: Battery Voltage (2 bytes)
    0,
    0,       // Byte 7: Frame Rate (1 byte)
    0,       // Byte 8: Member type
    0,       // Byte 9: Failed messages (2 bytes)
    0
};
char heartbeatPatternName[MAX_PATTERN_NAME_LENGTH];
#define heartbeatPayloadSize (11)
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
    heartbeatPayload[8] = 1;
}

void furSwarmMemberLinux::setup(){
    platform = new towerPatterns;
    platform->FS_BREATHE_MULTIPLIER = 50.0;
}

void furSwarmMemberLinux::setPattern(const uint8_t command[]) {
    // Speed, Red, Green, Blue, Intensity
    memcpy (data, command, 7);
    platform->initializePattern(data, 7);
}

void furSwarmMemberLinux::update() {
    platform->continuePatternDisplay();
}

void furSwarmMemberLinux::updateHeartbeatMessage(HeartbeatMessage* heartbeat) {
    heartbeat->set_messagetypeid(heartbeatPayload[0]);
    heartbeat->set_versionid(heartbeatPayload[1]);
    heartbeat->set_currentpattern(platform->pattern);
    heartbeat->set_red(platform->unadjustedRed);
    heartbeat->set_green(platform->unadjustedGreen);
    heartbeat->set_blue(platform->unadjustedBlue);
    heartbeat->set_speed(platform->patternSpeedLevel);
    heartbeat->set_intensity(platform->unadjustedIntensity);
    heartbeat->set_membertype(heartbeatPayload[8]);
    heartbeat->set_currentpatternname(patternNames[platform->pattern]);
}

void furSwarmMemberLinux::handleMessage(const CommandMessage command, uint8_t* buffer, int* messageSize) {
    
    FabricWrapperMessage wrapperMessage;
    if (command.command() == command.PROTOBUF_HEARTBEAT) {
        LOG_DEBUG << "Received heartbeat request";
        updateHeartbeatMessage(wrapperMessage.mutable_heartbeat());
    } else if (command.command() == command.PROTOBUF_PATTERN_NAMES) {
        LOG_INFO << "Received pattern names request";
        PatternNamesMessage* patternName = wrapperMessage.mutable_patternnames();
        for (int i = 1; i < FS_ID_MAX_ID_COUNT; i++) {
            patternName->add_name(fabricPatternNames[i]);
        }
    } else if (command.command() == command.PROTOBUF_OPC_DISCONNECT) {
        LOG_INFO << "Received OPC Disconnect request";
        platform->leds.disconnect();
    } else if (command.command() == command.PROTOBUF_OPC_CONNECT) {
        LOG_INFO << "Received OPC Connect request";
        platform->leds.connect();
    }
    wrapperMessage.SerializeToArray(buffer, wrapperMessage.ByteSize());
    *messageSize = wrapperMessage.ByteSize();
}

void furSwarmMemberLinux::handlePatternCommand(const PatternCommand patterncommand, uint8_t* buffer, int* messageSize){

    if (patterncommand.patternnumber() <= 0 || patterncommand.patternnumber() > FS_ID_MAX_ID_COUNT) {
        LOG_WARNING << "Received invalid pattern command request - " << patterncommand.patternnumber();
    } else {
        LOG_INFO << "Received pattern command request - " << patternNames[patterncommand.patternnumber()];
        
        FabricWrapperMessage wrapperMessage;
        
        uint8_t command[7];
        command[0] = (uint8_t)patterncommand.patternnumber();
        command[1] = (uint8_t)patterncommand.speed();
        command[2] = (uint8_t)patterncommand.red();
        command[3] = (uint8_t)patterncommand.green();
        command[4] = (uint8_t)patterncommand.blue();
        command[5] = (uint8_t)patterncommand.intensity();
        command[6] = 0;
        
        setPattern(command);
        
        LOG_INFO << "Pattern Set - " << patternNames[platform->pattern];

        updateHeartbeatMessage(wrapperMessage.mutable_heartbeat());
        wrapperMessage.SerializeToArray(buffer, wrapperMessage.ByteSize());
        *messageSize = wrapperMessage.ByteSize();
    }
}

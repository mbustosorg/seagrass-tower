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
#include "protobuf/FabricProtos.pb.h"
#include <arpa/inet.h>
#include <chrono>
#include <csignal>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS (30)

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

using namespace std;

int counter = 0;
const long long framePeriod = 16667;
const char* logFileName = "logs/furSwarmLinux.log";
int master_socket, addrlen, client_socket[30];
struct sockaddr_in address;
const char *message = "FUR_SWARM_MEMBER";
uint8_t buffer[1025];
furSwarmMemberLinux* member;

void setupServer() {
    int opt = 1;
    
    const char* envPort = getenv("FABRIC_LED_PORT");
    int port = 0;
    if (envPort) port = atoi(envPort);
    else port = 8801;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }
    if ((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
        LOG_WARNING << "socket failed";
        exit(EXIT_FAILURE);
    }
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        LOG_WARNING << "setsockopt";
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (::bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        LOG_WARNING << "bind failed";
        exit(EXIT_FAILURE);
    }
    LOG_INFO  << "Listener on port " << port;
    if (listen(master_socket, 3) < 0) {
        LOG_WARNING << "could not set listen";
        exit(EXIT_FAILURE);
    }
    addrlen = sizeof(address);
    LOG_INFO << "Waiting for connections ...";
}

void updateMember() {
    member->update();
    counter++;
    if (counter == 1) {
        uint8_t command[] = {FS_ID_FULL_COLOR, 255, 255, 0, 0, 255, 0};
        member->setPattern(command);
        LOG_INFO << "Red Initialization";
    } else if (counter == 120) {
        uint8_t command[] = {FS_ID_FULL_COLOR, 255, 0, 255, 0, 255, 0};
        member->setPattern(command);
        LOG_INFO << "Green Initialization";
    } else if (counter == 180) {
        uint8_t command[] = {FS_ID_FULL_COLOR, 255, 0, 0, 255, 255, 0};
        member->setPattern(command);
        LOG_INFO << "Blue Initialization";
    } else if (counter == 185) {
        uint8_t command[] = {FS_ID_RAINBOW_CHASE, 10, 130, 100, 130, 240, 0};
        member->setPattern(command);
        LOG_INFO << "RAINBOW_CHASE";
    }
}

int main() {
    
    const char * buildTime = STRINGIZE_VALUE_OF(__BUILD_TIME);
    const char * buildNumber = STRINGIZE_VALUE_OF(__BUILD_NUMBER);
    int valread, sd, max_sd, new_socket;
    fd_set readfds;
    
    signal(SIGPIPE, SIG_IGN);
    
    static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(logFileName, 1000000, 3);
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::info, &fileAppender).addAppender(&consoleAppender);
    
    LOG_INFO << "Logging to -> " << logFileName;
    LOG_INFO << "Build time  : " << buildTime;
    LOG_INFO << "Build number: " << buildNumber;
    
    member = new furSwarmMemberLinux();
    member->setup();
    setupServer();
    
    struct timeval tv = {0, framePeriod};
    
    while (1) {
        auto start = chrono::high_resolution_clock::now();
        updateMember();
        auto elapsed = chrono::high_resolution_clock::now() - start;
        long long updateLength = chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        
        tv.tv_sec = 0;
        tv.tv_usec = framePeriod - updateLength;
        
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
        for (int i = 0 ; i < MAX_CLIENTS ; i++) {
            sd = client_socket[i];
            if (sd > 0) FD_SET(sd , &readfds);
            if (sd > max_sd) max_sd = sd;
        }
        
        int selectResult = select(max_sd + 1, &readfds, NULL, NULL, &tv);
        
        if (selectResult > 0) {
            if (FD_ISSET(master_socket, &readfds)) {
                if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                    LOG_WARNING << "Could not accept new client";
                    exit(EXIT_FAILURE);
                }
                LOG_INFO << "New connection, socket fd:" << new_socket << ", ip: " << inet_ntoa(address.sin_addr) << ", port: " << ntohs(address.sin_port);
                memcpy(buffer, buildTime, strlen(buildTime));
                memcpy(buffer + strlen(buildTime), buildNumber, strlen(buildNumber));
                
                // Send version and build time info
                FabricWrapperMessage wrapperMessage;
                
                WelcomeMessage* welcomeMessage = wrapperMessage.mutable_welcome();
                welcomeMessage->set_buildtime(buildTime);
                welcomeMessage->set_version(buildNumber);
                wrapperMessage.SerializeToArray(buffer, wrapperMessage.ByteSize());
                
                if (send(new_socket, buffer, wrapperMessage.ByteSize(), 0) != wrapperMessage.ByteSize()) {
                    LOG_WARNING << "Could not send welcome message";
                }
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_socket[i] == 0) {
                        client_socket[i] = new_socket;
                        LOG_INFO << "Adding to list of sockets as " << i;
                        break;
                    }
                }
            }
            for (int i = 0; i < MAX_CLIENTS; i++) {
                sd = client_socket[i];
                if (FD_ISSET(sd , &readfds)) {
                    long received = read(sd , buffer, 1024);
                    if (received == 0L) {
                        getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                        LOG_INFO << "Host " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << " disconnected";
                        close(sd);
                        client_socket[i] = 0;
                    } else {
                        //LOG_DEBUG << "From " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << " - " << buffer;
                        FabricWrapperMessage wrapperMessage;
                        wrapperMessage.ParseFromArray(buffer, (int)received);
                        if (wrapperMessage.has_command()) {
                            member->handleMessage(wrapperMessage.command(), buffer, &valread);
                            send(sd, buffer, valread, 0);
                        } else if (wrapperMessage.has_patterncommand()) {
                            member->handlePatternCommand(wrapperMessage.patterncommand(), buffer, &valread);
                            send(sd, buffer, valread, 0);
                        }
                    }
                }
            }
        }
    }
    LOG_INFO << "EXITING";
}


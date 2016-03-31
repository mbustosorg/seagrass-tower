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

#define PORT 8888
#define MAX_CLIENTS (30)

using namespace std;

int counter = 0;
const long long framePeriod = 16667;
const char* logFileName = "furSwarmLinux.log";
int master_socket, addrlen, client_socket[30];
struct sockaddr_in address;
const char *message = "FUR_SWARM_MEMBER";
char buffer[1025];
furSwarmMemberLinux* member;

void signalHandler(int signum) {
  LOG_WARNING << "Interrupt signal (" << signum << ") received";
  exit(signum);  
}

void setupServer() {
  int opt = 1;
      
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
  address.sin_port = htons( PORT );
  if (::bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    LOG_WARNING << "bind failed";
    exit(EXIT_FAILURE);
  }
  LOG_INFO  << "Listener on port " << PORT;
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
  if (counter % 600 == 0) {
    uint8_t command[] = {FS_ID_RAINBOW_CHASE, 10, 130, 100, 130, 240, 0};
    member->setPattern(command);
    LOG_INFO << "RAINBOW_CHASE";
  } else if (counter % 300 == 0) {
    uint8_t command[] = {FS_ID_CYLON, 150, 255, 130, 0, 250, 255};
    member->setPattern(command);
    LOG_INFO << "CYLON";
  }
}

int main() {

  int valread, sd, max_sd, new_socket;
  fd_set readfds;
  
  static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(logFileName, 1000000, 3);
  static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::info, &fileAppender).addAppender(&consoleAppender);

  LOG_INFO << "Logging to -> " << logFileName;
  
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
	if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
	  LOG_WARNING << "Could not send welcome message";
	}
	LOG_INFO << "Welcome message sent successfully";
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
	  if ((valread = read( sd , buffer, 1024)) == 0) {
	    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
	    LOG_INFO << "Host " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << " disconnected";
	    close(sd);
	    client_socket[i] = 0;
	  } else {
	    LOG_INFO << "From " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << " - " << buffer;
	    member->handleMessage(buffer, &valread);
	    send(sd, buffer, valread, 0);
	  }
	}
      }
    }
  }
  LOG_INFO << "EXITING";
}
      

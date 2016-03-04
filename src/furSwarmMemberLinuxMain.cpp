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
#include <chrono>
#include <stdio.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <plog/Log.h>

const long long framePeriod = 16667;
const char* logFileName = "furSwarmLinux.log";

int main(){

  plog::init(plog::info, logFileName, 1000000, 3);
  printf("Logging to -> %s\n", logFileName);
  fflush(stdout);
  
  furSwarmMemberLinux* member = new furSwarmMemberLinux();
  member->setup();
  int counter = 0;
  
  struct timeval tv = {0, framePeriod};
  while  (select(0, NULL, NULL, NULL, &tv) == 0) {
    auto start = std::chrono::high_resolution_clock::now();
    member->update();
    counter++;
    if (counter % 600 == 0) {
      uint8_t command[] = {FS_ID_RAINBOW_CHASE, 10, 130, 100, 130, 240, 0};
      member->setPattern(command);
      LOG_INFO << "RAINBOW_CHASE";
    } else if (counter % 300 == 0) {
      uint8_t command[] = {FS_ID_CYLON, 250, 255, 100, 0, 250, 255};
      member->setPattern(command);
      LOG_INFO << "CYLON";
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long updateLength = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    tv.tv_usec = framePeriod - updateLength;
  }
}

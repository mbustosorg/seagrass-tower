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

const long long framePeriod = 16667;

int main(){
  furSwarmMemberLinux* member = new furSwarmMemberLinux();
  member->setup();
  int counter = 0;
  
  struct timeval tv = {0, framePeriod};
  while  (select(0, NULL, NULL, NULL, &tv) == 0) {
    auto start = std::chrono::high_resolution_clock::now();
    member->update();
    counter++;
    if (counter % 60 == 0) {
      printf(".");
      fflush(stdout);
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long updateLength = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    tv.tv_usec = framePeriod - updateLength;
  }
}

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

#include "towerPatterns.h"
#include <vector>
#include <list>

class furSwarmMemberLinux {
public:

  furSwarmMemberLinux();
  
  bool test;
  void setup();
  void setPattern(const uint8_t command[]);
  void update();
  void draw();
  void handleMessage(uint8_t * buffer, int * messageSize);
  
  towerPatterns* platform;
  
  uint8_t data[7];
  
};

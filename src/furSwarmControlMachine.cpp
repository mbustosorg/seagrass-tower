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

#include "furSwarmControlMachine.h"

#ifdef NOT_EMBEDDED
#include "embeddedInterface.h"
#endif

const unsigned long multiClickInterval = 400;
const int debounceDelay = 50;

//! Number of identified clicks
int clickCount(int pin) {
  int clickCounter = 0;
  if (digitalRead(pin) == LOW) {
	if (debouncePin(pin) == LOW) {
	  bool unClick = false;
	  for (unsigned long pauseInterval = millis(); millis() - pauseInterval < multiClickInterval; ) {
		if (digitalRead(pin) == HIGH) {
		  unClick = debouncePin(pin);
		  if (unClick) {
			clickCounter++;
			break;
		  }
		}
	  }
	  if (unClick) {
		int lastClickCounter = clickCounter;
		for (unsigned long pauseInterval = millis(); millis() - pauseInterval < multiClickInterval; ) {
		  clickCounter = clickCounter + clickCount(pin);
		  if (clickCounter != lastClickCounter) {
			break;
		  }
		}
	  } else {
		return 1;
	  }
	}
  }
  return clickCounter;
}

//! Debounce `pin', return if stable
bool debouncePin(int pin) {
  bool state;
  bool previousState;

  previousState = digitalRead(pin);
  for (int counter = 0; counter < debounceDelay; counter++) {
	delay (1);
	state = digitalRead(pin);
	if (state != previousState) {
	  counter = 0;
	  previousState = state;
	}
  }
  if (state == LOW) {
	return false;
  } else {
	return true;
  }
}


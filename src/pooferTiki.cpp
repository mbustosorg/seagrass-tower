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

#include "pooferTiki.h"
#ifdef NOT_EMBEDDED
#include "embeddedInterface.h"
#include <stdlib.h>
#else
#include "Arduino.h"
#endif

//! Create the poofer object
pooferTiki::pooferTiki() {
  pooferPins[0] = POOFER_TIKI_1_PIN;
  pinMode(POOFER_TIKI_1_PIN, OUTPUT);
  digitalWrite(POOFER_TIKI_1_PIN, LOW);

  runningPattern = PATTERNS_OFF;
  patternStartTime = 0;
  stepNumber = 0;

  patternTiki* patternsSetup = (patternTiki *) malloc(PATTERN_TIKI_COUNT * sizeof(patternTiki));
  patterns = patternsSetup;

  // Pattern 1
  patternTikiStep* pattern1steps = (patternTikiStep *) malloc(PATTERN_TIKI_1_COUNT * sizeof(patternTikiStep));
  pattern1steps[0] = {0, {HIGH}};
  pattern1steps[1] = {100, {LOW}};
  pattern1steps[2] = {500, {HIGH}};
  pattern1steps[3] = {600, {HIGH}};
  pattern1steps[4] = {1000, {HIGH}};
  pattern1steps[5] = {1100, {LOW}};
  pattern1steps[6] = {1300, {LOW}};
  pattern1steps[7] = {1400, {LOW}};  
  pattern1steps[8] = {2000, {LOW}};
  pattern1steps[9] = {2500, {LOW}};
  patterns[0] = {PATTERN_TIKI_1_COUNT, pattern1steps};

  // Pattern 2
  patternTikiStep* pattern2steps = (patternTikiStep *) malloc(PATTERN_TIKI_2_COUNT * sizeof(patternTikiStep));
  pattern2steps[0] = {0, {LOW}};
  pattern2steps[1] = {100, {LOW}};
  pattern2steps[2] = {200, {LOW}};
  pattern2steps[3] = {300, {LOW}};
  pattern2steps[4] = {400, {LOW}};
  pattern2steps[5] = {500, {LOW}};
  pattern2steps[6] = {600, {LOW}};
  pattern2steps[7] = {700, {LOW}};
  pattern2steps[8] = {800, {LOW}};
  pattern2steps[9] = {900, {LOW}};
  pattern2steps[10] = {1000, {LOW}};
  pattern2steps[11] = {1100, {LOW}};
  pattern2steps[12] = {1200, {LOW}};
  pattern2steps[13] = {1300, {LOW}};
  pattern2steps[14] = {1400, {LOW}};
  pattern2steps[15] = {1500, {LOW}};
  pattern2steps[16] = {1600, {LOW}};
  pattern2steps[17] = {1700, {LOW}};
  pattern2steps[18] = {1800, {LOW}};
  pattern2steps[19] = {1900, {LOW}};
  pattern2steps[20] = {2000, {LOW}};
  pattern2steps[21] = {2100, {LOW}};
  pattern2steps[22] = {2200, {LOW}};
  pattern2steps[23] = {2300, {LOW}};
  pattern2steps[24] = {2400, {LOW}};
  pattern2steps[25] = {2500, {LOW}};
  patterns[1] = {PATTERN_TIKI_2_COUNT, pattern2steps};

  // Pattern 3
  patternTikiStep* pattern3steps = (patternTikiStep *) malloc(PATTERN_TIKI_3_COUNT * sizeof(patternTikiStep));
  pattern3steps[0] = {0, {LOW}};
  pattern3steps[1] = {100, {LOW}};
  pattern3steps[2] = {200, {LOW}};
  pattern3steps[3] = {300, {LOW}};
  pattern3steps[4] = {400, {LOW}};
  pattern3steps[5] = {500, {LOW}};
  pattern3steps[6] = {600, {LOW}};
  pattern3steps[7] = {700, {LOW}};
  pattern3steps[8] = {800, {LOW}};
  pattern3steps[9] = {900, {LOW}};
  pattern3steps[10] = {1000, {LOW}};
  pattern3steps[11] = {1100, {LOW}};
  pattern3steps[12] = {1200, {LOW}};
  pattern3steps[13] = {1500, {LOW}};
  pattern3steps[14] = {2000, {LOW}};
  pattern3steps[15] = {2500, {LOW}};
  pattern3steps[16] = {3000, {LOW}};
  pattern3steps[17] = {3500, {LOW}};
  patterns[2] = {PATTERN_TIKI_3_COUNT, pattern3steps};

  // Pattern 4
  patternTikiStep* pattern4steps = (patternTikiStep *) malloc(PATTERN_TIKI_4_COUNT * sizeof(patternTikiStep));
  pattern4steps[0] = {0, {LOW}};
  pattern4steps[1] = {100, {LOW}};
  pattern4steps[2] = {200, {LOW}};
  pattern4steps[3] = {300, {LOW}};
  pattern4steps[4] = {400, {LOW}};
  pattern4steps[5] = {500, {LOW}};
  pattern4steps[6] = {600, {LOW}};
  pattern4steps[7] = {700, {LOW}};
  pattern4steps[8] = {800, {LOW}};
  pattern4steps[9] = {900, {LOW}};
  pattern4steps[10] = {1000, {LOW}};
  pattern4steps[11] = {1100, {LOW}};
  pattern4steps[12] = {1200, {LOW}};
  pattern4steps[13] = {1300, {LOW}};
  pattern4steps[14] = {1400, {LOW}};
  pattern4steps[15] = {1500, {LOW}};
  pattern4steps[16] = {2000, {LOW}};
  pattern4steps[17] = {2200, {LOW}};
  pattern4steps[18] = {2500, {LOW}};
  pattern4steps[19] = {2700, {LOW}};
  pattern4steps[20] = {2900, {LOW}};
  pattern4steps[21] = {3200, {LOW}};
  pattern4steps[22] = {3400, {LOW}};
  pattern4steps[23] = {3600, {LOW}};
  patterns[3] = {PATTERN_TIKI_4_COUNT, pattern4steps};
}

//! Destroy the object
pooferTiki::~pooferTiki() {
}

//! Create multi poofers
void pooferTiki::startPattern(int id) {
  if (id >= 0 && id < PATTERN_TIKI_COUNT) {
    if (runningPattern != id) {
      runningPattern = id;
      patternStartTime = millis();
      stepNumber = 0;
      for (int i = 0; i < POOFER_TIKI_COUNT; i++) {
	digitalWrite(pooferPins[i], patterns[runningPattern].steps[stepNumber].pooferState[i]);
      }
      stepNumber++;
    }
  }
}

//! Iterate the current pattern
void pooferTiki::iteratePattern() {
  if (runningPattern != PATTERNS_OFF) {
    if (stepNumber >= patterns[runningPattern].numberOfSteps) {
      runningPattern = PATTERNS_OFF;
      // Safety Shutdown
      for (int i = 0; i < POOFER_TIKI_COUNT; i++) {
	digitalWrite(pooferPins[i], LOW);
      }
    } else if (runningPattern >= 0) {
      unsigned long currentDiff = millis() - patternStartTime;
      if (currentDiff >= patterns[runningPattern].steps[stepNumber].stepStart) {
	for (int i = 0; i < POOFER_TIKI_COUNT; i++) {
	  digitalWrite(pooferPins[i], patterns[runningPattern].steps[stepNumber].pooferState[i]);
	}
	stepNumber++;
      }
    }
  }
}

//! Poof on command
void pooferTiki::poof(int id, int state) {
  if (runningPattern == PATTERNS_OFF || state == HIGH) {
    if (id >= 0 && id < POOFER_TIKI_COUNT) {
      runningPattern = PATTERNS_OFF;
      digitalWrite(pooferPins[id], state);
    }
  }
}




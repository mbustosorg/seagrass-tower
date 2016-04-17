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

#include "poofer.h"
#ifdef NOT_EMBEDDED
#include "embeddedInterface.h"
#include <stdlib.h>
#else
#include "Arduino.h"
#endif

//! Create the poofer object
poofer::poofer() {
  pooferPins[0] = POOFER_1_PIN;
  pooferPins[1] = POOFER_2_PIN;
  pinMode(POOFER_1_PIN, OUTPUT);
  digitalWrite(POOFER_1_PIN, LOW);
  pinMode(POOFER_2_PIN, OUTPUT);
  digitalWrite(POOFER_2_PIN, LOW);

  runningPattern = PATTERNS_OFF;
  patternStartTime = 0;
  stepNumber = 0;

  pattern* patternsSetup = (pattern *) malloc(PATTERN_COUNT * sizeof(pattern));
  patterns = patternsSetup;

  // Single Blast
  patternStep* pattern1steps = (patternStep *) malloc(PATTERN_1_COUNT * sizeof(patternStep));
  pattern1steps[0] = {0, {HIGH, HIGH}};
  pattern1steps[1] = {1000, {LOW, LOW}};
  patterns[0] = {PATTERN_1_COUNT, pattern1steps};

  // Alternating Blasts
  patternStep* pattern2steps = (patternStep *) malloc(PATTERN_2_COUNT * sizeof(patternStep));
  pattern2steps[0] = {0, {HIGH, LOW}};
  pattern2steps[1] = {500, {LOW, HIGH}};
  pattern2steps[2] = {1000, {HIGH, LOW}};
  pattern2steps[3] = {1500, {LOW, HIGH}};
  pattern2steps[4] = {2000, {LOW, LOW}};
  patterns[1] = {PATTERN_2_COUNT, pattern2steps};
}

//! Destroy the object
poofer::~poofer() {
}

//! Create multi poofers
void poofer::startPattern(int id) {
  if (id >= 0 && id < PATTERN_COUNT) {
    runningPattern = id;
    patternStartTime = millis();
    stepNumber = 0;
    for (int i = 0; i < POOFER_COUNT; i++) {
      digitalWrite(pooferPins[i], patterns[runningPattern].steps[stepNumber].pooferState[i]);
    }
    stepNumber++;
  }
}

//! Iterate the current pattern
void poofer::iteratePattern() {
  if (stepNumber >= patterns[runningPattern].numberOfSteps) {
    runningPattern = PATTERNS_OFF;
    // Safety Shutdown
    for (int i = 0; i < POOFER_COUNT; i++) {
      digitalWrite(pooferPins[i], LOW);
    }
  } else if (runningPattern >= 0) {
    unsigned long currentDiff = millis() - patternStartTime;
    if (currentDiff >= patterns[runningPattern].steps[stepNumber].stepStart) {
      for (int i = 0; i < POOFER_COUNT; i++) {
	digitalWrite(pooferPins[i], patterns[runningPattern].steps[stepNumber].pooferState[i]);
      }
      stepNumber++;
    }
  }
}

//! Poof on command
void poofer::poof(int id, int state) {
  if (runningPattern == PATTERNS_OFF || state == HIGH) {
    if (id >= 0 && id < POOFER_COUNT) {
      runningPattern = PATTERNS_OFF;
      digitalWrite(pooferPins[id], state);
    }
  }
}




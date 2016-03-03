DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWN_CENTER -DNOT_EMBEDDED

PRODUCT_NAME=linux
PROJECT_LOC=../../src/
VPATH=$(PROJECT_LOC)
OBJECT_DIR=./

CC=gcc
CXX=g++
CFLAGS=-c -Wall -std=gnu++0x
LDFLAGS=-lstdc++
INCLUDE=-I../../src/XBee

SOURCES=towerPatterns.cpp colorUtilities.cpp \
furSwarmPatterns.cpp towerAnimations.cpp \
towerBall.cpp ledDriver.cpp adxl335.cpp accelerometer.cpp \
embeddedInterface.cpp furSwarmMemberLinux.cpp furSwarmMemberLinuxMain.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=linux

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(DFLAGS) $(INCLUDE) $< -o $@

clean:
    #Clear previous...
	rm -v $(OBJECTS)
	rm -v *.d
	rm -v *.eep
	rm -v *.elf
	rm -v *.hex

DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWN_CENTER -DNOT_EMBEDDED

PRODUCT_NAME=linux
PROJECT_LOC=../../src/
VPATH=$(PROJECT_LOC)
OBJECT_DIR=./

CC=gcc
CXX=g++
CFLAGS=-c -Wall -std=gnu++0x
C_CFLAGS=-c -Wall
LDFLAGS=-lstdc++
INCLUDE=-I../../src/XBee -I../../src/plog-1.0.0/include -I../../src/opc

SOURCES=towerPatterns.cpp colorUtilities.cpp \
furSwarmPatterns.cpp towerAnimations.cpp \
towerBall.cpp ledDriver.cpp adxl335.cpp accelerometer.cpp \
embeddedInterface.cpp furSwarmMemberLinux.cpp furSwarmMemberLinuxMain.cpp
C_SOURCES=opc/opc_client.c

OBJECTS=$(SOURCES:.cpp=.o)
C_OBJECTS=$(C_SOURCES:.c=.o)

EXECUTABLE=linux

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(C_OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(C_OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(DFLAGS) $(INCLUDE) $< -o $@

.c.o:
	$(CXX) $(C_CFLAGS) $(DFLAGS) $(INCLUDE) $< -o $@

clean:
    #Clear previous...
	rm -v $(OBJECTS)
	rm -v *.d
	rm -v *.eep
	rm -v *.elf
	rm -v *.hex

DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWN_CENTER -DNOT_EMBEDDED

PRODUCT_NAME=furSwarmLinux
PROJECT_LOC=../../src/
VPATH=$(PROJECT_LOC)
OBJECT_DIR=./

CC=gcc
CXX=g++
CFLAGS=-c -Wall -std=gnu++0x
LDFLAGS=-lstdc++
INCLUDE=-I../../src/XBee -I../../src/plog-1.0.0/include -I../../src/opc

SOURCES=towerPatterns.cpp colorUtilities.cpp \
furSwarmPatterns.cpp towerAnimations.cpp \
towerBall.cpp ledDriver.cpp adxl335.cpp accelerometer.cpp \
embeddedInterface.cpp furSwarmMemberLinux.cpp furSwarmMemberLinuxMain.cpp \
opc/opc_client.cpp poofer.cpp

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=furSwarmLinux

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(DFLAGS) $(INCLUDE) $< -o $@

clean:
    #Clear previous...
	rm -v $(OBJECTS)

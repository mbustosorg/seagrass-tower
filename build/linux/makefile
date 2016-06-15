DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_REEDS -DNOT_EMBEDDED

BUILD_NUMBER_FILE=build-number.txt

DFLAGS+=-D__BUILD_TIME=$$(date +'%Y-%m-%d_%H-%M-%S')
DFLAGS+=-D__BUILD_NUMBER=$$(cat $(BUILD_NUMBER_FILE))

PRODUCT_NAME=furSwarmLinux
PROJECT_LOC=../../src/
VPATH=$(PROJECT_LOC)
OBJECT_DIR=./

CC=gcc
CXX=g++
CFLAGS=-c -Wall -std=gnu++0x -g
LDFLAGS=-lstdc++ -lprotobuf
INCLUDE=-I../../src/XBee -I../../src/plog-1.0.0/include -I../../src/opc

SOURCES=towerPatterns.cpp colorUtilities.cpp \
furSwarmPatterns.cpp towerAnimations.cpp \
towerBall.cpp ledDriver.cpp adxl335.cpp accelerometer.cpp \
embeddedInterface.cpp furSwarmMemberLinux.cpp furSwarmMemberLinuxMain.cpp \
opc/opc_client.cpp poofer.cpp

CC_SOURCES=protobuf/FabricProtos.pb.cc

OBJECTS=$(SOURCES:.cpp=.o)

CC_OBJECTS=$(CC_SOURCES:.cc=.o)

EXECUTABLE=furSwarmLinux

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(CC_OBJECTS)
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(CC_OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(DFLAGS) $(INCLUDE) $< -o $@

.cc.o:
	$(CXX) $(CFLAGS) $(DFLAGS) $(INCLUDE) $< -o $@

clean:
    #Clear previous...
	rm -v $(OBJECTS)

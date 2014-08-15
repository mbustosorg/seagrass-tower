#DEBUG ?= 1
#ifeq ($(DEBUG),1)
#	DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWER -DADXL345 -DFFT_DIAGNOSTICS -DSERIAL_DIAGNOSTICS
#	DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWER -DADXL345 -DFFT_DIAGNOSTICS
#else
	DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWER -DADXL345 -DSERIAL_DIAGNOSTICS
#	DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWER -DADXL345 -DSERIAL_DIAGNOSTICS -DGPS_DIAGNOSTICS
#	DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWER -DADXL345
#endif

PRODUCT_NAME=tower
PROJECT_LOC=../../src/
PROJECT_LIB_LOC=../../../teensy/lib/3.1/
VPATH=$(PROJECT_LOC)
OBJECT_DIR=./

include $(PROJECT_LIB_LOC)../../common/arm_common_31.txt

APP_CPP_SOURCES=TCL.cpp towerPatterns.cpp colorUtilities.cpp \
furSwarmControlMachine.cpp furSwarmMember.cpp furSwarmPatterns.cpp \
xBeeConfiguration.cpp gps2rtc.cpp towerAnimations.cpp \
towerBall.cpp accelerometer.cpp adxl345.cpp adxl335.cpp statusLED.cpp \
Adafruit_NeoPixel.cpp

APP_OBJECTS=$(addprefix $(OBJECT_DIR),$(APP_CPP_SOURCES:.cpp=.o))
LIB_OBJECTS=$(addprefix $(PROJECT_LIB_LOC),*.o)

default: build 

build: furSwarmMember_copy $(OBJECT_DIR)$(PRODUCT_NAME).hex
	@echo
	@echo Listing:
	@echo
	ls -l $(PRODUCT_NAME).hex 
	@echo
	@echo Size:
	@echo
	$(BIN)/arm-none-eabi-size $(PRODUCT_NAME).hex

$(OBJECT_DIR)$(PRODUCT_NAME).hex: $(OBJECT_DIR)$(PRODUCT_NAME).elf
	$(OBJCPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $< $(OBJECT_DIR)$(PRODUCT_NAME).eep
	$(OBJCPY) -O ihex -R .eeprom $< $@

$(OBJECT_DIR)$(PRODUCT_NAME).elf: $(APP_OBJECTS) $(LIB_OBJECTS)
	$(CC) -Os -Wl,--gc-sections $(MCU) -mthumb -T$(CORES)mk20dx256.ld -o $@ $^ -L. -larm_cortexM4l_math -lm

furSwarmMember_copy: $(PROJECT_LOC)/furSwarmMember.cpp

$(PROJECT_LOC)/furSwarmMember.cpp: furSwarmMember.ino
	@echo Copy ino...
	cp $(PROJECT_LOC)furSwarmMember.ino $(PROJECT_LOC)furSwarmMember.cpp

$(OBJECT_DIR)%.o : %.c
	$(CC) $< $(CFLAGS) $(DFLAGS) $(INCLUDE) -c -o $@

$(OBJECT_DIR)%.o : %.cpp
	$(CXX) $< $(CXXFLAGS) $(DFLAGS) $(INCLUDE) -c -o $@

clean:
    #Clear previous...
	rm -v $(APP_OBJECTS)
	rm -v *.d
	rm -v *.eep
	rm -v *.elf
	rm -v *.hex

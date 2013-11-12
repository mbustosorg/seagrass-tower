PRODUCT_NAME=tower
PROJECT_LOC=/Users/mauricio/Documents/development/furSwarmMember/src/
VPATH=$(PROJECT_LOC)
OBJECT_DIR=./
#DFLAGS=-DTEENSY -DUSE_WS2801 -DFS_TOWER 
DFLAGS=-DTEENSY -DUSE_TCL -DTCL_DIO -DFS_TOWER 

include /Users/mauricio/Documents/development/furSwarmMember/build/common/arm_common_dev.txt

APP_CPP_SOURCES=main.cpp TCL.cpp towerPatterns.cpp colorUtilities.cpp furSwarmControlMachine.cpp \
furSwarmMember.cpp furSwarmPatterns.cpp xBeeConfiguration.cpp oledControl.cpp gps2rtc.cpp towerAnimations.cpp towerBall.cpp flameFrames.cpp
APP_OBJECTS=$(addprefix $(OBJECT_DIR),$(APP_CPP_SOURCES:.cpp=.o))

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

$(OBJECT_DIR)$(PRODUCT_NAME).elf: $(APP_OBJECTS)
	$(CC) -Os -Wl,--gc-sections $(MCU) -mthumb -T$(CORES)/mk20dx128.ld -o $@ $^ $(PROJECT_LOC)/../build/lib/$(BOARD)/$(LIBNAME) -L. -larm_cortexM4l_math -lm

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



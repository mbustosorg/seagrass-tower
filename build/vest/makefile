PRODUCT_NAME=vest
PROJECT_LOC=/Users/mauricio/Documents/development/furSwarmMember/src/
VPATH=$(PROJECT_LOC)
OBJECT_DIR=./
DFLAGS=-DUSE_TCL -DFS_VEST -DTCL_SPI

include /Users/mauricio/Documents/development/furSwarmMember/build/common/avr_common.txt

APP_CPP_SOURCES=main.cpp colorUtilities.cpp furSwarmControlMachine.cpp \
furSwarmMember.cpp furSwarmPatterns.cpp xBeeConfiguration.cpp TCL.cpp ledDriver.cpp
APP_OBJECTS=$(addprefix $(OBJECT_DIR),$(APP_CPP_SOURCES:.cpp=.o))

default: build 

upload: build
	$(UP) -C$(UPCONF) -p$(CHIP) -carduino -P$(BOARD_PORT) -b57600 -D -Uflash:w:$(PRODUCT_NAME).hex:i

build: furSwarmMember_copy $(OBJECT_DIR)$(PRODUCT_NAME).hex
	@echo
	@echo Listing:
	@echo
	ls -l $(PRODUCT_NAME).hex 
	@echo
	@echo Size:
	@echo
	$(BIN)/avr-size $(PRODUCT_NAME).hex

$(OBJECT_DIR)$(PRODUCT_NAME).hex: $(OBJECT_DIR)$(PRODUCT_NAME).elf
	$(OBJCPY) -O ihex $< $@

$(OBJECT_DIR)$(PRODUCT_NAME).elf: $(APP_OBJECTS)
	$(CC) -Os -Wl,--gc-sections $(MCU) -o $@ $^ $(PROJECT_LOC)/../build/lib/$(BOARD)/$(LIBNAME) -L. -lm

furSwarmMember_copy: $(PROJECT_LOC)/furSwarmMember.cpp

$(PROJECT_LOC)/furSwarmMember.cpp: furSwarmMember.ino
	@echo Copy ino...
	cp $(PROJECT_LOC)furSwarmMember.ino $(PROJECT_LOC)furSwarmMember.cpp

$(OBJECT_DIR)%.o : %.c
	$(CC) $< $(CFLAGS) $(DFLAGS) $(INCLUDE) -c -o $@

$(OBJECT_DIR)%.o : %.cpp
	$(CXX) $< $(CFLAGS) $(DFLAGS) $(INCLUDE) -c -o $@

clean:
    #Clear previous...
	rm -v $(APP_OBJECTS)
	rm -v *.d
	rm -v *.elf
	rm -v *.hex



include /Users/mauricio/Documents/development/furSwarmMember/build/common/avr_common.txt

LIB_CPP_SOURCES=SPI.cpp EEPROM.cpp Adafruit_WS2801.cpp XBee.cpp LPD8806.cpp
CORE_C_SOURCES=WInterrupts.c wiring.c wiring_analog.c wiring_digital.c wiring_pulse.c wiring_shift.c 
CORE_CPP_SOURCES=CDC.cpp HardwareSerial.cpp HID.cpp new.cpp Tone.cpp USBCore.cpp \
IPAddress.cpp Print.cpp Stream.cpp WMath.cpp WString.cpp
OBJECTS=$(CORE_C_SOURCES:.c=.o) $(CORE_CPP_SOURCES:.cpp=.o)
COBJECTS=$(addprefix $(OBJECT_DIR),$(OBJECTS)) $(LIB_CPP_SOURCES:.cpp=.o)
VPATH = $(LIBRARIES)SPI $(LIBRARIES)EEPROM $(LIBRARIES)adafruitLED $(LIBRARIES)XBee $(LIBRARIES)adafruitStrip

default: $(COBJECTS)
	$(AR) rcs $(LIBNAME) $^

$(OBJECT_DIR)%.o : $(CORES)%.c
	$(CC) $< $(CFLAGS) $(INCLUDE) -c -o $@

$(OBJECT_DIR)%.o : $(CORES)%.cpp 
	$(CXX) $< $(CFLAGS) $(INCLUDE) -c -o $@

$(OBJECT_DIR)%.o : %.cpp
	$(CXX) $< $(CFLAGS) $(INCLUDE) -c -o $@

clean:
	@echo -n Clear previous...
	rm -v *.d
	rm -v *.o
	rm -v *.a


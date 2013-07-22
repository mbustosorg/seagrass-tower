
TRGTS = tower vest towerVest hat
LIBS = arduinoLib teensyLib

default: all

all: $(LIBS) $(TRGTS)

hex: $(TRGTS)

lib: $(LIBS)

arduinoLib:
	cd ./build/lib/arduino ; make

teensyLib:
	cd ./build/lib/teensy ; make

tower:
	cd ./build/tower ; make

vest:
	cd ./build/vest ; make

hat:
	cd ./build/hat ; make

towerVest:
	cd ./build/towerVest ; make

clean:
	cd ./build/tower ; make clean
	cd ./build/vest ; make clean
	cd ./build/hat ; make clean
	cd ./build/towerVest ; make clean
	cd ./build/lib/arduino ; make clean
	cd ./build/lib/teensy ; make clean


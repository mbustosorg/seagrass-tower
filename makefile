
TRGTS = vest hat tower towerVest towerHat towerEye townCenter
LIBS = arduinoLib teensyLib

default: all

all: $(LIBS) $(TRGTS)

hex: $(TRGTS)

lib: $(LIBS)

arduinoLib:
	cd ./build/lib/arduino ; make

teensyLib:
	cd ./build/lib/teensy ; make

vest:
	cd ./build/vest ; make

hat:
	cd ./build/hat ; make

tower:
	cd ./build/tower ; make

towerVest:
	cd ./build/towerVest ; make

towerHat:
	cd ./build/towerHat ; make

towerEye:
	cd ./build/towerEye ; make

townCenter:
	cd ./build/townCenter ; make

clean:
	cd ./build/tower ; make clean
	cd ./build/vest ; make clean
	cd ./build/hat ; make clean
	cd ./build/towerVest ; make clean
	cd ./build/towerHat ; make clean
	cd ./build/towerEye ; make clean
	cd ./build/townCenter ; make clean
	cd ./build/lib/arduino ; make clean
	cd ./build/lib/teensy ; make clean


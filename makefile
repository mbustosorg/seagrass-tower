TRGTS = buzzInn rhbSignal rotoFuego tower towerDress towerEye towerHat towerTiki towerVest townCenter
LIBS = teensyLib

default: all

all: $(LIBS) $(TRGTS)

hex: $(TRGTS)

lib: $(LIBS)

teensyLib:
	cd ./build/lib/teensy ; make clean ; make

buzzInn:
	cd ./build/buzzInn ; make clean ; make

rhbSignal:
	cd ./build/rhbSignal ; make clean ; make

rotoFuego:
	cd ./build/rotoFuego ; make clean ; make

tower:
	cd ./build/tower ; make clean ; make

towerDress:
	cd ./build/towerDress ; make clean ; make

towerEye:
	cd ./build/towerEye ; make clean ; make

towerHat:
	cd ./build/towerHat ; make clean ; make

towerTiki:
	cd ./build/towerTiki ; make clean ; make

towerVest:
	cd ./build/towerVest ; make clean ; make

townCenter:
	cd ./build/townCenter ; make clean ; make



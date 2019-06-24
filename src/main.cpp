#include "WProgram.h"

int main(void)
{

#if defined(USBCON)
	USBDevice.attach();
#endif

	setup();
    
	for (;;) {
		loop();
	}
        
	return 0;
}

#ifdef USB_SERIAL
usb_serial_class Serial;
#endif

/*
	Include C++ headers here.

	If you want to use C++ libraries, they must be included in this CPP OR explicitely wrapped as extern "C++" {} so the compiler knows what conventions to use
	and how to link them.
*/

/*
	Tell the compiler to wrap main as C convention since most of the IDF framework in standard C.
*/

extern "C" {
	// Include C-headers here.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <configGpio.h>
	#include <configLedc.h>
	#include <configTimers.h>

    void app_main(void);
}

void app_main(void) {
	// Call all ESP-IDF initializers first.
	vInitGPIO();
	vInitTimers();
	vInitLEDC();


	// Call all FreeRTOS initializers after.
	vInitTaskLEDFade();
	// vInitTaskRTOSDebug();	// use for debugging only. warning: slow and takes long time to process
	
}


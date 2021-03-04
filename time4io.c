#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw() {
	return (PORTD >> 8) & 0x000F;
}


int getbtns() {
	return (PORTD >> 5) & 0x0007;
}
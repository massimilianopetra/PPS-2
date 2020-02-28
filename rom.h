#ifndef __ROM_H
#define __ROM_H

#include <stdint.h>

extern uint8_t _CHARROM[2048];

class ROM
{
	private:
		uint8_t *MEMORY;
		
	public:
		ROM(uint8_t *_MEMORY);
		int loadROM(uint16_t startAddress, uint16_t len, char *filename);
		int loadROM(uint16_t startAddress, char *filename);
		int loadCHARROM(char* filename);
};

#endif

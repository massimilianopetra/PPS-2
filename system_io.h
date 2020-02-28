#ifndef __SYSTEM_IO_H
#define __SYSTEM_IO_H

#include <stdint.h>
#include "disk.h"

// **************** MEMORY **********************
// Keyboard Data
#define _KBD			0xC000
// Clear Keyboard Strobe     
#define _KBDCR			0xC010
// Sound
#define _SOUND  		0xC030
// Paddle 
#define _PDL_TRIGGER 	0xC070
#define _PDL0			0xC064
#define _PDL1			0xC065
#define _PDL2			0xC066
#define _PDL3			0xC067
// ***********************************************

#define _PDL_MIN_uS		2
#define _PDL_MAX_uS		3300


class system_io
{
	private:
		uint8_t * _RAM = NULL;
		
		uint8_t shift = 0;
		uint8_t altgr = 0;
		uint8_t ctrl = 0;
		
		uint32_t pdl0 = 0;
		uint32_t pdl1 = 0;
		uint32_t pdl2 = 0;
		uint32_t pdl3 = 0;
		uint32_t pdl0_timer = 0;
		uint32_t pdl1_timer = 0;
		uint32_t pdl2_timer = 0;
		uint32_t pdl3_timer = 0;
		
		disk *dsk = NULL;
		
	public:
		system_io(uint8_t *RAM);
		int keyboard();
		void keyboard_init();
		void sound();
		void paddle_init();
		void paddle_trigger();
		void paddle_timer(uint64_t cycles);
		uint8_t readPDL0();
		uint8_t readPDL1();
		uint8_t readPDL2();
		uint8_t readPDL3();
		int devicectrl(uint16_t address, uint8_t rw,uint8_t data);
		void Reset();
		void diskslot(uint8_t i);
		int diskmount(char *filename, int drvnum);
		void disksavenib(char *filename, int drvnum);
};

extern system_io *IO;

#endif

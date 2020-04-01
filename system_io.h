#ifndef __SYSTEM_IO_H
#define __SYSTEM_IO_H

#include <stdint.h>
#include "memory.h"
#include "disk.h"

// **************** MEMORY **********************
// Keyboard Data
#define _KBD			0xC000
// Clear Keyboard Strobe     
#define _KBDCR			0xC010
// Sound
#define _SOUND  		0xC030
// Timer 
#define _TIMER_TRIGGER 	0xC070
// Serial I/O
#define _CASSETTE_IN	0xC060
#define _PB0			0xC061
#define _PB1			0xC062
#define _PB2			0xC063
#define _PDL0			0xC064
#define _PDL1			0xC065
#define _PDL2			0xC066
#define _PDL3			0xC067
// ***********************************************

#define _PDL_MIN_uS		2
#define _PDL_MAX_uS		3300
#define _PDL_INC		30


class system_io
{
	private:
		uint8_t * _RAM;
		
		uint8_t showkbdcode;
		uint8_t shift;
		uint8_t altgr;
		uint8_t ctrl;
		
		uint8_t pb0;
		uint8_t pb1;
		uint8_t pb2;
		
		int32_t pdl0;
		int32_t pdl1;
		int32_t pdl2;
		int32_t pdl3;
		int32_t pdl0_inc;
		int32_t pdl1_inc;
		int32_t pdl2_inc;
		int32_t pdl3_inc;
		int32_t pdl0_timer;
		int32_t pdl1_timer;
		int32_t pdl2_timer;
		int32_t pdl3_timer;
		
		disk *dsk;
		
	public:
		system_io();
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
		uint8_t readPB0();
		uint8_t readPB1();
		uint8_t readPB2();
		void resetPB0();
		void resetPB1();
		void resetPB2();
		void setPB0();
		void setPB1();
		void setPB2();		
		int devicectrl(uint16_t address, uint8_t rw,uint8_t data);
		void Reset();
		void diskslot(uint8_t i);
		int diskmount(char *filename, int drvnum);
		void disksavenib(char *filename, int drvnum);
		void disksavenib();
		void disksavedsk();
		void diskfetch(uint8_t cycles);
		void diskprint();
		void diskdebug(uint8_t *_activedrv,uint8_t *_pwr1, int8_t *_halftrk1, uint16_t *_ptr1,uint8_t *_pwr2, int8_t *_halftrk2, uint16_t *_ptr2);
};

extern system_io *IO;

#endif

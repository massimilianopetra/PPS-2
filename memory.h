#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include "system_io.h"

/*
 * memory.h 
 *
 * Author: Massimiliano Petra <massimiliano.petra@gmail.com> February, 2020
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */
   
#define _READ  0
#define _WRITE 1

// ROM Switches
#define INTCXROM_OFF	0xC006
#define INTCXROM_ON		0xC007
#define INTCXROM_READ	0xC015

#define SLOTC3ROM_OFF	0xC00A
#define SLOTC3ROM_ON	0xC00B
#define SLOTC3ROM_READ	0xC017

// Sound
#define _SOUND  		0xC030

// BUS Acces Prototype

uint8_t busreader(uint16_t address);
void buswriter(uint16_t address,uint8_t value);

extern uint8_t _CHARROM[2048];

class memory
{
	private:
		// Memory spaces
		uint8_t _RAM[0x10000];
		uint8_t _RAM_BANK1[0x1000];
		uint8_t _ROM[0x10000];
		uint8_t _ROM_SLOT[0x1000];
		
		// Soft switches
		uint8_t INTCXROM;
		uint8_t INTC8ROM;
		uint8_t SLOTC3ROM;
		uint8_t BANK1;
		uint8_t HRAMRD;
		uint8_t HRAMWRT;
		uint8_t PREWRITE;
		
	public:
		memory();
		
		uint8_t read(uint16_t address);
		void write(uint16_t address,uint8_t value);
		
		void Reset();
		
		uint8_t* getRAM();
		uint8_t* getROM();
		
		uint8_t readRAM(uint16_t address);
		uint8_t readROM(uint16_t address);
		
		void writeRAM(uint16_t address,uint8_t value);
		void writeROM(uint16_t address,uint8_t value);		
		void writeROMSLOT(uint8_t address,uint8_t slot, uint8_t value);
		
		int loadROM(uint16_t startAddress, uint16_t len, char *filename);
		int loadROM(uint16_t startAddress, char *filename);
		int loadRAM(uint16_t startAddress, char *filename);
		int saveRAM(uint16_t startAddress, uint16_t len, char *filename);
		int loadCHARROM(char* filename);
		
		uint8_t getBANK1();
		uint8_t getHRAMRD();
		uint8_t getHRAMWRT();
};

extern memory *mem;

#endif


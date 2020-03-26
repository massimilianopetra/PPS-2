#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include "system_io.h"
#include "rom.h"

#define _READ  0
#define _WRITE 1

#define INTCXROM_OFF	0xC006
#define INTCXROM_ON		0xC007
#define INTCXROM_READ	0xC015



#define SLOTC3ROM_OFF	0xC00A
#define SLOTC3ROM_OFF	0xC00B
#define SLOTC3ROM_READ	0xC017


// BUS Acces Prototype

uint8_t busreader(uint16_t address);
void buswriter(uint16_t address,uint8_t value);

class memory
{
	private:
		uint8_t _RAM[0x10000];
		uint8_t _RAM_BANK1[0x1000];
		uint8_t _ROM[0x10000];
		
		uint8_t INTCXROM;
		uint8_t SLOTC3ROM;
		uint8_t BANK1;
		uint8_t HRAMRD;
		uint8_t HRAMWRT;
		uint8_t PREWRITE;
		
		ROM *rom;
		
	public:
		memory();
		uint8_t read(uint16_t address);
		void write(uint16_t address,uint8_t value);
		void Reset();
		uint8_t* getRAM();
		uint8_t* getROM();
		int loadROM(uint16_t startAddress, uint16_t len, char *filename);
		int loadROM(uint16_t startAddress, char *filename);
		int loadCHARROM(char* filename);
		uint8_t getBANK1();
		uint8_t getHRAMRD();
		uint8_t getHRAMWRT();
};

extern memory *mem;

#endif


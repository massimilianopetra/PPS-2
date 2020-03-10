#include <stdio.h>
#include "memory.h"
#include "video.h"



/* ********************** Memory BUS Functions *************** */  

uint8_t busreader(uint16_t address)
{
	return mem->read(address);
}

void buswriter(uint16_t address,uint8_t value)
{
	mem->write(address,value);
	
}

/****************************************************************/

memory *mem = NULL;

memory::memory()
{
	rom = new ROM(_ROM);
	Reset();
}

void memory::Reset()
{
	INTCXROM = 0;
    SLOTC3ROM = 0;
    BANK1 = 0;
    HRAMRD = 0;
    HRAMRD = 0;
    PREWRITE = 0;
}

uint8_t* memory::getRAM()
{
	return _RAM;
}

uint8_t memory::getBANK1()
{
	return BANK1;
}

uint8_t memory::getHRAMRD()
{
	return HRAMRD;
}

uint8_t memory::getHRAMWRT()
{
	return HRAMWRT;
}

int memory::loadROM(uint16_t startAddress, uint16_t len, char *filename)
{
	return rom->loadROM(startAddress,len,filename);
}

int memory::loadROM(uint16_t startAddress, char *filename)
{
	return rom->loadROM(startAddress,filename);	
}

int memory::loadCHARROM(char* filename)
{
	return rom->loadCHARROM(filename);
}

uint8_t memory::read(uint16_t address)
{
	uint8_t value = 0;
	
	// BANK1 Switch
	if (address >=0xC080 &&  address <=0xC087) 
	{ 
		BANK1 = 0;
	}
	if (address >=0xC088 &&  address <=0xC08F) 
	{ 
		BANK1 = 1;
	}
	
	// HRAMRD Switch
	switch(address)
	{
		case 0xC080:
		case 0xC083:
		case 0xC084:
		case 0xC087:
		case 0xC088:
		case 0xC08B:
		case 0xC08C:
		case 0xC08F:
			HRAMRD = 1;
			break;
		
		case 0xC081:
		case 0xC082:
		case 0xC085:
		case 0xC086:
		case 0xC089:
		case 0xC08A:
		case 0xC08D:
		case 0xC08E:
			HRAMRD = 0;
			break;					
	}
	
	if (address == _KBD) 
	{ 
		value = _RAM[_KBD];
	}
	else if (address == _KBDCR)
	{
		_RAM[_KBD] = _RAM[_KBD] & 0x7f;
	}
	else if (address == _SOUND)
	{
		IO->sound();
	}
	else if ((address & 0xFFF0) == _PDL_TRIGGER)
	{
		IO->paddle_trigger();
	}
	else if (address == _PDL0)
	{
		value = IO->readPDL0();
	}
	else if (address == _PDL1)
	{
		value = IO->readPDL1();
	}
	else if (address == _PDL2)
	{
		value = IO->readPDL3();
	}
	else if (address == _PDL3)
	{
		value = IO->readPDL3();
	}
	else if (address == INTCXROM_READ)
	{
		value = INTCXROM;
	}
	else if (address >= 0xC090 && address <= 0xC0FF) 
	{
		// Handle devices
		value = IO->devicectrl(address,_READ,0x00);
	}
	else if (address == _GRPHICS_MODE)
	{
		screen_switches |= _TEXT_GRAPHICS;
	}
	else if (address == _TEXT_MODE)
	{
		screen_switches &= ~ _TEXT_GRAPHICS;
	}
	else if (address == _MIX_MODE)
	{
		screen_switches |= _ALL_MIX;
	}
	else if (address == _ALL_MODE)
	{
		screen_switches &= ~ _ALL_MIX;
	}
	else if (address == _PAGE2_MODE)
	{
		screen_switches |= _PAGE1_PAGE2;
	}
	else if (address == _PAGE1_MODE)
	{
		screen_switches &= ~ _PAGE1_PAGE2;
	}
	else if (address == _HIRES_MODE)
	{
		screen_switches |= _LORES_HIRES;
	}
	else if (address == _LORES_MODE)
	{
		screen_switches &= ~ _LORES_HIRES;
	}
	else if (address > 0xCFFF)
	{
		// Read ROM
		if (HRAMRD)
		{
			if (BANK1 == 1 && address < 0xE000)
			{
				value = _RAM_BANK1[address-0xD000];
			}
			else
			{
				value = _RAM[address];
			}
		}
		else
			value = _ROM[address];
	}
  	else
  	{
  		// read RAM
  		value = _RAM[address];	
	}
	
	return value;
}

void memory::write(uint16_t address,uint8_t value)
{

	// BANK1 Switch
	if (address >=0xC080 &&  address <=0xC087) 
	{ 
		BANK1 = 0;
	}
	if (address >=0xC088 &&  address <=0xC08F) 
	{ 
		BANK1 = 1;
	}
	
	// HRAMRD Switch
	switch(address)
	{
		case 0xC080:
		case 0xC083:
		case 0xC084:
		case 0xC087:
		case 0xC088:
		case 0xC08B:
		case 0xC08C:
		case 0xC08F:
			HRAMRD = 1;
			break;
		
		case 0xC081:
		case 0xC082:
		case 0xC085:
		case 0xC086:
		case 0xC089:
		case 0xC08A:
		case 0xC08D:
		case 0xC08E:
			HRAMRD = 0;
			break;					
	}
	
	if (address == _KBDCR)
	{
		_RAM[_KBD] = _RAM[_KBD] & 0x7f;
	}
	else if (address == _SOUND)
	{
		IO->sound();	
	}
	else if ((address & 0xFFF0) == _PDL_TRIGGER)
	{
		IO->paddle_trigger();
	}
	else if (address == INTCXROM_OFF)
	{
		INTCXROM = 0x00;
	}
	else if (address == INTCXROM_ON)
	{
		INTCXROM = 0x80;
	}
	else if (address >= 0xC080 && address <= 0xC0FF) 
	{
		// Handle devices
		IO->devicectrl(address,_WRITE,value);
	}
	else if (address == _GRPHICS_MODE)
	{
		screen_switches |= _TEXT_GRAPHICS;
	}
	else if (address == _TEXT_MODE)
	{
		screen_switches &= ~ _TEXT_GRAPHICS;
	}
	else if (address == _MIX_MODE)
	{
		screen_switches |= _ALL_MIX;
	}
	else if (address == _ALL_MODE)
	{
		screen_switches &= ~ _ALL_MIX;
	}
	else if (address == _PAGE2_MODE)
	{
		screen_switches |= _PAGE1_PAGE2;
	}
	else if (address == _PAGE1_MODE)
	{
		screen_switches &= ~ _PAGE1_PAGE2;
	}
	else if (address == _HIRES_MODE)
	{
		screen_switches |= _LORES_HIRES;
	}
	else if (address == _LORES_MODE)
	{
		screen_switches &= ~ _LORES_HIRES;
	}
	else
	{
		// Write value in RAM/HRAM  (DON'T WRITE ON PERIPHERAL ROM)
		if (address < 0xC100 || address > 0xCFFF)
		{
			if (address >= 0xD000 && HRAMWRT == 0)
			{
				if (BANK1 == 1 && address < 0xE000)
				{
					_RAM_BANK1[address-0xD000] = value;
				}
				else
				{
					_RAM[address] = value;
				}
			}
			else
				_RAM[address] = value;
		}
	}
}


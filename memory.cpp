/*
 * memory.cpp 
 *
 * Copyright (c) Massimiliano Petra <massimiliano.petra@gmail.com> February, 2020
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

uint8_t _CHARROM[2048];

memory *mem = NULL;

memory::memory()
{
	memset(_RAM,0x00,0x10000);
	memset(_ROM,0x00,0x10000);
	memset(_ROM_SLOT,0x00,0x1000);
	memset(_RAM_BANK1,0x00,0x1000);
	Reset();
}

void memory::Reset()
{
	INTCXROM = 0;
	INTC8ROM = 0;
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

uint8_t* memory::getROM()
{
	return _ROM;
}

uint8_t memory::readRAM(uint16_t address)
{
	uint8_t value = 0x00;
	
	if (address < 0x10000)
		value = _RAM[address];
		
	return value;
}

uint8_t memory::readROM(uint16_t address)
{
	uint8_t value = 0x00;
	
	if (address < 0x10000)
		value = _ROM[address];
		
	return value;	
}

void memory::writeRAM(uint16_t address,uint8_t value)
{
	if (address < 0x10000)
		_RAM[address]=value;
}

void memory::writeROM(uint16_t address,uint8_t value)
{
	if (address < 0x10000)
		_ROM[address]=value;	
}		

void memory::writeROMSLOT(uint8_t address,uint8_t slot, uint8_t value)
{
	uint16_t laddr;
	
	laddr = 0x0100*slot+address;
	_ROM_SLOT[laddr]=value;	
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
	FILE *fp=NULL;
	int i=0;
	uint8_t b;
	
	fp=fopen(filename,"rb");
	if (fp == NULL)
		return -1;

	while((fread(&b,1,1,fp) == 1) && (i<len)) 
	{
		if (startAddress+i >= 0xc100)
			writeROM(startAddress+i,b);
		i++;
	}

	if (i != len)
		return -2;
		
	fclose(fp);
	return i;	
}

int memory::loadROM(uint16_t startAddress, char *filename)
{
	FILE *fp=NULL;
	int i=0;
	uint8_t b;
	
	fp=fopen(filename,"rb");
	if (fp == NULL)
		return -1;
	
	while(fread(&b,1,1,fp) == 1) 
	{
		if (startAddress+i >= 0xc100)
			writeROM(startAddress+i,b);
		i++;
	}
			
	fclose(fp);
	return i;	
}

int memory::loadCHARROM(char* filename)
{
	FILE *fp=NULL;
	int i;
	
	fp=fopen(filename,"rb");
	if (fp == NULL)
		return -1;
	i = fread(_CHARROM,1,2048,fp); 
	if (i != 2048)
		return -2;
	fclose(fp);
	return 0;
}

uint8_t memory::read(uint16_t address)
{
	uint8_t value = 0;
	
	/********** BANK1 SOFT SWITCHES **********/
	if (address >=0xC080 &&  address <=0xC087) 
	{ 
		BANK1 = 0;
	}
	if (address >=0xC088 &&  address <=0xC08F) 
	{ 
		BANK1 = 1;
	}
	
	/********** HRAMRD SOFT HANDLER **********/
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
	
	/********** I/O SOFT SWITCHES **********/
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
	else if ((address & 0xFFF0) == _TIMER_TRIGGER)
	{
		IO->paddle_trigger();
	}
	else if (address == _PDL0 || address == _PDL0+8) 
	{
		value = IO->readPDL0();
	}
	else if (address == _PDL1 || address == _PDL1+8)
	{
		value = IO->readPDL1();
	}
	else if (address == _PDL2 || address == _PDL2+8)
	{
		value = IO->readPDL3();
	}
	else if (address == _PDL3 || address == _PDL3+8)
	{
		value = IO->readPDL3();
	}
	else if (address == _PB0 || address == _PB0+8)
	{
		value = IO->readPB0();
	}
	else if (address == _PB1 || address == _PB1+8)
	{
		value = IO->readPB1();
	}
	else if (address == _PB2 || address == _PB2+8)
	{
		value = IO->readPB2();
	}	
	else if (address == INTCXROM_READ)
	{
		value = INTCXROM;
	}
	else if (address == SLOTC3ROM_READ)
	{
		value = SLOTC3ROM;
	}
	
	/********** DEVICE SLOT SOFT SWITCHES **********/
	else if (address >= 0xC090 && address <= 0xC0FF) 
	{
		// Handle devices
		value = IO->devicectrl(address,_READ,0x00);
	}
	
	/********** VIDEO SOFT SWITCHES **********/
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
	
	/********** ROM HANDLER **********/
	else if (address >= 0xC100 && address < 0xD000)
	{
		if (address <= 0xC2FF)
		{
			// Slot 1 - 2
			if(INTCXROM)
				value = _ROM[address];
			else
				value = _ROM_SLOT[address-0xC000];
		}
		else if (address <= 0xC3FF)
		{
			// Slot 3
			if(INTCXROM == 0 && SLOTC3ROM == 0x80)
				value = _ROM_SLOT[address-0xC000];
			else
				value = _ROM[address];
		}
		else if (address <= 0xC7FF)
		{
			// Slot 4 - 5 - 6 - 7
			if(INTCXROM)
				value = _ROM[address];
			else
				value = _ROM_SLOT[address-0xC000];
		}
		else
		{	// Read motherboard ROM
			value = _ROM[address];
		}
		
	}
	else if (address >= 0xD000)
	{
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
	
	/********** READ RAM **********/
  	else
  	{
  		value = _RAM[address];	
	}
	
	return value;
}

void memory::write(uint16_t address,uint8_t value)
{

	/********** BANK1 SOFT SWITCHES **********/
	if (address >=0xC080 &&  address <=0xC087) 
	{ 
		BANK1 = 0;
	}
	if (address >=0xC088 &&  address <=0xC08F) 
	{ 
		BANK1 = 1;
	}
	
	/********** HRAMRD SOFT HANDLER **********/
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
	
	/********** I/O SOFT SWITCHES **********/
	if (address == _KBDCR)
	{
		_RAM[_KBD] = _RAM[_KBD] & 0x7f;
	}
	else if (address == _SOUND)
	{
		IO->sound();	
	}
	else if ((address & 0xFFF0) == _TIMER_TRIGGER)
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
	else if (address == SLOTC3ROM_OFF)
	{
		SLOTC3ROM = 0x00;
	}
	else if (address == SLOTC3ROM_ON)
	{
		SLOTC3ROM = 0x80;
	}
	
	/********** DEVICE SLOT SOFT SWITCHES **********/
	else if (address >= 0xC080 && address <= 0xC0FF) 
	{
		// Handle devices
		IO->devicectrl(address,_WRITE,value);
	}
	
	/********** VIDEO SOFT SWITCHES **********/
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
	
	/********** WRITE RAM **********/
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


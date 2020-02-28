#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "nibbilizer.h"

uint8_t _DISK_P5A[256] = {
  0xA2, 0x20, 0xA0, 0x00, 0xA2, 0x03, 0x86, 0x3C, 0x8A, 0x0A, 0x24, 0x3C, 0xF0, 0x10, 0x05, 0x3C, 0x49, 0xFF, 0x29, 0x7E, 0xB0, 0x08, 0x4A, 0xD0, 0xFB, 0x98, 0x9D, 0x56, 0x03, 0xC8, 0xE8, 0x10, 0xE5, 0x20, 0x58, 0xFF, 0xBA, 0xBD, 0x00, 0x01,
  0x0A, 0x0A, 0x0A, 0x0A, 0x85, 0x2B, 0xAA, 0xBD, 0x8E, 0xC0, 0xBD, 0x8C, 0xC0, 0xBD, 0x8A, 0xC0, 0xBD, 0x89, 0xC0, 0xA0, 0x50, 0xBD, 0x80, 0xC0, 0x98, 0x29, 0x03, 0x0A, 0x05, 0x2B, 0xAA, 0xBD, 0x81, 0xC0, 0xA9, 0x56, 0x20, 0xA8, 0xFC, 0x88,
  0x10, 0xEB, 0x85, 0x26, 0x85, 0x3D, 0x85, 0x41, 0xA9, 0x08, 0x85, 0x27, 0x18, 0x08, 0xBD, 0x8C, 0xC0, 0x10, 0xFB, 0x49, 0xD5, 0xD0, 0xF7, 0xBD, 0x8C, 0xC0, 0x10, 0xFB, 0xC9, 0xAA, 0xD0, 0xF3, 0xEA, 0xBD, 0x8C, 0xC0, 0x10, 0xFB, 0xC9, 0x96,
  0xF0, 0x09, 0x28, 0x90, 0xDF, 0x49, 0xAD, 0xF0, 0x25, 0xD0, 0xD9, 0xA0, 0x03, 0x85, 0x40, 0xBD, 0x8C, 0xC0, 0x10, 0xFB, 0x2A, 0x85, 0x3C, 0xBD, 0x8C, 0xC0, 0x10, 0xFB, 0x25, 0x3C, 0x88, 0xD0, 0xEC, 0x28, 0xC5, 0x3D, 0xD0, 0xBE, 0xA5, 0x40,
  0xC5, 0x41, 0xD0, 0xB8, 0xB0, 0xB7, 0xA0, 0x56, 0x84, 0x3C, 0xBC, 0x8C, 0xC0, 0x10, 0xFB, 0x59, 0xD6, 0x02, 0xA4, 0x3C, 0x88, 0x99, 0x00, 0x03, 0xD0, 0xEE, 0x84, 0x3C, 0xBC, 0x8C, 0xC0, 0x10, 0xFB, 0x59, 0xD6, 0x02, 0xA4, 0x3C, 0x91, 0x26,
  0xC8, 0xD0, 0xEF, 0xBC, 0x8C, 0xC0, 0x10, 0xFB, 0x59, 0xD6, 0x02, 0xD0, 0x87, 0xA0, 0x00, 0xA2, 0x56, 0xCA, 0x30, 0xFB, 0xB1, 0x26, 0x5E, 0x00, 0x03, 0x2A, 0x5E, 0x00, 0x03, 0x2A, 0x91, 0x26, 0xC8, 0xD0, 0xEE, 0xE6, 0x27, 0xE6, 0x3D, 0xA5,
  0x3D, 0xCD, 0x00, 0x08, 0xA6, 0x2B, 0x90, 0xDB, 0x4C, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00
};

drive::drive(uint8_t num)
{
	image = (uint8_t*)malloc(sizeof(uint8_t)*DISK_SIZE);
	
	if (image == NULL)
	{
		printf("Can't allocate disk image");
	}
	
	write_protect = 0;
	mounted = 0;	
	track = 0;
	sector = 0;
	phase = 0;
	rw = 0;
	power = 0;
	drvnum = num;
	sequencer = 0;
	timming = 0;
	nib = new nibbilizer();
}

int drive::mount(char * filename)
{
	FILE *fp;
	
	printf("Mounting %s on drive %d ...\n",filename,drvnum);
	
	fp=fopen(filename,"rb");
	if (fp == NULL)
	{
		//printf("Drive %d: can't open FILE: %s\n",drvnum,filename);
		return -1;
	}
		
	if (fread(image,1,DISK_SIZE,fp) != DISK_SIZE)
	{
		//printf("Drive %d: wrong disk format\n",drvnum);
		return -2;		 
	}
		
	fclose(fp);
	
	// Nibbilize disk image
	nib->convert(image);
	
	mounted = 1;
	
	printf("Mount OK\n",drvnum,filename);
	
	return 0;	
}

void drive::savenib(char * filename)
{
	nib->save(filename);	
}

void drive::stepper(uint8_t p)
{
	switch(phase)
	{
		case 0:
				if (p == 1)
					track++;
				else if (p == 3)
					track--;	
			break;
		case 1:
				if (p == 2)
					track++;
				else if (p == 0)
					track--;	
			break;
		case 2:
				if (p == 3)
					track++;
				else if (p == 1)
					track--;	
			break;
		case 3:
				if (p == 0)
					track++;
				else if (p == 2)
					track--;
			break;
	}

	if (track < 0)
		track = 0;
	
	if (track > ((2*NUM_TRACKS)-1))
		track = (2*NUM_TRACKS)-1;
		 
	phase = p;
}

void drive::umount()
{
	mounted = 0;
}

void drive::setpower(uint8_t p)
{
	power = p;
}

uint8_t drive::ismounted()
{
	return mounted;
}

void drive::setmode(int m)
{
	rw = m;
}

int drive::getmode()
{
	return rw;
}

void drive::print()
{
	printf("DRV: %d PWR: %d TRK: %02d SEC: %02d PHA: %d SEQ: %02X",drvnum,power,track,sector,phase,sequencer);
}

void drive::load(uint8_t data)
{
	;	
}

disk::disk(uint8_t *_RAM)
{
	RAM = _RAM;
	slot = 0;
	strcpy(filename,"");
	drv1 = new drive(1);
	drv2 = new drive(2);
	activedrv = drv1;
	data_register = 0;
}

void disk::init(uint8_t _slot)
{
	int i;
	uint16_t start_address;
	slot = _slot;
	start_address = 0xC000+_slot*0x100;
	
	for(i = 0; i < 0xFF; i++)
	{
		RAM[start_address+i]=_DISK_P5A[i];
	}
	
	printf("DISK II installed at %04X \n",start_address);
}

uint8_t disk::controller(uint8_t n, uint8_t on, uint8_t rw, uint8_t data)
{
	uint8_t rw_latch = 0x00;
	
	//printf("<%d - %d>  ",n,on);
	//activedrv->print();
	//printf("\n");
	
	switch(n)
	{
		// PHASE C080,X C081,X C082,X C083,X C084,X C085,X C086,X C087,X 
		case 0:
		case 1:	
		case 2:
		case 3:
			if (on)
				activedrv->stepper(n);	
			break;
			
		// POWER C088,X C089,X
		case 4:
				activedrv->setpower(on);	
			break;
		
		// DRIVE SELECT C08A,X C08B,X
		case 5:
			if (on)
				activedrv = drv2;
			else
				activedrv = drv1;
			break;
		
		// SHIFT / LOAD C08C,X C08D,X
		case 6:
			if (on == 0)
				rw_latch = activedrv->readwrite();
			else
			{
				if (activedrv->getmode())	
				{
					// Load data
					data_register = data;
				}
				else
				{
					// Check write protect
					data_register = 0x00; // TODO Check Write Protect
				}
					
						
			}
			break;
				
		// READ / WRITE C08E,X C08F,X
		case 7:
			activedrv->setmode(on);
			break;
	}
	
	// Reading even address read the latch
	if (rw == 0 && on == 0)
		return data_register;
	else
		return 0;
}

uint8_t drive::readwrite()
{
	return 0x00;
}

uint8_t disk::getslot()
{
 	return slot;
}

int disk::diskmount(char *filename, int drvnum)
{
	int i;
	
	if (drvnum == 1)
		i = drv1->mount(filename);
	else
		i = drv2->mount(filename);
		
	return i;
}

void disk::savenib(char * filename,int drvnum)
{
	if (drvnum == 1)
		drv1->savenib(filename);
	else
		drv2->savenib(filename);
}

void disk::reset()
{
	data_register = 0;
}

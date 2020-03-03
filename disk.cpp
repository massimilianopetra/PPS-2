#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "nibbilizer.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 
  
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
	ptr = 0;
	curbyte = 0;
	phase = 0;
	rw = 0;
	power = 0;
	drvnum = num;
	sequencer = 0;
	nib = new nibbilizer();
	track_data = nib->get_track(0);
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
	if ((track % 2) == 0)
	{
		ptr = 0;
		track_data = nib->get_track(track/2);
		curbyte = track_data[ptr];
		sequencer = 0;
	}
}

void drive::umount()
{
	mounted = 0;
}

void drive::setpower(uint8_t p)
{
	power = p;
}

uint8_t drive::getpower()
{
	return power;
}

uint8_t drive::ismounted()
{
	return mounted;
}

void drive::setmode(int m)
{
	rw = m;
}

uint8_t drive::getmode()
{
	return rw;
}

uint8_t drive::getsequencer()
{
	return sequencer;
}

uint8_t drive::clear()
{
	sequencer = 0;
}

uint8_t drive::fetch()
{
	uint8_t value;

	if (sequencer < 8)
	{
		value = (curbyte & 0x80) >> 7;
		curbyte = curbyte << 1;
		sequencer++;
		if (sequencer == 8)
		{
			ptr++;
			ptr = ptr % BYTES_PER_NIB_TRACK;
			curbyte = track_data[ptr];
		}
	}
	else
	{
		value = 0x80;
	}

	return value;
}

void drive::print()
{
	printf("DRV: %d PWR: %d HALFTRK: %02d TRK: %02d PTR: %04d BYTE: %02X PHA: %d SEQ: %02X \n",drvnum,power,track,track/2,ptr,track_data[ptr],phase,sequencer);
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

void disk::print()
{
	activedrv->print();
	printf("DATA_REGISTER "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data_register)); 
}

void disk::fetch()
{
	uint8_t value;

	if (activedrv->getpower()) 
	{
		value = activedrv->fetch();
		
		if (value != 0x80)
			data_register = (data_register << 1) | value;
	}
}

uint8_t disk::controller(uint8_t n, uint8_t on, uint8_t rw, uint8_t data)
{
	uint8_t value = data_register;
	
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
			if (activedrv->getpower())
			{
				if (on) 
				{
					activedrv->stepper(n);	
					//data_register = 0x00;
					//value = 0x00;
				}
					
			}
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
			if (activedrv->getpower())
			{			
				if (on == 0)
				{
					// SHIFT
					if (activedrv->getsequencer() == 8)
					{
						activedrv->clear();
						data_register = 0x00;
					}
				}
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
						value = 0x00; // TODO Check Write Protect
					}			
				}
			}
			break;
				
		// READ / WRITE C08E,X C08F,X
		case 7:
			activedrv->setmode(on);
			break;
	}
	
	// Reading even address read the latch
	if (rw == 0 && on == 0 && activedrv->getpower())
		return value;
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "nibbilizer.h"
#include "memory.h"
#include "mos6502.h"

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


#define CYCLES_PER_STEP 4

// Disk II Slot ROM  
uint8_t _DISK_P5A[256] = {
  0xA2, 0x20, 0xA0, 0x00,  0xA2, 0x03, 0x86, 0x3C,  0x8A, 0x0A, 0x24, 0x3C,  0xF0, 0x10, 0x05, 0x3C, 
  0x49, 0xFF, 0x29, 0x7E,  0xB0, 0x08, 0x4A, 0xD0,  0xFB, 0x98, 0x9D, 0x56,  0x03, 0xC8, 0xE8, 0x10, 
  0xE5, 0x20, 0x58, 0xFF,  0xBA, 0xBD, 0x00, 0x01,  0x0A, 0x0A, 0x0A, 0x0A,  0x85, 0x2B, 0xAA, 0xBD, 
  0x8E, 0xC0, 0xBD, 0x8C,  0xC0, 0xBD, 0x8A, 0xC0,  0xBD, 0x89, 0xC0, 0xA0,  0x50, 0xBD, 0x80, 0xC0, 
  0x98, 0x29, 0x03, 0x0A,  0x05, 0x2B, 0xAA, 0xBD,  0x81, 0xC0, 0xA9, 0x56,  0x20, 0xA8, 0xFC, 0x88,
  0x10, 0xEB, 0x85, 0x26,  0x85, 0x3D, 0x85, 0x41,  0xA9, 0x08, 0x85, 0x27,  0x18, 0x08, 0xBD, 0x8C, 
  0xC0, 0x10, 0xFB, 0x49,  0xD5, 0xD0, 0xF7, 0xBD,  0x8C, 0xC0, 0x10, 0xFB,  0xC9, 0xAA, 0xD0, 0xF3,
  0xEA, 0xBD, 0x8C, 0xC0,  0x10, 0xFB, 0xC9, 0x96,  0xF0, 0x09, 0x28, 0x90,  0xDF, 0x49, 0xAD, 0xF0, 
  0x25, 0xD0, 0xD9, 0xA0,  0x03, 0x85, 0x40, 0xBD,  0x8C, 0xC0, 0x10, 0xFB,  0x2A, 0x85, 0x3C, 0xBD, 
  0x8C, 0xC0, 0x10, 0xFB,  0x25, 0x3C, 0x88, 0xD0,  0xEC, 0x28, 0xC5, 0x3D,  0xD0, 0xBE, 0xA5, 0x40,
  0xC5, 0x41, 0xD0, 0xB8,  0xB0, 0xB7, 0xA0, 0x56,  0x84, 0x3C, 0xBC, 0x8C,  0xC0, 0x10, 0xFB, 0x59, 
  0xD6, 0x02, 0xA4, 0x3C,  0x88, 0x99, 0x00, 0x03,  0xD0, 0xEE, 0x84, 0x3C,  0xBC, 0x8C, 0xC0, 0x10, 
  0xFB, 0x59, 0xD6, 0x02,  0xA4, 0x3C, 0x91, 0x26,  0xC8, 0xD0, 0xEF, 0xBC,  0x8C, 0xC0, 0x10, 0xFB, 
  0x59, 0xD6, 0x02, 0xD0,  0x87, 0xA0, 0x00, 0xA2,  0x56, 0xCA, 0x30, 0xFB,  0xB1, 0x26, 0x5E, 0x00, 
  0x03, 0x2A, 0x5E, 0x00,  0x03, 0x2A, 0x91, 0x26,  0xC8, 0xD0, 0xEE, 0xE6,  0x27, 0xE6, 0x3D, 0xA5,
  0x3D, 0xCD, 0x00, 0x08,  0xA6, 0x2B, 0x90, 0xDB,  0x4C, 0x01, 0x08, 0x00,  0x00, 0x00, 0x00, 0x00
};

// Disk II Logic State Sequencer ROM
uint8_t _DISK_P6A[256] = {
  0x88, 0xB8, 0x88, 0x08,  0x0A, 0x0A, 0x0A, 0x0A,  0x88, 0xC9, 0x88, 0xC9,  0x88, 0xCB, 0x88, 0xCB, 
  0x88, 0xC8, 0x88, 0x48,  0x0A, 0x0A, 0x0A, 0x0A,  0x88, 0xC9, 0x88, 0xC9,  0x88, 0xCB, 0x88, 0xCB, 
  0xB8, 0x3D, 0xB8, 0xB8,  0x0A, 0x0A, 0x0A, 0x0A,  0x98, 0xD9, 0x98, 0xD9,  0x98, 0xDB, 0x98, 0xDB, 
  0x98, 0xDD, 0x98, 0xD8,  0x0A, 0x0A, 0x0A, 0x0A,  0x98, 0xD9, 0x98, 0xD9,  0x98, 0xDB, 0x98, 0xDB, 
  0xB8, 0xB8, 0xB8, 0xB8,  0x0A, 0x0A, 0x0A, 0x0A,  0xA8, 0xE8, 0xA8, 0xE8,  0xA8, 0xE8, 0xA8, 0xE8,
  0xA8, 0xE8, 0xA8, 0xE8,  0x0A, 0x0A, 0x0A, 0x0A,  0xA8, 0xE8, 0xA8, 0xE8,  0xA8, 0xE8, 0xA8, 0xE8, 
  0xB9, 0xFD, 0xB8, 0xF8,  0x0A, 0x0A, 0x0A, 0x0A,  0xB8, 0xF8, 0xB8, 0xF8,  0xB8, 0xF8, 0xB8, 0xF8, 
  0xB9, 0xFD, 0x50, 0xF8,  0x0A, 0x0A, 0x0A, 0x0A,  0xB8, 0xF8, 0xB8, 0xF8,  0xB8, 0xF8, 0xB8, 0xF8, 
  0x4D, 0xB8, 0xC8, 0x28,  0x0A, 0x0A, 0x0A, 0x0A,  0x48, 0x28, 0x48, 0x28,  0x48, 0x28, 0x48, 0x28, 
  0x4D, 0x28, 0xC8, 0x28,  0x0A, 0x0A, 0x0A, 0x0A,  0x48, 0x28, 0x48, 0x28,  0x48, 0x28, 0x48, 0x28,
  0xB8, 0xB9, 0xB8, 0xB8,  0x0A, 0x0A, 0x0A, 0x0A,  0x58, 0x38, 0x58, 0x38,  0x58, 0x38, 0x58, 0x38, 
  0x49, 0xA9, 0x58, 0x38,  0x0A, 0x0A, 0x0A, 0x0A,  0x58, 0x38, 0x58, 0x38,  0x58, 0x38, 0x58, 0x38, 
  0xB8, 0xB8, 0xB8, 0xB8,  0x0A, 0x0A, 0x0A, 0x0A,  0x68, 0x08, 0x68, 0x18,  0x68, 0x08, 0x68, 0x18, 
  0x68, 0x18, 0x68, 0x18,  0x0A, 0x0A, 0x0A, 0x0A,  0x68, 0x08, 0x68, 0x18,  0x68, 0x08, 0x68, 0x18, 
  0xB8, 0xBD, 0x78, 0x70,  0x0A, 0x0A, 0x0A, 0x0A,  0x78, 0x18, 0x78, 0x08,  0x78, 0x18, 0x78, 0x08,
  0x08, 0x2D, 0x78, 0x70,  0x0A, 0x0A, 0x0A, 0x0A,  0x78, 0x18, 0x78, 0x08,  0x78, 0x18, 0x78, 0x08
};

extern mos6502 *cpu;

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
	drvnum = num;
	sequencer = 0;
	strcpy(filename,"");
	nib = new nibbilizer();
	track_data = nib->get_track(0);
}

int drive::mount(char * _filename)
{
	int res;
	char *last=NULL;
	
	printf("Mounting %s on drive %d ...\n",_filename,drvnum);
	last = strrchr(_filename,'.');
	
	if ((strcmp(last,".dsk") == 0) || (strcmp(last,".do") == 0))
		res = loaddsk(_filename);
	else if (strcmp(last,".nib") == 0)
		res = loadnib(_filename);
		
	if (res == 0)
	{
		mounted = 1;
		
		printf("Mount OK %d %s\n",drvnum,_filename);
		
		// Store filename without extension
		if (last != NULL)
		{
			*last='\0';
			strcpy(filename,_filename);
		}
	}
	else
	{
		mounted = 0;
		printf("Mount ERROR %d %s\n",drvnum,_filename);
	}
	
	return 0;	
}

int drive::loaddsk(char * _filename)
{
	FILE *fp = NULL;
	
	fp=fopen(_filename,"rb");
	if (fp == NULL)
	{
		printf("Drive %d: EMPTY FILE %s\n",drvnum,_filename);
		nib->clear();
		return 0;
	}
		
	if (fread(image,1,DISK_SIZE,fp) != DISK_SIZE)
	{
		printf("Drive %d: wrong disk format\n",drvnum);
		return -1;		 
	}
		
	fclose(fp);
	
	// Nibbilize disk image
	nib->convert(image,0);
	
	return 0;
}

int drive::loadnib(char * _filename)
{
	int i;
	FILE *fp = NULL;
	
	fp=fopen(_filename,"rb");
	if (fp == NULL)
	{
		printf("Drive %d: EMPTY FILE %s\n",drvnum,_filename);
		nib->clear();
		return 0;
	}
	
	for (i=0;i<NUM_TRACKS;i++)
	{
		if (fread(nib->get_track(i),1,BYTES_PER_NIB_TRACK,fp) != BYTES_PER_NIB_TRACK)
		{
			printf("Drive %d: wrong disk format\n",drvnum);
			return -1;		 
		}
	}
		
	fclose(fp);
		
	return 0;
}

void drive::savenib()
{
	char _filename[256];
	sprintf(_filename,"%s.nib",filename);
	savenib(_filename);	
}

void drive::savenib(char * _filename)
{
	nib->save(_filename);	
}

void drive::savedsk()
{
	if (mounted == 0)
		return;
		
	char _filename[256];
	sprintf(_filename,"%s.dsk",filename);
	savedsk(_filename);	
}

void drive::savedsk(char * _filename)
{
	FILE *fp;

	if (mounted == 0)
		return;
	
	fp=fopen(_filename,"wb");
	if (fp == NULL)
	{
		printf("Drive %d: SAVE DSK ERROR OPENING %s\n",drvnum,_filename);
		return;
	}
		
	if (fwrite(image,1,DISK_SIZE,fp) != DISK_SIZE)
	{
		printf("Drive %d: SAVE DSK ERROR WRITING %s\n",drvnum,_filename);
		return;		 
	}
		
	fclose(fp);	
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

uint8_t drive::getwrite_protect()
{
	return write_protect;
}

void drive::setwrite_protect(uint8_t on)
{
	write_protect = on;
}
		
uint8_t drive::getsequencer()
{
	return sequencer;
}

uint8_t drive::clear()
{
	sequencer = 0;
}

uint8_t drive::read_pulse()
{
	uint8_t value;
	
	value = (curbyte & 0x80) >> 7;
	
	sequencer++;
	curbyte = curbyte << 1;
	if (sequencer == 8)
	{
		ptr++;
		ptr = ptr % BYTES_PER_NIB_TRACK;
		curbyte = track_data[ptr];
		sequencer = 0;
	}

	return value;
}

void drive::print()
{
	printf("DRV: %d HALFTRK: %02d TRK: %02d PTR: %04d BYTE: %02X PHA: %d SEQ: %02X \n",drvnum,track,track/2,ptr,track_data[ptr],phase,sequencer);
}

void drive::load(uint8_t data)
{
	;	
}

disk::disk()
{
	slot = 0;
	sequencer = 0;
	residue_cycles = 0;
	drv1 = new drive(1);
	drv2 = new drive(2);
	activedrv = drv1;
	
	reset();
}

void disk::init(uint8_t _slot)
{
	int i;
	slot = _slot;
	
	for(i = 0; i < 0xFF; i++)
	{
		mem->writeROMSLOT(i,_slot,_DISK_P5A[i]);
	}
	
	printf("DISK II installed at slot %d \n",slot);
}

void disk::print()
{
	activedrv->print();
	printf("DATA_REGISTER "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data_register)); 
}

void disk::step(uint8_t cycles)
{
	uint8_t value;
	int i;

	if (drive_off_on) 
	{
		
		residue_cycles = residue_cycles + cycles;
		for (i=0;i<residue_cycles/CYCLES_PER_STEP;i++)
		{
			if (read_write == 0 && shift_load == 0)
			{
				// READ / SHIFT  
				sequencer = activedrv->getsequencer();
				value = activedrv->read_pulse();
				switch(sequencer)
				{
					case 0:
						break;
					case 1:
						// CLR + BYTEFLAG + READ PULSE
						data_register = 2 | value;	
						break;
					default:
						data_register = (data_register << 1) | value;
				}
			}
			else if (read_write == 0 && shift_load == 1)
			{
				data_register = activedrv->getwrite_protect() << 7;
			}	
		}	
		residue_cycles = residue_cycles % CYCLES_PER_STEP;			
	}
}

uint8_t disk::decoder(uint8_t n, uint8_t on, uint8_t rw, uint8_t data)
{	
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
			if (drive_off_on)
			{
				if (on) 
				{
					activedrv->stepper(n);	
				}	
			}
			sequencer = 0;
			residue_cycles = 0;
			break;
			
		// POWER C088,X C089,X
		case 4:
			drive_off_on = on;
			sequencer = 0;
			residue_cycles = 0;
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
			shift_load = on;
			/*
			if (drive_off_on)
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
			*/
			break;
				
		// READ / WRITE C08E,X C08F,X
		case 7:
			read_write = on;
			activedrv->setmode(on);
			sequencer = 0;
			data_register = 0;
			break;
	}
	
	// Step remaining clock
	step(cpu->getElapsedCycles());
	cpu->resetElapsedCycles();
	
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

void drive::debug(uint8_t *_drvnum, int8_t *_halftrk, uint16_t *_ptr)
{
	*_drvnum = drvnum;
	*_halftrk = track;
	*_ptr = ptr;
}

uint8_t disk::getslot()
{
 	return slot;
}

int disk::diskmount(char *_filename, int drvnum)
{
	int i;
	
	if (drvnum == 1)
		i = drv1->mount(_filename);
	else
		i = drv2->mount(_filename);
		
	return i;
}

void disk::savenib()
{
	activedrv->savenib();	
}

void disk::savenib(char * _filename,int drvnum)
{
	if (drvnum == 1)
		drv1->savenib(_filename);
	else
		drv2->savenib(_filename);
}

void disk::savedsk()
{
	activedrv->savedsk();
}

void disk::savedsk(char * _filename,int drvnum)
{
	if (drvnum == 1)
		drv1->savedsk(_filename);
	else
		drv2->savedsk(_filename);
}

void disk::reset()
{
	sequencer = 0;
	residue_cycles = 0;
	data_register = 0;
	shift_load = 0;
	read_write = 0;
	drive_off_on = 0;
}

void disk::debug(uint8_t *_activedrv,uint8_t *_pwr1, int8_t *_halftrk1, uint16_t *_ptr1,uint8_t *_pwr2, int8_t *_halftrk2, uint16_t *_ptr2)
{
	uint8_t dummy;
	
	activedrv->debug(_activedrv,_halftrk1,_ptr1);
	drv1->debug(&dummy,_halftrk1,_ptr1);
	drv2->debug(&dummy,_halftrk2,_ptr2);
	
	if (*_activedrv == 1)
	{
		*_pwr1 = drive_off_on;
		*_pwr2 = 0;
	}
	else
	{
		*_pwr2 = drive_off_on;
		*_pwr1 = 0;		
	}
}


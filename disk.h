#ifndef __DISK_H
#define __DISK_H

#include <stdint.h>
#include "nibbilizer.h"

extern uint8_t _DISK_P5A[256];

class drive
{
	private:
		uint8_t phase;
		uint8_t *image;
		uint8_t mounted; 
		int8_t track;
		int8_t sector;
		uint8_t rw;
		uint8_t power;
		uint8_t drvnum;
		uint8_t sequencer;
		uint8_t write_protect;
		uint64_t timming;
		nibbilizer *nib;
	
	public:
		drive(uint8_t num);	
		void print();
		int mount(char * filenane);
		void umount();
		void setmode(int rw);
		int getmode();
		uint8_t ismounted();
		void stepper(uint8_t p);
		void setpower(uint8_t p);
		uint8_t readwrite();
		void load(uint8_t data);
		void savenib(char * filename);
		
};

class disk
{
	private:
		uint8_t *RAM;
		uint8_t slot;
		char filename[256];
		drive *drv1;
		drive *drv2;
		drive *activedrv;
		uint8_t data_register;
		
	public:
		disk(uint8_t *_RAM);
		void init(uint8_t _slot);
		uint8_t controller(uint8_t n, uint8_t on, uint8_t rw,uint8_t data);
		uint8_t getslot();
		int diskmount(char *filename, int drvnum);
		void reset();
		void savenib(char * filename,int drvnum);
		
};

#endif

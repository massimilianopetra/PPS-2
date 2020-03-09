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
		uint8_t* track_data;
		uint16_t ptr;
		uint8_t curbyte;
		uint8_t rw;
		uint8_t power;
		uint8_t drvnum;
		uint8_t sequencer;
		uint8_t write_protect;
		nibbilizer *nib;
	
	public:
		drive(uint8_t num);	
		void print();
		int mount(char * filenane);
		void umount();
		void setmode(int rw);
		uint8_t getmode();
		uint8_t getsequencer();
		uint8_t clear();
		uint8_t ismounted();
		void stepper(uint8_t p);
		void setpower(uint8_t p);
		uint8_t getpower();
		uint8_t readwrite();
		void load(uint8_t data);
		void savenib(char * filename);
		uint8_t fetch();
		void debug(uint8_t *_drvnum,uint8_t *_pwr, int8_t *_halftrk, uint16_t *_ptr);
		
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
		void fetch();
		void print();
		void debug(uint8_t *_activedrv,uint8_t *_pwr1, int8_t *_halftrk1, uint16_t *_ptr1,uint8_t *_pwr2, int8_t *_halftrk2, uint16_t *_ptr2);
		
};

#endif

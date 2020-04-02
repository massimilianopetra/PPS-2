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
		uint16_t trackstat[NUM_TRACKS];
		uint16_t ptr;
		uint8_t curbyte;
		uint8_t sequencer;
		uint8_t drvnum;
		uint8_t write_protect;
		nibbilizer *nib;
		char filename[256];
		
		int loaddsk(char * _filenane);
		int loadnib(char * _filenane);
		
	public:
		drive(uint8_t num);	
		void print();
		int mount(char * _filenane);
		void umount();
		uint8_t getwrite_protect();
		void setwrite_protect(uint8_t on);
		uint8_t getsequencer();
		uint8_t clear();
		uint8_t ismounted();
		void stepper(uint8_t p);
		uint8_t readwrite();
		uint8_t read_pulse();
		void load(uint8_t data);
		void savenib();
		void savenib(char * _filename);
		void savedsk();
		void savedsk(char * _filename);
		void debug(uint8_t *_drvnum, int8_t *_halftrk, uint16_t *_ptr);
		void printstat();
};

class disk
{
	private:
		uint8_t slot;
		drive *drv1;
		drive *drv2;
		drive *activedrv;
		
		uint8_t sequencer;
		uint8_t residue_cycles;
		uint8_t data_register;
		uint8_t shift_load;
		uint8_t read_write;
		uint8_t drive_off_on;
		
		
	public:
		disk();
		void init(uint8_t _slot);
		uint8_t decoder(uint8_t n, uint8_t on, uint8_t rw,uint8_t data);
		uint8_t getslot();
		int diskmount(char *filename, int drvnum);
		void reset();
		void savenib();
		void savenib(char * filename,int drvnum);
		void savedsk();
		void savedsk(char * filename,int drvnum);
		void step(uint8_t cycles);
		void print();
		void debug(uint8_t *_activedrv,uint8_t *_pwr1, int8_t *_halftrk1, uint16_t *_ptr1,uint8_t *_pwr2, int8_t *_halftrk2, uint16_t *_ptr2);
		void printstat();
		
};

#endif

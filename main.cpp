/* **********************************************************
   *
   * main.cpp  PPS-2 main module
   *   Author Massimiliano Petra,  february 2020
   *
   **********************************************************/
   
#include <iostream>
#include <stdio.h>
#include <conio.h>

#include "rom.h"
#include "mos6502.h"
#include "system_io.h"
#include "video.h"
#include "shell.h"


#define XTAL 1000000L

using namespace std;


extern const uint8_t _A2ROM[];


mos6502 *cpu = NULL;
ROM *rom;
uint8_t RAM[0x10000];
uint64_t cycles=0;
uint8_t debug_mode = 0;

#define INTCXROM_OFF	0xC006
#define INTCXROM_ON		0xC007
#define INTCXROM_READ	0xC015

uint8_t INTCXROM = 0x00;

#define SLOTC3ROM_OFF	0xC00A
#define SLOTC3ROM_OFF	0xC00B
#define SLOTC3ROM_READ	0xC017

#define _READ  0
#define _WRITE 1


uint8_t SLOTC3ROM = 0x00;


/* ************************** Prototype ********************** */

uint16_t d6502(uint8_t* memory, uint16_t address, uint16_t nline);


/* ********************** Memory BUS Functions *************** */  

uint8_t busreader(uint16_t address)
{
	uint8_t value = 0;
	
	if (address == _KBD) 
	{ 
		value = RAM[_KBD];
	}
	else if (address == _KBDCR)
	{
		RAM[_KBD] = RAM[_KBD] & 0x7f;
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
	else if (address >= 0xC080 && address <= 0xC0FF) 
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
  	else
  	{
  		// read RAM
  		value = RAM[address];	
	}
	
	return value;
}

void buswriter(uint16_t address,uint8_t value)
{
	if (address == _KBDCR)
	{
		RAM[_KBD] = RAM[_KBD] & 0x7f;
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
		// Write value in RAM (ROM is mapped from $D000
		if (address < 0xC100)
			RAM[address] = value;
	}
}


/* ************************** Main ********************** */

int main( int argc, char *argv[] )
{
	int i;
	int slot;
	int drv;
	int pc;
	char filename[256];
	char line[256];
	char cmd[32];
	char p1[32];
	char peripheral[32];
	
	uint16_t start_address;
	uint8_t _A,_X,_Y,_SP,_P;
	uint64_t elapsed;
	uint64_t io_count = 0;
	uint64_t NOW = 0;
	uint64_t LAST = 0;
	double deltaTime = 0;
	FILE *fpcfg = NULL;
	FILE *fp = NULL;

 	printf("***** PPS-2 ***** \n\n");
 	
	// Main loop flag
	bool quit = false;
	
	printf("ROM   init ... \n");
	fflush(stdout);
	rom = new ROM(RAM);
	i = rom->loadROM(0xD000,12288,"A2ROM.bin");
	if (i != 12288)
		printf("Failed to load A2ROM.bin \n");
	i = rom->loadCHARROM("CHARGEN.bin");
	if (i != 0)
		printf("Failed to load CHARGEN.bin \n");

	printf("I/O   init ...  \n");
	IO = new system_io(RAM);
	printf("init OK\n");
			
	// Load config file
	fpcfg = fopen("pps.cfg","r");
	if (fpcfg == NULL)
	{
		printf("Failed to open pps.cfg \n");
	}
	else
	{
		while(fgets(line,255,fpcfg) != NULL)
		{
			sscanf(line,"%s",cmd);
			
			if (strcmp(cmd,"ROM") == 0)
			{
				i = sscanf(line,"%s %s %s",cmd,p1,filename);	
				if (i == 3)
				{
					start_address = strtoul (p1, NULL, 16);
					if (start_address >= 0xC100)
					{
						i = rom->loadROM(start_address,filename);
						if (i > 0)
							printf("upload %d bytes at %04X from: %s \n",i,start_address,filename);
					}
				}
			}
			else if (strcmp(cmd,"SLOT") == 0)
			{
				i = sscanf(line,"%s %s %s",cmd,p1,peripheral);	
				if (i == 3)
				{
					sscanf(p1,"%d",&slot);
					if (strcmp(peripheral,"DISK") == 0)
						IO->diskslot(slot);						
				}
			}
			else if (strcmp(cmd,"MOUNT") == 0)
			{
				i = sscanf(line,"%s %s %s",cmd,p1,filename);	
				if (i == 3)
				{
					sscanf(p1,"%d",&drv);
					if (drv >= 1 && drv <= 2)
					{
						i = IO->diskmount(filename,drv);
						/*
						if (i == 0)
							printf("Disk mounted on DRIVE %d with FILE: %s\n",drv,filename);
						else if (i == -1)
							printf("Disk can't open FILE: %s\n",filename);
						else if (i == -2)
							printf("Disk wrong format FILE: %s\n",filename);	
						*/
					}	
				}
			}
		}
	}
	
	fclose(fpcfg);
	
	printf("init OK\n");
	
	printf("Video init ...  \n");
	video_init();
	//video_test();
	printf("init OK\n");


	printf("CPU   init ...  \n");	
	cpu = new mos6502(busreader,buswriter);
	printf("init OK\n");
	
	printf("\n");
	
	while(!quit)
	{
		NOW = SDL_GetPerformanceCounter();	
        
        if (debug_mode)
        {
        	pc = cpu->Dump(&_A,&_X,&_Y,&_SP,&_P);
        	
        	printf("A-%02X X-%02X Y-%02X S-%02X P-%02X ",_A,_X,_Y,_SP,_P);

			if (_P & OVERFLOW) printf("O");
			if (_P & BREAK) printf("B");
			if (_P & DECIMAL) printf("D");
			if (_P & INTERRUPT) printf("I");
			if (_P & ZERO) printf("Z");
			if (_P & CARRY) printf("C");
			printf("\n");
	
        	//printf("[%04X] = %02X ; [%04X] = %02X \n",_KBD,RAM[_KBD],_KBDCR,RAM[_KBDCR] );
        	d6502(RAM,pc,1);
        	printf("---------------------------------\n");
        	i = IO->keyboard();
        	video_refresh(RAM);
        	
        	while (i != 7 && i != 6)
			{
				i = IO->keyboard();
			} 
			
			if (i == 6)
			{
				debug_mode = 0;
				printf("***** DEBUG MODE OFF *****\n");
			}	
		}
        
        
        elapsed = cpu->Run(1,cycles);
        IO->paddle_timer(elapsed);
        
        io_count += elapsed;
        
        // Update I/O 50Hz
        if (io_count > (XTAL/50))
        {
        	switch(IO->keyboard())
        	{
        		case 1:
        			// Reset
        			printf("***** RESET *****\n");
        			clearscreen();
        			cpu->Reset();
        			IO->Reset();
        			INTCXROM = 0x00;
        			SLOTC3ROM = 0x00;
        			break;
        		case 2:
        			// Load
        			printf("***** LOAD *****\n");
        			printf("Insert filename :\n");
        			scanf("%s",filename);
        			load(filename,RAM,cpu);
        			break;
        		case 3:
        			// Cleat Screen
        			printf("***** CLEAR SCREEN *****\n");
        			clearscreen();
        			break;
        		case 4:
        			// Quit
        			printf("***** QUIT *****\n");
        			quit = true;
        			break;
        		case 5:
        			// Quit
        			printf("***** PASTE *****\n");
					paste(RAM,cpu);
        			break;
        		case 6:
        			// Debug
        			debug_mode = 1;
        			printf("***** DEBUG MODE ON *****\n");
        			break;
        		case 8:
        			// Shell Prompt
        			shell_prompt(RAM,cpu);
        			break;
        			
        		default:
        			// Do nothing
        			break;
			}
			
			video_refresh(RAM);
			
			io_count = 0;
    	}
    	
    	// Wait for 6502 1 MHz accuracy
    	deltaTime = (double)((NOW - LAST)*1000000 / (double)SDL_GetPerformanceFrequency());
    	while(deltaTime < elapsed)
    	{
    		NOW = SDL_GetPerformanceCounter();
    		deltaTime = (double)((NOW - LAST)*1000000 / (double)SDL_GetPerformanceFrequency());
		}
		
    	LAST = NOW;
   } 


	//Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}

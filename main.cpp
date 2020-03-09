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
#include "memory.h"
#include "gui.h"

#define XTAL 1000000L

using namespace std;

mos6502 *cpu = NULL;


uint64_t cycles=0;
uint8_t debug_mode = 0;
uint16_t break_address[256];
uint8_t num_break = 0;
int32_t xtal = XTAL;





/* ************************** Prototype ********************** */

uint16_t d6502(uint16_t address, uint16_t nline);



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
	
	uint8_t _A,_X,_Y,_SP,_P;
	uint8_t gui_count = 0;
	uint16_t start_address;
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
	
	printf("MEMORY   init ... \n");
	fflush(stdout);
	
	mem = new memory();
	
	i = mem->loadROM(0xD000,12288,"A2ROM.bin");
	if (i != 12288)
		printf("Failed to load A2ROM.bin \n");
	i = mem->loadCHARROM("CHARGEN.bin");
	if (i != 0)
		printf("Failed to load CHARGEN.bin \n");

	printf("init OK\n");
	
	printf("I/O   init ...  \n");
	IO = new system_io(mem->getRAM());
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
			cmd[0] = '\0';
			sscanf(line,"%s",cmd);
			
			if (strcmp(cmd,"ROM") == 0)
			{
				i = sscanf(line,"%s %s %s",cmd,p1,filename);	
				if (i == 3)
				{
					start_address = strtoul (p1, NULL, 16);
					if (start_address >= 0xC100)
					{
						i = mem->loadROM(start_address,filename);
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
				i = sscanf(line, "%s %d",cmd,&drv);
				if (i == 2)
				{
					if (drv == 1 || drv == 2)
					{
						i = sscanf(line, "%*[^']'%[^']'%*[^\n]",filename);
						if (i == 1)
						{
							i = IO->diskmount(filename,drv);
						}
						else
						{	
							printf("*** Mount wrong filename\n");
						}
					}
					else
					{
						printf("*** Mount wrong disk number %d \n",drv);
					}	
				}
				else
				{
					printf("*** Mount wrong mount command\n");
				}
				
			}
			else if (strcmp(cmd,"BRK") == 0)
			{
				i = sscanf(line,"%s %s %s",cmd,p1);	
				if (i == 2)
				{
					start_address = strtoul (p1, NULL, 16);
						
				}
			}
			else if (strcmp(cmd,"XTAL") == 0) 
			{
				i = sscanf(line,"%s %s %s",cmd,p1);
				if (i == 2)
				{
					sscanf(p1,"%d",&i);
					if (i >=  1000000)
					{
						xtal = i;
						printf("SET XTAL: %.2f MHz\n",xtal/1000000.);	
					}
					else
					{
						printf("*** SET XTAL too low: %.2f MHz\n",i/1000000.);
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
        	d6502(pc,1);
        	printf("\n");
        	IO->diskprint();
			printf("---------------------------------\n");
        	
        	i = IO->keyboard();
        	video_refresh(mem->getRAM());
        	
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
        IO->diskfetch();
        
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
        			mem->Reset();
#ifdef WITHGUI
					initgui();
#endif
        			break;
        		case 2:
        			// Load
        			printf("***** LOAD *****\n");
        			printf("Insert filename :\n");
        			scanf("%s",filename);
        			load(filename,mem->getRAM(),cpu);
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
					paste(mem->getRAM(),cpu);
        			break;
        		case 6:
        			// Debug
        			debug_mode = 1;
        			printf("***** DEBUG MODE ON *****\n");
        			break;
        		case 8:
        			// Shell Prompt
        			shell_prompt(mem->getRAM(),cpu);
        			break;
        			
        		default:
        			// Do nothing
        			break;
			}
			
			video_refresh(mem->getRAM());
			
#ifdef WITHGUI
			if (gui_count == 0)
				refreshgui();
			gui_count ++;
			gui_count %= 25;
#endif
			
			io_count = 0;
    	}
    	
    	// Wait for 6502 1 MHz accuracy
    	deltaTime = (double)((NOW - LAST)*xtal / (double)SDL_GetPerformanceFrequency());
    	while(deltaTime < elapsed)
    	{
    		NOW = SDL_GetPerformanceCounter();
    		deltaTime = (double)((NOW - LAST)*xtal / (double)SDL_GetPerformanceFrequency());
		}
		
    	LAST = NOW;
   } 

	//Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}

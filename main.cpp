/*
 * main.cpp 
 *
 * Author: Massimiliano Petra <massimiliano.petra@gmail.com> February, 2020
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
   
#include <iostream>
#include <stdio.h>
#include <conio.h>

#include "cpu_6502.h"
#include "system_io.h"
#include "video.h"
#include "shell.h"
#include "memory.h"
#include "gui.h"
#include "audio.h"

#define XTAL 1000000L

using namespace std;

cpu_6502 *cpu = NULL;



uint64_t cycles=0;
uint64_t halt=0;
uint8_t tick_break = 0;
uint8_t debug_mode = 0;
uint16_t break_address[256];
uint8_t num_break = 0;
int32_t xtal = XTAL;
uint8_t guidebug = 0;

uint8_t brk[0x10000];


/* ************************** Prototype ********************** */

uint16_t disassembly(uint16_t address, uint8_t source);



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
	Uint32 flag;
	uint8_t isFullscreen;

	printf("=========================\n");
 	printf("=====     PPS-2     =====\n");
 	printf("=========================\n\n");
	// Main loop flag
	bool quit = false;
	
	printf("MEMORY   init ... \n");
	fflush(stdout);
	
	mem = new memory();
	
	printf("init OK\n");
	
	printf("I/O   init ...  \n");
	IO = new system_io();
	printf("init OK\n");
			
	// Load config file
	printf("CONFIG  init ...  \n");
	fpcfg = fopen("pps.cfg","r");
	if (fpcfg == NULL)
	{
		printf("Failed to open pps.cfg \n");
	}
	else
	{
		while(fgets(line,255,fpcfg) != NULL)
		{
			shell_cmd(line,1);
		}
	}
	
	fclose(fpcfg);
	
	printf("init OK\n");
	
	printf("Video init ...  \n");
	video_init();
	//video_test();
	printf("init OK\n");

	printf("Audio init ...  \n");
	AUDIO = new audio();
	if (xtal == 1000000)
		AUDIO->on();
	printf("init OK\n");
	
	printf("CPU   init ...  \n");	
	
	cpu = new cpu_6502(busreader,buswriter);
	
	printf("init OK\n");
	
	printf("\n");
	
	while(!quit)
	{
		NOW = SDL_GetPerformanceCounter();	
        
        /***** DEBUG MODE *****/
        if (debug_mode)
        {
        	pc = cpu->Dump(&_A,&_X,&_Y,&_SP,&_P);
        	
        	printf("A-%02X X-%02X Y-%02X S-%02X P-%02X TICK-%012lX   ",_A,_X,_Y,_SP,_P,cpu->getTick());

			if (_P & _NEGATIVE) printf("N");
			if (_P & _OVERFLOW) printf("O");
			if (_P & _BREAK) printf("B");
			if (_P & _DECIMAL) printf("D");
			if (_P & _INTERRUPT) printf("I");
			if (_P & _ZERO) printf("Z");
			if (_P & _CARRY) printf("C");
			printf("\n");
	
        	//printf("[%04X] = %02X ; [%04X] = %02X \n",_KBD,RAM[_KBD],_KBDCR,RAM[_KBDCR] );
        	disassembly(pc,1);
        	printf("\n");
        	IO->diskprint();
			printf("---------------------------------\n");
        	
        	i = IO->keyboard();
        	video_refresh(mem->getRAM());
        	
        	while (i != 7 && i != 6 && i != 8)
			{
				i = IO->keyboard();
			} 
			
			if (i == 6)
			{
				debug_mode = 0;
				printf("***** DEBUG MODE OFF *****\n");
			}	
			
			if (i == 8)
			{
				debug_mode = 0;
				printf("***** DEBUG MODE OFF *****\n");
				shell_prompt();
			}	
			
		}
        
        /***** CPU CYCLE *****/
        pc = cpu->Step();
        
        if (brk[pc] || (tick_break == 1 && (cpu->getTick() >= halt)))   
        {
        	// Delet Halt command
        	if (tick_break == 1 && (cpu->getTick() >= halt))
        		tick_break = 0;
        	
        	printf("*** BREAKPOINT at %04X (FROM %04X)\n",pc,cpu->getFROM());
        	debug_mode = 1;
        	if (brk[pc] == 2)
        		brk[pc] = 0;
		}
		
        if (cpu->getIllegalOpcode())
        {
        	pc = cpu->Dump(&_A,&_X,&_Y,&_SP,&_P);
        	
        	printf("*** ILLEGAL OPCODE %02X at %04X (FROM %04X)\n",mem->read(pc-1),pc-1,cpu->getFROM());
        	printf("\n");
        	printf("A-%02X X-%02X Y-%02X S-%02X P-%02X TICK-%012lX   ",_A,_X,_Y,_SP,_P,cpu->getTick());

			if (_P & _OVERFLOW) printf("O");
			if (_P & _BREAK) printf("B");
			if (_P & _DECIMAL) printf("D");
			if (_P & _INTERRUPT) printf("I");
			if (_P & _ZERO) printf("Z");
			if (_P & _CARRY) printf("C");
			printf("\n");
			disassembly(pc,1);
        	printf("\n");
			IO->diskprint();
			printf("---------------------------------\n\n");
			
			shell_prompt();	
		}
        
        elapsed = cpu->getElapsedCycles();
        cpu->resetElapsedCycles();
        
        /***** TIMER *****/
        IO->timer(elapsed);
        
        /***** DISK I/O *****/
        IO->diskstep(elapsed);
        
        io_count += elapsed;
        
        /***** I/O and VIDEO 50Hz *****/
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
					if (guidebug)
						initgui();
#endif
        			break;
        		case 2:
        			// Load
        			printf("***** LOAD TEXT *****\n");
        			printf("Insert filename :\n");
        			scanf("%s",filename);
        			load(filename,mem->getRAM());
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
					paste(mem->getRAM());
        			break;
        		case 6:
        			// Debug
        			debug_mode = 1;
        			printf("***** DEBUG MODE ON *****\n");
        			break;
        		case 8:
        			// Shell Prompt
        			shell_prompt();
        			break;
        		case 9:
        			// fULL SCREEN
        			printf("***** SWITCH FULL SCREEN  *****\n");
        			flag = SDL_WINDOW_FULLSCREEN ;
    				isFullscreen = SDL_GetWindowFlags(window) & flag;
					SDL_SetWindowFullscreen(window, isFullscreen ? 0 : flag);
					SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    				// Get window surface
            		//screenSurface = SDL_GetWindowSurface(window);
            		//Main_Renderer =	SDL_CreateSoftwareRenderer(screenSurface);
    				break;
        		case 11:
        			// Save NIB
        			if (shell_query("Save NIB y/n ? "))
					{
						IO->disksavenib();
						printf("NIB save OK\n");
					}
        			break;
        		case 12:
        			// Save DSK
        			if (shell_query("Save DSK y/n ? "))
        			{
        				IO->disksavedsk();
        				printf("DSK save OK\n");
					}
        			break;
        		default:
        			// Do nothing
        			break;
			}
			
			video_refresh(mem->getRAM());
			AUDIO->refresh();
			
#ifdef WITHGUI
			if (gui_count == 0 && guidebug == 0x80)
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

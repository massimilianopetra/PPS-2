
/* **********************************************************
   *
   * shell.cpp
   *   Author Massimiliano Petra,  february 2020
   *
   **********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "mos6502.h"
#include "system_io.h"
#include "video.h"
#include "SDL2/SDL.h"
   
/* ************************** Prototype ********************** */

uint16_t d6502(uint8_t* memory, uint16_t address, uint16_t nline);

uint16_t __last_address = 0;

void paste(uint8_t *RAM,mos6502 *cpu)
{
	SDL_Event event;
	uint64_t cycles;
	char *clipboard;
	int i,pc;
	int ch = 0;
	
	int cc=0;
		
	clipboard = SDL_GetClipboardText();
	
	if (clipboard != NULL)
	{
		i = 0;
		
		while(clipboard[i] != '\0')
		{
			ch = clipboard[i];
			
			// Wait keyboard ready
			cc=0;
			while(RAM[_KBD] & 0x80)
			{
				pc = cpu->Run(1,cycles);
			}
			
			if (ch == 0x0a)
				ch = 0x0d;
			
			RAM[_KBD] = ch|0x80;
			
			i++;
				
			SDL_PollEvent(&event);
			//SDL_Delay(1);
		}
		
		// Wait keyboard ready
		while(RAM[_KBD] & 0x80)
			cpu->Run(1,cycles);
		
		IO->keyboard_init();
	}	
}

void load(char *filename,uint8_t *RAM,mos6502 *cpu)
{
	FILE *fp=NULL;
	int ch = 0;
	uint64_t cycles;
		
	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		printf("Error opening %s \n",filename);
	}
	else
	{
		SDL_Event event;
	
		while((ch=fgetc(fp)) != EOF)
		{
			// Wait keyboard ready
			while(RAM[_KBD] & 0x80)
				cpu->Run(1,cycles);
			
			if (ch == 0x0a)
				ch = 0x0d;
			
			RAM[_KBD] = ch|0x80;
				
			SDL_PollEvent(&event);
			//SDL_Delay(1);
		}
		
		// Wait keyboard ready
		while(RAM[_KBD] & 0x80)
			cpu->Run(1,cycles);
		
		IO->keyboard_init();

		//close the file
		fclose(fp);        			    					
	}
}

void print_ascii(uint8_t *RAM, uint16_t address, uint16_t length)
{
	int i;
	printf("%04X: ",address);
	
	for (i=0;i<length;i++)
		putchar(RAM[address+i]);
	printf("\n");
}

void print_hex(uint8_t *RAM, uint16_t address, uint16_t length)
{
	int i;
	int k=0;
	
	for (i=0;i<length;i++)
	{
		if ((i % 8) == 0)
			printf("\n%04X: ",address+i);
			
		printf("%02X ",RAM[address+i]);
	}	
	printf("\n");
}

void shell_prompt(uint8_t *RAM,mos6502 *cpu)
{
	char line[256];
	char cmd[32];
	char operand[64];
	char operand2[64];
	uint16_t address;
	int param;
	int len;
	int i;
	
	while(1)
	{
		printf("> ");
		fgets(line,255,stdin);
		param = sscanf(line,"%s %s %s",cmd,operand,operand2);
		
		if ((strcmp(cmd,"quit") == 0) ||  (strcmp(cmd,"q") == 0) || (strcmp(cmd,"Q") == 0))
		{
			return;
		}
		else if ( (strcmp(cmd,"d") == 0) || (strcmp(cmd,"D") == 0) )
		{
			// Disassemble
			if (param == 1)
				address = __last_address;
			else if (param == 2)
			{
				address = (uint16_t)strtol(operand, NULL, 16);
				i = 10;
			}
			else
			{
				address = (uint16_t)strtol(operand, NULL, 16);
				sscanf(operand2,"%d",&i);				
			}
				
			__last_address = d6502(RAM,address,i);
		}
		else if ( (strcmp(cmd,"pa") == 0) || (strcmp(cmd,"PA") == 0) )
		{
			// Print Ascii
			if (param == 3)
			{
				address = (uint16_t)strtol(operand, NULL, 16);
				len = (uint16_t)strtol(operand2, NULL, 10);
				print_ascii(RAM,address,len);
			}
		}
		else if ( (strcmp(cmd,"ph") == 0) || (strcmp(cmd,"PH") == 0) )
		{
			// Print Hex
			if (param == 3)
			{
				address = (uint16_t)strtol(operand, NULL, 16);
				len = (uint16_t)strtol(operand2, NULL, 10);
				print_hex(RAM,address,len);
			}
		}
		else if ( (strcmp(cmd,"ss") == 0) || (strcmp(cmd,"SS") == 0) )
		{
			// Showswitches
			printf("SCREEN SWITCHES = %02X\n",screen_switches);

		}		
		else if ( (strcmp(cmd,"savenib") == 0) || (strcmp(cmd,"SAVENIB") == 0) )
		{
			if (param == 3)
			{
				sscanf(operand,"%d",&i);
				if (i>0 && i<3)
				{
					IO->disksavenib(operand2,i);	
				}
				else
				{
					printf("*** Disknumber out of range\n");
				}
				
			}
			else
			{
				printf("*** Syntax Error\n");
			}
		}
		else if ( (strcmp(cmd,"mount") == 0) || (strcmp(cmd,"MOUNT") == 0) )
		{
			if (param == 3)
			{
				sscanf(operand,"%d",&i);
				if (i>0 && i<3)
				{
					IO->diskmount(operand2,i);	
				}
				else
				{
					printf("*** Disknumber out of range\n");
				}
				
			}
			else
			{
				printf("*** Syntax Error\n");
			}
		}
		else if (param >0)
		{
			printf("*** Unknown Command\n");
		}

	}
}


/*
 * shell.cpp 
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
   

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"
#include "memory.h"
#include "mos6502.h"
#include "cpu_6502.h"
#include "system_io.h"
#include "video.h"
#include "SDL2/SDL.h"
   
/* ************************** Prototype ************************* */

uint16_t disassembly(uint16_t address, uint8_t source);
void shell_assembly();

/* ********************* External Variables ********************** */

extern int32_t xtal;
//extern mos6502 *cpu;
extern cpu_6502 *cpu;
extern uint8_t guidebug;
extern uint8_t brk[0x10000];
extern uint64_t halt;
extern uint8_t tick_break;

/* ********************* Private Variables *********************** */

uint16_t __last_address = 0;

uint8_t shell_query(char *prompt)
{
	char answer[256];
	uint8_t query = 0;
	
	printf("%s",prompt);
	scanf("%s",answer);
	
	if (strcmp(answer,"y")==0)
		query = 1;
	else if (strcmp(answer,"Y")==0)
		query = 1;		
	else if (strcmp(answer,"yes")==0)
		query = 1;		
	else if (strcmp(answer,"YES")==0)
		query = 1;		

	return query;
}
void paste(uint8_t *RAM)
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
				cpu->Step();
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
			cpu->Step();
		
		IO->keyboard_init();
	}	
}

void load(char *filename,uint8_t *RAM)
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
				cpu->Step();
			
			if (ch == 0x0a)
				ch = 0x0d;
			
			RAM[_KBD] = ch|0x80;
				
			SDL_PollEvent(&event);
			//SDL_Delay(1);
		}
		
		// Wait keyboard ready
		while(RAM[_KBD] & 0x80)
			cpu->Step();
		
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

void print_hex(uint8_t *m, uint16_t address, uint16_t length)
{
	int i;
	int k=0;
	
	for (i=0;i<length;i++)
	{
		if ((i % 8) == 0)
			printf("\n%04X: ",address+i);
			
		printf("%02X ",m[address+i]);
	}	
	printf("\n");
}

int shell_cmd(char *line, uint8_t offline)
{
	char cmd[32];
	char operand[64];
	char operand2[64];
	uint16_t address;
	int param;
	int len;
	int i;
	int j;
	int slot;
	int drv;
	
	cmd[0]='\0';
	param = sscanf(line,"%s %s %s",cmd,operand,operand2);
	for (i=0;i<strlen(cmd);i++)
		cmd[i] = (char)toupper((int)cmd[i]);
	
	if ((strcmp(cmd,"GO") == 0) || (strcmp(cmd,"G") == 0))
	{
		if (param == 2)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			if (cpu != NULL)
			{
				// Set Program Counter to address (JMP emulation)
				cpu->setPC(address);
			}
		}
		
		return -999;
	}
	else if ((strcmp(cmd,"A") == 0) && offline == 0)
	{
		// Asassemble
		shell_assembly();
	}
	else if ((strcmp(cmd,"D") == 0) && offline == 0)
	{
		// Disassemble
		if (param == 1)
		{
			address = __last_address;
			i = 10;
		}
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
		
		__last_address = address;
		for (j=0;j<i;j++)
		{
			__last_address = disassembly(__last_address,1);
		}
	}
	else if ((strcmp(cmd,"BRK") == 0))
	{
		// Breakpoint
		if (param >= 2)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			if (address > 0 && address < 0x10000)
				brk[address]=1;
		}
		else
		{
			printf("*** BRK MISSING ADDRESS\n");
		}
	}
	else if ((strcmp(cmd,"HALT") == 0))
	{
		// System Halt Overruning Tick
		if (param >= 2)
		{
			halt = (uint64_t)strtol(operand, NULL, 16);
			tick_break = 1;
		}
		else
		{
			printf("*** HALT MISSING TICK\n");
		}
	}
	else if ((strcmp(cmd,"STEPTO") == 0))
	{
		// Breakpoint
		if (param >= 2)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			if (address > 0 && address < 0x10000)
				brk[address]=2;
		}
		else
		{
			printf("*** STEPTO MISSING ADDRESS\n");
		}
	}
	else if ((strcmp(cmd,"DELBRK") == 0))
	{
		// Delete Breakpoint
		if (param == 2)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			if (address > 0 && address < 0x10000)
				brk[address]=0;
		}
		else
		{
			printf("*** Syntax Error\n");
		}
	}
	else if ((strcmp(cmd,"PRINT") == 0) && offline == 0)
	{
		// Print Ascii
		if (param == 3)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			len = (uint16_t)strtol(operand2, NULL, 10);
			print_ascii(mem->getRAM(),address,len);
		}
		else if (param == 2)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			len = 8;
			print_ascii(mem->getRAM(),address,len);				
		}
	}
	else if ((strcmp(cmd,"DUMPRAM") == 0) && offline == 0)
	{
		// Print Hex
		if (param == 3)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			len = (uint16_t)strtol(operand2, NULL, 10);
			print_hex(mem->getRAM(),address,len);
		}
		else if (param == 2)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			len = 8;
			print_hex(mem->getRAM(),address,len);				
		}
	}
	else if ((strcmp(cmd,"DUMPROM") == 0) && offline == 0)
	{
		// Print Hex
		if (param == 3)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			len = (uint16_t)strtol(operand2, NULL, 10);
			print_hex(mem->getROM(),address,len);
		}
		else if (param == 2)
		{
			address = (uint16_t)strtol(operand, NULL, 16);
			len = 8;
			print_hex(mem->getROM(),address,len);				
		}
	}
	else if ((strcmp(cmd,"SHOWSWITCH") == 0) && offline == 0)
	{
		// Showswitches
		printf("SCREEN SWITCHES = %02X\n",screen_switches);

	}		
	else if ((strcmp(cmd,"SAVENIB") == 0) )
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
	else if ((strcmp(cmd,"DISKSTAT") == 0) && offline == 0)
	{
		IO->diskprintstat();
	}
	else if ((strcmp(cmd,"MOUNT") == 0))
	{
		i = sscanf(line, "%s %d",cmd,&drv);
		if (i == 2)
		{
			if (drv == 1 || drv == 2)
			{
				i = sscanf(line, "%*[^']'%[^']'%*[^\n]",operand2);
				if (i == 1)
				{
					i = IO->diskmount(operand2,drv);
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
	else if ((strcmp(cmd,"XTAL") == 0) )
	{
		if (param == 2)
		{
			sscanf(operand,"%d",&i);
			if (i >=  1000000)
			{
				xtal = i;	
			}
			else
			{
				printf("*** XTAL too low\n");
			}
		}
		else
		{
			printf("*** Syntax Error\n");
		}
	}
	else if ((strcmp(cmd,"RESET") == 0) && offline == 0) 
	{
		printf("***** RESET *****\n");
		cpu->Reset();
	}
	else if (strcmp(cmd,"SLOT") == 0)
	{
		if (param == 3)
		{
			sscanf(operand,"%d",&slot);
			if (strcmp(operand2,"DISK") == 0)
				IO->diskslot(slot);						
		}
	}
	else if (strcmp(cmd,"LOADROM") == 0)
	{	
		if (param == 3)
		{
			address = strtoul (operand, NULL, 16);
			if (address >= 0xC000)
			{
				i = mem->loadROM(address,operand2);
				if (i > 0)
					printf("upload %d bytes at %04X from: %s \n",i,address,operand2);
			}
		}
		else
		{
			printf("*** Syntax Error\n");
		}
	}
	else if (strcmp(cmd,"CHARGEN") == 0)
	{
		if (param == 2)
		{
			i = mem->loadCHARROM(operand);
			if (i == 0)
				printf("loaded character generator %s \n",operand);
			else
				printf("ERROR loading character generator %s [%d]\n",operand,i);
		}
		else
		{
			printf("*** Syntax Error\n");
		}
	}
#ifdef WITHGUI
	else if ((strcmp(cmd,"GUIDEBUG") == 0) && offline == 1)
	{
		guidebug = 0x80;
		printf("*** GUI DEBUG ENABLED\n");
	}
#endif	
	else if (param > 0 && offline == 0)
	{
		printf("*** Unknown Command [%s]\n",cmd);
	}
	
	return 0;
}

void shell_prompt()
{
	char line[256];
	int res = 0;
	
	while(res != -999)
	{
		printf("> ");
		fgets(line,255,stdin);
		res = shell_cmd(line, 0);
	}
}


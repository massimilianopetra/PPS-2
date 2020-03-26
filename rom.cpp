
#include <stdio.h>
#include <stdint.h>

#include "rom.h"

uint8_t _CHARROM[2048];

ROM::ROM(uint8_t *_MEMORY)
{
	MEMORY = _MEMORY;	
	
}

int ROM::loadROM(uint16_t startAddress, uint16_t len, char *filename)
{
	FILE *fp=NULL;
	int i;
	
	fp=fopen(filename,"rb");
	if (fp == NULL)
		return -1;
	i = fread(&(MEMORY[startAddress]),1,len,fp); 
	if (i != len)
		return -2;
	fclose(fp);
	return i;	
}

int ROM::loadROM(uint16_t startAddress, char *filename)
{
	FILE *fp=NULL;
	int i=0;
	uint8_t b;
	
	fp=fopen(filename,"rb");
	if (fp == NULL)
		return -1;
	
	while(fread(&b,1,1,fp) == 1) 
	{
		if (startAddress+i >= 0xc100)
			MEMORY[startAddress+i]=b;
		i++;
	}
	
				 
		
	fclose(fp);
	return i;	
}

int ROM::loadCHARROM(char* filename)
{
	FILE *fp=NULL;
	int i;
	
	fp=fopen(filename,"rb");
	if (fp == NULL)
		return -1;
	i = fread(_CHARROM,1,2048,fp); 
	if (i != 2048)
		return -2;
	fclose(fp);
	return 0;
}

#include <stdio.h>

#include "video.h"
#include "gui.h"
#include "system_io.h"
#include "memory.h"
#include "rom.h"


#define DRV1STARTX  10
#define DRV2STARTX 100
#define DRV1STARTY  (SCREEN_HEIGHT+30)
#define DRV2STARTY  (SCREEN_HEIGHT+30)
#define HRAMSTARTX 180
#define HRAMSTARTY  (SCREEN_HEIGHT+30)



drawchar(int x,int y,uint8_t c, uint8_t r, uint8_t g, uint8_t b,uint8_t big)
{
	int i,j;
	uint8_t char_byte;
	
	SDL_SetRenderDrawColor(Main_Renderer, r, g, b, 255);
	
	// ASCII Translation
	if (c >= '@')
	{
		c = c - '@';
	}
	else
	{
		c = c - ' '+32;
	}
	
	for (i=0;i<8;i++)
	{
		char_byte = _CHARROM[c*8+i] & 0b01111110;
	
		for (j = 0; j<8;j++)
		{
			if (REVERSE_CHAR_ROM)
			{
				if (char_byte & 0x01)
				{
					if (big)
					{
						SDL_RenderDrawPoint(Main_Renderer, x+j*2, y+i*2);
						SDL_RenderDrawPoint(Main_Renderer, x+j*2+1, y+i*2);
						SDL_RenderDrawPoint(Main_Renderer, x+j*2, y+i*2+1);
						SDL_RenderDrawPoint(Main_Renderer, x+j*2+1, y+i*2+1);
					}
					else
						SDL_RenderDrawPoint(Main_Renderer, x+j, y+i);
				}
			
				char_byte = char_byte >> 1;	
			}
			else
			{
				if (char_byte & 0x80)
				{
					if (big)
					 {
						SDL_RenderDrawPoint(Main_Renderer, x+j*2, y+i*2);
						SDL_RenderDrawPoint(Main_Renderer, x+j*2+1, y+i*2);
						SDL_RenderDrawPoint(Main_Renderer, x+j*2, y+i*2+1);
						SDL_RenderDrawPoint(Main_Renderer, x+j*2+1, y+i*2+1);					 	
					 }
					else
						SDL_RenderDrawPoint(Main_Renderer, x+j, y+i);
				}

				char_byte = char_byte << 1;
			}						
		}
	}
												
}

drawstring(int x,int y,char *s, uint8_t r, uint8_t g, uint8_t b,uint8_t big)
{
	int i=0;

	while(s[i] != '\0')
	{
		drawchar(x,y,s[i],r,g,b,big);
		if (big)
			x += 16;
		else
			x += 9;
		
		i++;	
	}
	
}

void initgui()
{
    // Clear GUI
    
    SDL_Rect rect;
    rect.x = 0;
    rect.y = SCREEN_HEIGHT;
    rect.w = SCREEN_WIDTH;
    rect.h = SCREEN_HEIGHT_DBG-SCREEN_HEIGHT;
    
    SDL_SetRenderDrawColor(Main_Renderer, 150, 150, 150, 255);
	SDL_RenderFillRect(Main_Renderer, &rect);

    rect.x = 0+4;
    rect.y = SCREEN_HEIGHT+4;
    rect.w = SCREEN_WIDTH-8;
    rect.h = SCREEN_HEIGHT_DBG-SCREEN_HEIGHT-8;
    
    SDL_SetRenderDrawColor(Main_Renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(Main_Renderer, &rect);

	// DISK II
	
	drawstring(DRV1STARTX+20,DRV1STARTY-21,"DISK II",0,0,0,1);
		
	// DRIVE1
	rect.x = DRV1STARTX;
	rect.y = DRV1STARTY;
	rect.w = 60;
	rect.h = 20;	
	SDL_SetRenderDrawColor(Main_Renderer, 80, 80, 80, 255);
	SDL_RenderFillRect(Main_Renderer, &rect);
    SDL_SetRenderDrawColor(Main_Renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(Main_Renderer, &rect);
	rect.x = DRV1STARTX+20;
	rect.y = DRV1STARTY+8;
	rect.w = 16;
	rect.h = 4;	
    SDL_SetRenderDrawColor(Main_Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Main_Renderer, &rect);
	SDL_RenderDrawLine(Main_Renderer, DRV1STARTX+10,DRV1STARTY+9,DRV1STARTX+50,DRV1STARTY+9);
	
	// DRIVE2
	rect.x = DRV2STARTX;
	rect.y = DRV2STARTY;
	rect.w = 60;
	rect.h = 20;
	SDL_SetRenderDrawColor(Main_Renderer, 80, 80, 80, 255);
	SDL_RenderFillRect(Main_Renderer, &rect);	
    SDL_SetRenderDrawColor(Main_Renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(Main_Renderer, &rect);
	rect.x = DRV2STARTX+20;
	rect.y = DRV2STARTY+8;
	rect.w = 16;
	rect.h = 4;	
    SDL_SetRenderDrawColor(Main_Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Main_Renderer, &rect);
	SDL_RenderDrawLine(Main_Renderer, DRV2STARTX+10,DRV2STARTY+9,DRV2STARTX+50,DRV2STARTY+9);
	
	
	// HRAM
	
	drawstring(HRAMSTARTX-2,HRAMSTARTY-21,"HRAM",0,0,0,1);
	drawchar(HRAMSTARTX+5,HRAMSTARTY+10,'R',0,0,0,0);
	drawchar(HRAMSTARTX+18,HRAMSTARTY+10,'W',0,0,0,0);
	drawchar(HRAMSTARTX+31,HRAMSTARTY+10,'1',0,0,0,0);
	drawchar(HRAMSTARTX+44,HRAMSTARTY+10,'2',0,0,0,0);
	
	rect.x = HRAMSTARTX;
	rect.y = HRAMSTARTY;
	rect.w = 62;
	rect.h = 20;
	SDL_SetRenderDrawColor(Main_Renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(Main_Renderer, &rect);
	
    SDL_UpdateWindowSurface(window);	
}

void refreshgui()
{
	SDL_Rect rect;
	
	uint8_t _activedrv;
	uint8_t _pwr1; 
	int8_t _halftrk1;
	uint16_t _ptr1;
	uint8_t _pwr2; 
	int8_t _halftrk2; 
	uint16_t _ptr2;
	
	IO->diskdebug(&_activedrv,&_pwr1,&_halftrk1,&_ptr1,&_pwr2,&_halftrk2,&_ptr2);

	// DRIVE1 POWER LED  
	
	rect.x = DRV1STARTX+50;
	rect.y = DRV1STARTY+4;
	rect.w = 6;
	rect.h = 4;
	

	if (_pwr1)
	{
	    SDL_SetRenderDrawColor(Main_Renderer, 255, 00, 00, 255);			
	}
	else
	{
		SDL_SetRenderDrawColor(Main_Renderer, 70, 00, 00, 255);	
	}
	SDL_RenderFillRect(Main_Renderer, &rect);

	// DRIVE2 POWER LED  
	
	rect.x = DRV2STARTX+50;
	rect.y = DRV2STARTY+4;
	rect.w = 6;
	rect.h = 4;
	  
	if (_pwr2)
	{
	    SDL_SetRenderDrawColor(Main_Renderer, 255, 00, 00, 255);			
	}
	else
	{
		SDL_SetRenderDrawColor(Main_Renderer, 70, 00, 00, 255);	
	}
	SDL_RenderFillRect(Main_Renderer, &rect);
	
	// HRAM POWER LED    
	
	rect.x = HRAMSTARTX+6;
	rect.y = HRAMSTARTY+4;
	rect.w = 6;
	rect.h = 4;
	
	if (mem->getHRAMRD())
	{
	    SDL_SetRenderDrawColor(Main_Renderer, 00, 255, 00, 255);			
	}
	else
	{
		SDL_SetRenderDrawColor(Main_Renderer, 00, 70, 00, 255);	
	}
	SDL_RenderFillRect(Main_Renderer, &rect);

	rect.x = HRAMSTARTX+19;
	rect.y = HRAMSTARTY+4;
	rect.w = 6;
	rect.h = 4;
	
	if (mem->getHRAMWRT() == 0)
	{
	    SDL_SetRenderDrawColor(Main_Renderer, 00, 255, 00, 255);			
	}
	else
	{
		SDL_SetRenderDrawColor(Main_Renderer, 00, 70, 00, 255);	
	}
	SDL_RenderFillRect(Main_Renderer, &rect);
	
	rect.x = HRAMSTARTX+32;
	rect.y = HRAMSTARTY+4;
	rect.w = 6;
	rect.h = 4;
	
	if (mem->getBANK1() != 0) 
	{
	    SDL_SetRenderDrawColor(Main_Renderer, 255, 00, 00, 255);			
	}
	else
	{
		SDL_SetRenderDrawColor(Main_Renderer, 70, 00, 00, 255);	
	}
	SDL_RenderFillRect(Main_Renderer, &rect);

	rect.x = HRAMSTARTX+45;
	rect.y = HRAMSTARTY+4;
	rect.w = 6;
	rect.h = 4;
	
	if (mem->getBANK1() == 0) 
	{
	    SDL_SetRenderDrawColor(Main_Renderer, 255, 00, 00, 255);			
	}
	else
	{
		SDL_SetRenderDrawColor(Main_Renderer, 70, 00, 00, 255);	
	}
	SDL_RenderFillRect(Main_Renderer, &rect);
		
	SDL_UpdateWindowSurface(window);
}




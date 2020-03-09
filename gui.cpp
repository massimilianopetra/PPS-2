#include "video.h"
#include "gui.h"
#include "system_io.h"
#include "memory.h"


#define DRV1STARTX  10
#define DRV2STARTX 100
#define DRV1STARTY  (SCREEN_HEIGHT+25)
#define DRV2STARTY  (SCREEN_HEIGHT+25)
#define HRAMSTARTX 180
#define HRAMSTARTY  (SCREEN_HEIGHT+25)

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
	rect.x = HRAMSTARTX;
	rect.y = HRAMSTARTY;
	rect.w = 34;
	rect.h = 15;
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
	
	rect.x = HRAMSTARTX+4;
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

	rect.x = HRAMSTARTX+14;
	rect.y = HRAMSTARTY+4;
	rect.w = 6;
	rect.h = 4;
	
	if (mem->getHRAMRD() != 0 && mem->getBANK1() != 0) 
	{
	    SDL_SetRenderDrawColor(Main_Renderer, 255, 00, 00, 255);			
	}
	else
	{
		SDL_SetRenderDrawColor(Main_Renderer, 70, 00, 00, 255);	
	}
	SDL_RenderFillRect(Main_Renderer, &rect);

	rect.x = HRAMSTARTX+24;
	rect.y = HRAMSTARTY+4;
	rect.w = 6;
	rect.h = 4;
	
	if (mem->getHRAMRD() != 0 && mem->getBANK1() == 0) 
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


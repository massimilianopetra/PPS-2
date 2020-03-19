/* **********************************************************
   *
   * video.cpp  PPS-1 main module
   *   Author Massimiliano Petra,  january 2020
   *
   **********************************************************/
   
#include <stdio.h>
#include "video.h"
#include "gui.h"
#include "rom.h"

// Gui debug
extern uint8_t guidebug;

//The window we'll be rendering to
SDL_Window* window = NULL;

//The renderer (used by DSL2 drawing API)
SDL_Renderer* Main_Renderer;

//The surface contained by the window0
SDL_Surface* screenSurface = NULL;

//Video pixels
uint32_t *pixels = NULL;

// Screen switches
uint8_t screen_switches = 0;

uint32_t color = 0x00ffffff;

uint16_t page_row_memory_map[] = { 
	0x000, 0x080, 0x100, 0x180, 0x200, 0x280, 0x300,0x380,
	0x028, 0x0A8, 0x128, 0x1A8, 0x228, 0x2A8, 0x328,0x3A8,
	0x050, 0x0D0, 0x150, 0x1D0, 0x250, 0x2D0, 0x350,0x3D0
};


uint32_t palette[] = {
	0x00000000,0x0099035F,0X004204E1,0X00CA13FE,
	0X00007310,0X007F7F7F,0X002497FF,0X00AAA2FF,	
	0X004F5101,0X00F05C00,0X00BEBEBE,0X00FF85FF,
	0X0012CA07,0X00CED413,0X0051F595,0x00FFFFFF};
		
uint32_t hpalette0[] = {
	0X00CA13FE,0X002497FF};
uint32_t hpalette1[] = {	
	0X0012CA07,0X00F05C00};


void clearscreen()
{
    // Clear screen
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00));
    SDL_UpdateWindowSurface(window);	
}

uint16_t flash_count = 0;

void video_refresh(uint8_t *RAM)
{
	uint8_t c_val;
	uint8_t p,p_next;
	uint8_t char_byte;
	uint32_t v;
	int r,c;
	int text_r,text_c;
	int gr_r,gr_c;
	int mode;
	uint16_t base_address=0;
	uint16_t base_address_text=0;
	uint8_t hcolor;
	
	
	// Flash count
	flash_count = (flash_count +1) &0x3f;
	
	if ((screen_switches & _TEXT_GRAPHICS) == 0)
	{
		// TEXT MODE
		
		if ((screen_switches & _PAGE1_PAGE2) == 0)
			base_address = _LO_RES1;
		else
			base_address = _LO_RES2;
			
		for (r=0;r<SCREEN_HEIGHT/2;r++)
		{
			text_r = r / 8;
			
			for (c=0;c<screenSurface->w/2;c++)
			{
				text_c = c / 7;  
		
				if ((c % 7) == 0)
				{
					c_val = RAM[base_address+page_row_memory_map[text_r]+text_c];
					mode = c_val >> 6;
					
					// reduce to 5x7 matrix
					//c_val = c_val & 0x3F;
					
					char_byte = _CHARROM[c_val*8+(r&0x07)];

					if (REVERSE_CHAR_ROM)
						char_byte = char_byte >> 1;
					else
						char_byte = char_byte << 1;
					
					// Inverse
					if (mode == 0)
						char_byte = char_byte ^ 0xFF;
						
					// Flashing
					if ((mode == 1) && (flash_count >= 0x1f))
						char_byte = char_byte ^ 0xFF;
				}
				
				if (REVERSE_CHAR_ROM)
				{
					if (char_byte & 0x01)
						v = color;
					else 
						v = 0;
				}
				else
				{
					if (char_byte & 0x80)
						v = color;
					else 
						v = 0;
				}
				
				
				//printf("%02X %x \n",char_byte,v);
				
				// Quadruple pixel effect
				pixels[screenSurface->w*(2*r)+c*2]=v;
				pixels[screenSurface->w*(2*r)+c*2+1]=v;
				//pixels[screenSurface->w*(2*r+1)+c*2]=v;
				//pixels[screenSurface->w*(2*r+1)+c*2+1]=v;
				
				if (REVERSE_CHAR_ROM)
					char_byte = char_byte >> 1;
				else
					char_byte = char_byte << 1;
			}
		} 
	} // TEXT MODE
	else if (((screen_switches & _TEXT_GRAPHICS) == _TEXT_GRAPHICS) && ((screen_switches & _LORES_HIRES) == 0))
	{
		// LORES MODE
		if ((screen_switches & _PAGE1_PAGE2) == 0)
			base_address = _LO_RES1;
		else
			base_address = _LO_RES2;
		
		for (r=0;r<SCREEN_HEIGHT/2;r++)
		{
			text_r = r / 8;
			gr_r = r / 4;
			
			for (c=0;c<screenSurface->w/2;c++)
			{
				if (((screen_switches & _ALL_MIX) == _ALL_MIX) && (text_r > 19))
				{					
					// MIX MODE
					text_c = c / 7;
			
					if ((c % 7) == 0)
					{
						c_val = RAM[base_address+page_row_memory_map[text_r]+text_c];
						mode = c_val >> 6;
						
						// reduce to 5x7 matrix
						//c_val = c_val & 0x3F;
						
						char_byte = _CHARROM[c_val*8+(r&0x07)];

						if (REVERSE_CHAR_ROM)
							char_byte = char_byte >> 1;
						else
							char_byte = char_byte << 1;
												
						// Inverse
						if (mode == 0)
							char_byte = char_byte ^ 0xFF;
							
						// Flashing
						if ((mode == 1) && (flash_count >= 0x1f))
							char_byte = char_byte ^ 0xFF;
					}
					
					if (REVERSE_CHAR_ROM)
					{
						if (char_byte & 0x01)
							v = color;
						else 
							v = 0;
					}
					else
					{
						if (char_byte & 0x80)
							v = color;
						else 
							v = 0;
					}
				} // // MIX MODE
				else
				{
					// GRAPHIC SECTION
					gr_c = c / 7;
					c_val = RAM[base_address+page_row_memory_map[text_r]+gr_c];
					
					if ((gr_r & 0x01) == 1)
						v = palette[(c_val & 0xF0)>>4];
					else
						v = palette[(c_val & 0x0F)];
				}
				
				
				//printf("%02X %x \n",char_byte,v);
				
				// Quadruple pixel effect
				pixels[screenSurface->w*(2*r)+c*2]=v;
				pixels[screenSurface->w*(2*r)+c*2+1]=v;
				//pixels[screenSurface->w*(2*r+1)+c*2]=v;
				//pixels[screenSurface->w*(2*r+1)+c*2+1]=v;
				
				if (REVERSE_CHAR_ROM)
					char_byte = char_byte >> 1;
				else
					char_byte = char_byte << 1;
			}
		}
	} // LORES MODE
	else if (((screen_switches & _TEXT_GRAPHICS) == _TEXT_GRAPHICS) && ((screen_switches & _LORES_HIRES) == _LORES_HIRES))
	{
		// HIRES MODE
		if ((screen_switches & _PAGE1_PAGE2) == 0)
		{
			base_address = _HI_RES1;
			base_address_text = _LO_RES1;
		}
		else
		{
			base_address = _HI_RES2;
			base_address_text = _LO_RES2;
		}
			
		
		for (r=0;r<SCREEN_HEIGHT/2;r++)
		{
			text_r = r / 8;
			gr_r = r % 8;
			
			for (c=0;c<screenSurface->w/2;c++)
			{

				if (((screen_switches & _ALL_MIX) == _ALL_MIX) && (text_r > 19))
				{					
					// MIX MODE
					text_c = c / 7;
			
					if ((c % 7) == 0)
					{
						c_val = RAM[base_address_text+page_row_memory_map[text_r]+text_c];
						mode = c_val >> 6;
						
						// reduce to 5x7 matrix
						//c_val = c_val & 0x3F;
						
						char_byte = _CHARROM[c_val*8+(r&0x07)];

						if (REVERSE_CHAR_ROM)
							char_byte = char_byte >> 1;
						else
							char_byte = char_byte << 1;
												
						// Inverse
						if (mode == 0)
							char_byte = char_byte ^ 0xFF;
							
						// Flashing
						if ((mode == 1) && (flash_count >= 0x1f))
							char_byte = char_byte ^ 0xFF;
					}
					
					if (REVERSE_CHAR_ROM)
					{
						if (char_byte & 0x01)
							v = color;
						else 
							v = 0;
					}
					else
					{
						if (char_byte & 0x80)
							v = color;
						else 
							v = 0;
					}
				} // MIX MODE
				else
				{
					// GRAPHIC SECTION
					gr_c = c / 7;
					
					if ((c % 7) == 0)
					{
						c_val = RAM[base_address+page_row_memory_map[text_r]+gr_r*1024+gr_c];
						hcolor = (c_val & 0x80) >> 7;
					}
					
					
					p = c_val & 0x01;
					if ((c % 7) <= 5)
					{
						p_next = c_val & 0x02;
					} 
					else
					{
						p_next = RAM[base_address+page_row_memory_map[text_r]+gr_r*1024+gr_c+1] & 0x01;
					}
					
					
					if (p == 0)
					{
						v = 0x00000000;
					}
					else
					{
						if (p_next)
						{
							v = 0x00ffffff;
						}
						else
						{
							if (c%2)
							{
								v = hpalette1[hcolor]; 
							}	
							else
							{
								v = hpalette0[hcolor]; 
							}
						}
					}	
										
					c_val = c_val >> 1;
				}
				
				// Quadruple pixel effect
				pixels[screenSurface->w*(2*r)+c*2]=v;
				pixels[screenSurface->w*(2*r)+c*2+1]=v;
				
				if (REVERSE_CHAR_ROM)
					char_byte = char_byte >> 1;
				else
					char_byte = char_byte << 1;
			}
		}
	} // HIRES MODE
	SDL_UpdateWindowSurface(window);
}


void video_init()
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
    	
        //Create window
#ifdef WITHGUI
		if(guidebug)
        	window = SDL_CreateWindow( "PPS-2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT_DBG, SDL_WINDOW_SHOWN );
        else
        	window = SDL_CreateWindow( "PPS-2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        	
#else
        window = SDL_CreateWindow( "PPS-2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
#endif
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else
        {
            // Get window surface
            screenSurface = SDL_GetWindowSurface(window);
            Main_Renderer =	SDL_CreateSoftwareRenderer(screenSurface);
        }
    }
    		
    		
    pixels = (uint32_t *)screenSurface->pixels;
    clearscreen();     

#ifdef WITHGUI
	if(guidebug)
    	initgui();
#endif

}

void video_test()
{
	SDL_Surface* testSurface = NULL;
	SDL_Window* testwindow = NULL;
	uint32_t *testpixels = NULL;

	uint8_t c_val;
	uint8_t char_byte;
	uint32_t v;
	int i,r,c,cr,cc;
	
    //Create window
    testwindow = SDL_CreateWindow( "PPS-2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( testwindow == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        // Get window surface
        testSurface = SDL_GetWindowSurface( testwindow );
    }
 
	testpixels = (uint32_t *)testSurface->pixels;

    // Clear screen
    SDL_FillRect(testSurface, NULL, SDL_MapRGB( testSurface->format, 0x00, 0x00, 0x00));
    
	 
 	for (i=0;i<256;i++)
	{	
		for (cr=0;cr<8;cr++)
		{
			r = (i & 0x0f) * 8 +cr;
			
			char_byte = _CHARROM[i*8+cr] & 0x7E;
			//printf("%d\n",char_byte);
			
			for (cc=0;cc<8;cc++)
			{
				c = ((i & 0xf0) >> 1) + cc;
				//printf("%d %d %d\n",i,r,c);
				  		
				if (REVERSE_CHAR_ROM)
				{
					if (char_byte & 0x01)
						v = color;
					else 
						v = 0;
				}
				else
				{
					if (char_byte & 0x80)
						v = color;
					else 
						v = 0;
				}
				
				
				//printf("%02X %x \n",char_byte,v);
				
				//Quadruple pixel effect
				testpixels[testSurface->w*(2*r)+c*2]=v;
				testpixels[testSurface->w*(2*r)+c*2+1]=v;
				//testpixels[testwindow->w*(2*r+1)+c*2]=v;
				//testpixels[testwindow->w*(2*r+1)+c*2+1]=v;
				
				if (REVERSE_CHAR_ROM)
					char_byte = char_byte >> 1;
				else
					char_byte = char_byte << 1;
			}  
		}
	}
	
	SDL_UpdateWindowSurface(testwindow);
}

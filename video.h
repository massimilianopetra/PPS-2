/* **********************************************************
   *
   * video.h
   *   Author Massimiliano Petra,  january 2020
   *
   **********************************************************/
   
#ifndef __VIDEO_H
#define __VIDEO_H

#include <stdint.h>
#include "SDL2/SDL.h"
#include "gui.h"

// Screen dimension constants
#define SCREEN_WIDTH  560
#define SCREEN_HEIGHT 384



// Text/LoRes Primary 40x24
#define _LO_RES1	0x0400
// Text/LoRes Secondary 40x24
#define _LO_RES2	0x0800
// Hi-Res Primary
#define _HI_RES1	0x2000
// Hi-Res Secondary
#define _HI_RES2	0x4000

// Display mode
#define _GRPHICS_MODE	0xC050
#define _TEXT_MODE		0xC051

#define _ALL_MODE		0xC052
#define _MIX_MODE		0xC053

#define _PAGE1_MODE		0xC054
#define _PAGE2_MODE		0xC055

#define _LORES_MODE		0xC056
#define _HIRES_MODE		0xC057

// Screen switches ~
#define _TEXT_GRAPHICS  0b00000001
#define _ALL_MIX  		0b00000010
#define _PAGE1_PAGE2	0b00000100
#define _LORES_HIRES	0b00001000

extern uint8_t screen_switches;


#define REVERSE_CHAR_ROM 0

extern SDL_Window* window;
extern SDL_Surface* screenSurface;
extern SDL_Renderer* Main_Renderer;

void video_init();
void video_test();
void clearscreen();
void video_refresh(uint8_t *RAM);

#endif

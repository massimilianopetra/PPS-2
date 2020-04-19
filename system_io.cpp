/*
 * system_io.cpp 
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

#include "SDL2/SDL.h"
#include "system_io.h"
#include "audio.h"


/*    Keyboard scancode mapped on Appley ][ layout like

   Left Arrow  0x40000050
   Right Arrow 0x4000004F
   shift       0x400000E1 || 0x400000E5
   Ctrl        0x400000E0 || 0x400000E4
   Alt         0x400000E6
   AltGr       0x400000E6
   F1          0x4000003A
   F2          0x4000003B
   F3          0x4000003C
   F4          0x4000003D
   F5          0x4000003E
   F6          0x4000003F
   NUM9        0x40000061
   NUM3        0x4000005B
   NUM7        0x4000005F
   NUM1        0x40000059

 ***********************************************************************************************************
       0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
                                                                                     RET        		0
                                                                         ESC                    		1
                                                 :                               ,   /     .      		2
       0     1     2     3     4     5     6     7     8     9                                    	    3
                                                                                                		4
                                                                                                		5
             A     B     C     D     E     F     G     H     I     J     K     L     M     N     O	    6
       P     Q     R     S     T     U     V     W     X     Y     Z                               		7
                                                                                                		8
                                                                                                		9
                                                                                                		A
                                                                                                		B
                                                                                                		C
                                                                                                	    D
                                                                               -                		E
                   ;                                                                            		F
************************************************************************************************************/


uint8_t keymap [] = {
//     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x00, 0x00,		// 0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9B, 0x00, 0x00, 0x00, 0x00,		// 1
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBA, 0x00, 0x00, 0x00, 0x2b, 0xAC, 0xAF, 0xAE, 0x00,     // 2
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,     // 3
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 4
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 5
	0x00, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,     // 6
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,     // 7
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 8
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 9
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // A
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // B
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // C
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // D
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAD, 0x00, 0x00, 0x00,     // E
	0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00      // F
};

uint8_t keymap_shift [] = {
//     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x00, 0x00,		// 0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9B, 0x00, 0x00, 0x00, 0x00,		// 1
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x2b, 0xBc, 0xBF, 0xBE, 0x00,     // 2
	0xB0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,     // 3
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 4
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 5
	0x00, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xDE, 0xCF,     // 6
	0xC0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,     // 7
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 8
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 9
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // A
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // B
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // C
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // D
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBD, 0x00, 0x00, 0x00,     // E
	0x00, 0x00, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00      // F
};

uint8_t keymap_ctrl [] = {
//     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x00, 0x00,		// 0
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9B, 0x00, 0x00, 0x00, 0x00,		// 1
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBA, 0x00, 0x00, 0x00, 0x2b, 0xAC, 0xAF, 0xAE, 0x00,     // 2
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,     // 3
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 4
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 5
	0x00, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,     // 6
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,     // 7
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 8
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // 9
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // A
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // B
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // C
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // D
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAD, 0x00, 0x00, 0x00,     // E
	0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00      // F
};

uint8_t mask_on[] = {
	0b00000001,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00100000,
	0b01000000,
	0b10000000
};

uint8_t mask_off[] = {
	0b11111110,
	0b11111101,
	0b11111011,
	0b11110111,
	0b11101111,
	0b11011111,
	0b10111111,
	0b01111111
};


system_io *IO = NULL;

system_io::system_io()
{
	_RAM = mem->getRAM();
	keyboard_init();
	paddle_init();
	dsk = new disk();
	showkbdcode = 0;
}

void system_io::paddle_init()
{	
	pdl0 = _PDL_MIN_uS;
	pdl1 = _PDL_MIN_uS;
	pdl2 = _PDL_MIN_uS;
	pdl3 = _PDL_MIN_uS;
	pdl0_timer = 0;
	pdl1_timer = 0;
	pdl2_timer = 0;
	pdl3_timer = 0;
}

int system_io::devicectrl(uint16_t address, uint8_t rw,uint8_t data)
{
	uint8_t slot;
	uint8_t n;
	uint8_t on;
	uint8_t value;
	
	slot = ((address & 0x00F0) >> 4) - 8;
	n = (address & 0x000F) >> 1;
	on = address & 0x0001;	
	
	// Check if slot refers to disk controller
	if (slot == dsk->getslot())
	{
		value = dsk->decoder(n,on,rw,data);
	}
	
	return value;
}

void system_io::Reset()
{
	int slot;
	
	for(slot=1;slot<8;slot++)
	{
		// Check if slot refers to disk controller
		if (slot == dsk->getslot())
		{
			dsk->reset();
		}			
	}	
}

void system_io::diskstep(uint8_t cycles)
{
	dsk->step(cycles);
}

void system_io::diskprint()
{
	dsk->print();
}

void system_io::diskprintstat()
{
	dsk->printstat();
}

void system_io::paddle_trigger()
{
	pdl0_timer = pdl0;
	pdl1_timer = pdl1;
	pdl2_timer = pdl2;
	pdl3_timer = pdl3;
}

void system_io::timer(uint64_t cycles)
{
	if (pdl0_timer > cycles)
		pdl0_timer -= cycles;
	else
		pdl0_timer = 0;

	if (pdl1_timer > cycles)
		pdl1_timer -= cycles;
	else
		pdl1_timer = 0;

	if (pdl2_timer > cycles)
		pdl2_timer -= cycles;
	else
		pdl2_timer = 0;
		
	if (pdl3_timer > cycles)
		pdl3_timer -= cycles;
	else
		pdl3_timer = 0;
}

uint8_t system_io::readPDL0()
{
	if (pdl0_timer > 0)
		return 0xff;
	else
		return 0x00;
}

uint8_t system_io::readPDL1()
{
	if (pdl1_timer > 0)
		return 0xff;
	else
		return 0x00;
}

uint8_t system_io::readPDL2()
{
	if (pdl2_timer > 0)
		return 0xff;
	else
		return 0x00;
}

uint8_t system_io::readPDL3()
{
	if (pdl3_timer > 0)
		return 0xff;
	else
		return 0x00;
}

void system_io::keyboard_init()
{
	shift = 0;
	altgr = 0;
	ctrl = 0;
	pdl0_inc = 0;
	pdl1_inc = 0;
	pdl2_inc = 0;
	pdl3_inc = 0;
	pb0 = 0;
	pb1 = 0;
	pb2 = 0;
}

uint8_t system_io::readPB0()
{
	return pb0;
}

uint8_t system_io::readPB1()
{
	return pb1;
}

uint8_t system_io::readPB2()
{
	return pb2;
}

void system_io::resetPB0()
{
	pb0 = 0;
}

void system_io::resetPB1()
{
	pb1 = 0;	
}

void system_io::resetPB2()
{
	pb2 = 0;	
}

void system_io::setPB0()
{
	pb0 = 0x80;
}

void system_io::setPB1()
{
	pb1 = 0x80;	
}

void system_io::setPB2()
{
	pb2 = 0x80;	
}
		
int system_io::keyboard()
{
	// Handle paddle
	pdl0 += pdl0_inc;
	if (pdl0 > _PDL_MAX_uS)
		pdl0 = _PDL_MAX_uS;
	if (pdl0 < _PDL_MIN_uS)
		pdl0 = _PDL_MIN_uS;
	pdl1 += pdl1_inc;
	if (pdl1 > _PDL_MAX_uS)
		pdl1 = _PDL_MAX_uS;
	if (pdl1 < _PDL_MIN_uS)
		pdl1 = _PDL_MIN_uS;
	pdl2 += pdl2_inc;
	if (pdl2 > _PDL_MAX_uS)
		pdl2 = _PDL_MAX_uS;
	if (pdl2 < _PDL_MIN_uS)
		pdl2 = _PDL_MIN_uS;
	pdl3 += pdl3_inc;
	if (pdl3 > _PDL_MAX_uS)
		pdl3 = _PDL_MAX_uS;
	if (pdl3 < _PDL_MIN_uS)
		pdl3 = _PDL_MIN_uS;

	// Get the next event
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
		{
			int tmp = event.key.keysym.sym;
			if (showkbdcode)
				printf("DBD: (P) %02x\n", tmp);
			
			if ((tmp == 0x400000e1) || (tmp == 0x400000e5))
			{	
				// Shift pressed
				shift = 1;
				setPB2();
				
			}
			else if ((tmp == 0x400000e6))
			{	
				// Altgr
				altgr = 1;
			}
			else if ((tmp == 0x400000e0) || (tmp == 0x400000e4))
			{
				ctrl = 1;
			}
			else if (tmp == 0x40000050)
			{
				// Left Arrow
				_RAM[_KBD] = 0x88;
				return 0;	
			}
			else if (tmp == 0x4000004F)
			{
				// Right Arrow
				_RAM[_KBD] = 0x95;
				return 0;	
			}
			else if (tmp == 0x40000061)
			{
				// NUM9
				pdl1_inc = -_PDL_INC;
				return 0;
			}		
			else if (tmp == 0x4000005F)
			{
				// NUM7
				pdl0_inc= -_PDL_INC;
				return 0;
			}		
			else if (tmp == 0x4000005B)
			{
				// NUM3
				pdl1_inc = _PDL_INC;
				return 0;
			}		
			else if (tmp == 0x40000059)
			{
				// NUM1
				pdl0_inc = _PDL_INC;
				return 0;
			}
			else if (tmp == 0x4000005c)
			{
				// NUM4
				setPB0();
				return 0;
			}	
			else if (tmp == 0x4000005e)
			{
				// NUM6
				setPB1();
				return 0;
			}
			else if (tmp == 0x4000003A)
			{
				// F1
				return 11;
			}
			else if (tmp == 0x4000003B)
			{
				// F2
				return 12;
			}	
			else if (tmp == 0x4000003C)
			{
				// F3
				showkbdcode ^= 1;
				printf("=== TOGGLE SHOW KBD CODE ===\n");
				return 0;
			}	
			else
			{
				if (altgr)
				{
					if ((tmp & 0xFF) == 0x72)
					{
						// Reset ALTGR+R
						return 1;
					}
					else if ((tmp & 0xFF) == 0x6c)
					{
						// Load ALTGR+L
						return 2;
					}
					else if ((tmp & 0xFF) == 0x68)
					{
						// Clear Screen ALTGR+H
						return 3;
					}
					else if ((tmp & 0xFF) == 0x76)
					{
						// Paste Screen ALTGR+V
						return 5;
					}
					else if ((tmp & 0xFF) == 0x64)
					{
						// Toggle debug mode ALTGR+D
						return 6;
					}
					else if ((tmp & 0xFF) == 0x74)
					{
						// Trace ALTGR+T
						return 7;
					}	
					if ((tmp & 0xFF) == 0x70)
					{
						// Shell Prompt ALTGR+P
						return 8;
					}
					else if ((tmp & 0xFF) == 0x66)
					{
						// Full Screen ALTGR+F
						return 9;
					}				
				}
				else if (ctrl)
				{
					_RAM[_KBD] = keymap_ctrl[tmp & 0xFF]|0x80;
				}
				else if (shift)
					_RAM[_KBD] = keymap_shift[tmp & 0xFF]|0x80;
				else
					_RAM[_KBD] = keymap[tmp & 0xFF]|0x80;
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			int tmp = event.key.keysym.sym;
			if (showkbdcode)
				printf("DBD: (R) %02x\n", tmp);
			
			if ((tmp == 0x400000e1) || (tmp == 0x400000e5))
			{	
				// Shift release
				shift = 0;
				resetPB2();
			}
			else if ((tmp == 0x400000e6))
			{	
				// Altgr
				altgr = 0;
			}
			else if ((tmp == 0x400000e0) || (tmp == 0x400000e4))
			{
				ctrl = 0;
			}
			else if (tmp == 0x40000061)
			{
				// NUM9
				pdl1_inc = 0;
				return 0;
			}		
			else if (tmp == 0x4000005F)
			{
				// NUM7
				pdl0_inc= 0;
				return 0;
			}		
			else if (tmp == 0x4000005B)
			{
				// NUM3
				pdl1_inc = 0;
				return 0;
			}		
			else if (tmp == 0x40000059)
			{
				// NUM1
				pdl0_inc = 0;
				return 0;
			}
			else if (tmp == 0x4000005c)
			{
				// NUM4
				resetPB0();
				return 0;
			}	
			else if (tmp == 0x4000005e)
			{
				// NUM6
				resetPB1();
				return 0;
			}

		}
		else if (event.type == SDL_QUIT)
		{
			return 4;
		}
			
	}
	
	return 0;
}

void system_io::diskslot(uint8_t i)
{
	dsk->init(i);
}

int system_io::diskmount(char *filename, int drvnum)
{
	int i;

	i = dsk->diskmount(filename,drvnum);
			
	return i;
}

void system_io::disksavenib(char *filename, int drvnum)
{
	dsk->savenib(filename,drvnum);
}

void system_io::disksavenib()
{
	dsk->savenib();
}

void system_io::disksavedsk()
{
	dsk->savedsk();
}

void system_io::sound()
{
	AUDIO->toggle();
}

void system_io::diskdebug(uint8_t *_activedrv,uint8_t *_pwr1, int8_t *_halftrk1, uint16_t *_ptr1,uint8_t *_pwr2, int8_t *_halftrk2, uint16_t *_ptr2)
{
	dsk->debug(_activedrv,_pwr1,_halftrk1,_ptr1,_pwr2,_halftrk2,_ptr2);
}



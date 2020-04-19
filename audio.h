/*
 * audio.h 
 *
 * Author: Massimiliano Petra <massimiliano.petra@gmail.com> April, 2020
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
   
   
#ifndef __AUDIO_H
#define __AUDIO_H

#include <stdint.h>
#include "SDL2/SDL.h"

#define __AUDIO_SAMPLINGRATE 40000
#define __AUDIO_FRAMESAMPLES 0x10000
#define __AUDIO_LOW 64
#define __AUDIO_HIGH 192


class audio
{
	private:
		SDL_AudioSpec AudioSettings;
		SDL_AudioDeviceID devid;
		uint64_t last_tick;
		uint8_t buffer[__AUDIO_FRAMESAMPLES+10];
		uint8_t value;
		uint8_t status;
		uint16_t ptr;
		uint8_t cnt;
		
	public:
		audio();
		void callback(void *UserData, uint8_t *AudioData, int Length);
		void on();
		void off();
		void refresh();
		void toggle();
	
};

extern audio *AUDIO;

#endif

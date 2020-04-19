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
 
#include <stdio.h>
#include "audio.h"
#include "cpu_6502.h"

// External reference to cpu elapsed ticks
extern cpu_6502 *cpu;

audio *AUDIO = NULL;

void SDLAudioCallback(void *UserData, uint8_t *AudioData, int Length)
{
	AUDIO->callback(UserData,AudioData,Length);
}

audio::audio()
{
	int i;
	
	AudioSettings.freq = __AUDIO_SAMPLINGRATE;
	AudioSettings.format = AUDIO_U8;
	AudioSettings.channels = 1;
	AudioSettings.samples = __AUDIO_FRAMESAMPLES;
	AudioSettings.callback = NULL;	

	last_tick = 0;	
	ptr = 0;
	value = __AUDIO_LOW;
	memset(buffer,value,__AUDIO_FRAMESAMPLES);
	
	devid = SDL_OpenAudio(&AudioSettings, 0);
	off();
	
	/*
	for (i=0;i<0x10000;i++)
	{
		if (i & 0x100)
			buffer[i] = __AUDIO_LOW;
		else
			buffer[i] = __AUDIO_HIGH;
	}
	*/
}

void audio::on()
{
	status = 1;	
	SDL_PauseAudio(0);
}

void audio::off()
{
	status = 0;
	SDL_PauseAudio(1);
}

void audio::callback(void *UserData, uint8_t *AudioData, int Length)
{

}

void audio::refresh()
{
	uint64_t tick = cpu->getTick();
	uint8_t queue = 0;
	int sample;
	int i;
	
	
	sample = ((tick - last_tick)/1000000.0)*__AUDIO_SAMPLINGRATE;
	
	for(i=0;i<sample;i++)
	{
		buffer[ptr++]=value;
		if (ptr == 0x0000)
		{
			if (cnt > 0)
			{
				SDL_QueueAudio(1, &(buffer[0x8000]), __AUDIO_FRAMESAMPLES/2);
				cnt--;
			}
			
		}
			
		if (ptr == 0x8000)
		{
			if (cnt > 0)
			{
				SDL_QueueAudio(1, &(buffer[0x0000]), __AUDIO_FRAMESAMPLES/2);
				cnt--;
			}
		}
			
	}
	
	last_tick = tick;
}

void audio::toggle()
{	
	cnt = 4;
	
	if (status)
	{
		refresh();
			
		if (value == __AUDIO_LOW)
		{
			value = __AUDIO_HIGH;
		}
		else
		{
			value = __AUDIO_LOW;	
		}
		
		buffer[ptr-1]=value;
	}
}

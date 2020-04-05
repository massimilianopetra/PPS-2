/*
 * nibbilizer.cpp 
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
#include "nibbilizer.h"

uint8_t addr_prologue[] = {0xD5, 0xAA, 0x96};
uint8_t data_prologue[] = {0xD5, 0xAA, 0xAD};
uint8_t epilogue[] = {0xDE, 0xAA, 0xEB};

uint8_t translate_table[] = {
    0X96, 0X97, 0X9A, 0X9B, 0X9D, 0X9E, 0X9F, 0XA6,
    0XA7, 0XAB, 0XAC, 0XAD, 0XAE, 0XAF, 0XB2, 0XB3,
    0XB4, 0XB5, 0XB6, 0XB7, 0XB9, 0XBA, 0XBB, 0XBC,
    0XBD, 0XBE, 0XBF, 0XCB, 0XCD, 0XCE, 0XCF, 0XD3,
    0XD6, 0XD7, 0XD9, 0XDA, 0XDB, 0XDC, 0XDD, 0XDE,
    0XDF, 0XE5, 0XE6, 0XE7, 0XE9, 0XEA, 0XEB, 0XEC,
    0XED, 0XEE, 0XEF, 0XF2, 0XF3, 0XF4, 0XF5, 0XF6,
    0XF7, 0XF9, 0XFA, 0XFB, 0XFC, 0XFD, 0XFE, 0XFF
};

// .dsk .do sectors interleave
uint8_t do_interleave[] =  {0X0, 0X7, 0xE, 0X6, 0xD, 0X5, 0xC, 0X4, 0xB, 0X3, 0xA, 0X2, 0X9, 0X1, 0X8, 0xF};

// .po sectors interleave
uint8_t po_interleave[] =  {0X0, 0X8, 0x1, 0X9, 0x2, 0XA, 0x3, 0XB, 0x4, 0XC, 0x5, 0XD, 0X6, 0XE, 0X7, 0xF};

// uint8_t phys_interleave[] = {0X0, 0xD, 0xB, 0x9, 0x7, 0x5, 0x3, 0x1, 0xE, 0xC, 0xA, 0x8, 0x6, 0x4, 0x2, 0xF};

uint8_t phys_interleave[] = {0X0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

nibbilizer::nibbilizer()
{
	int i;
	uint8_t volume = DEFAULT_VOLUME;
	
    for (i = 0; i < NUM_TRACKS; i++)
    {
        if ((track_buffer[i]  = (uint8_t *) malloc(sizeof(uint8_t)*BYTES_PER_NIB_TRACK)) == NULL)
        {
        	printf("Can't allocate NIB DISK space\n");
        	exit(-1);
		}
	}
	
	// CLEAR NIB SECTOR
	clear();
	
	// SETUP NIB SECTOR
	memcpy(nib_sector.addr.prologue,addr_prologue,3);
    memcpy(nib_sector.addr.epilogue,epilogue,3);
    memcpy(nib_sector.data.prologue,data_prologue,3);
    memcpy(nib_sector.data.epilogue,epilogue,3);
    odd_even_encode(volume,nib_sector.addr.volume);   
    
    memset(nib_sector.gap1,GAP_BYTE,GAP1_LEN);
    memset(nib_sector.gap2,GAP_BYTE,GAP2_LEN);
}

void nibbilizer::clear()
{
	int i;
	
    for (i = 0; i < NUM_TRACKS; i++)
    {
    	memset(track_buffer[i],GAP_BYTE,BYTES_PER_NIB_TRACK);
	}	
}

void nibbilizer::convert(uint8_t *dsk, uint8_t interleave)
{
	uint8_t t,s;
	uint8_t checksum;
	int softsec;
	int physsec;
	
	for (t=0;t<NUM_TRACKS;t++)
	{
		for(s=0;s<NUM_SECTORS;s++)
		{

			// Sectors Interleave
			if (interleave == 1)
				softsec = po_interleave[s];
			else
            	softsec = do_interleave[s];
            	
            physsec = phys_interleave[s];
            
			// Setup address field
            odd_even_encode(t,nib_sector.addr.track);
            odd_even_encode(s,nib_sector.addr.sector);
            checksum = DEFAULT_VOLUME ^ t ^ s;
            odd_even_encode(checksum,nib_sector.addr.checksum);
            
            // Encode data
			nib_sector.data.checksum = six_two_encode(&dsk[((t*NUM_SECTORS)+softsec)*BYTES_PER_SECTOR],nib_sector.data.userdata);
			memcpy(&(track_buffer[t][physsec*BYTES_PER_NIB_SECTOR]),&nib_sector,BYTES_PER_NIB_SECTOR);
		}
	}
}

uint8_t* nibbilizer::get_track(uint8_t t)
{
	return track_buffer[t];
}

int nibbilizer::save(char* filename)
{
	FILE *fp;
	int i;
	
	fp=fopen(filename,"wb");
	if (fp == NULL)
		return -1;

	for (i=0;i<NUM_TRACKS;i++)
	{
		fwrite(track_buffer[i],1,BYTES_PER_NIB_TRACK,fp);
	}
	
	fclose(fp);
	
	return 0;
}

void nibbilizer::odd_even_encode(uint8_t inbyte, uint8_t outbyte[])
{
    outbyte[0]  = (inbyte >> 1) | 0xAA;
    outbyte[1]  = inbyte | 0xAA;
}

uint8_t nibbilizer::six_two_translate(uint8_t inbyte)
{
	return translate_table[inbyte & 0x3F];
}

uint8_t nibbilizer::six_two_encode(uint8_t inbyte[],uint8_t outbyte[])
{
	int i,j;
	
	// BUFFER ERASE
	memset(primary_buf,0,PRIMARY_BUF_LEN);
    memset(secondary_buf,0,SECONDARY_BUF_LEN);
    
	// BUFFERING
	for (i=0;i<PRIMARY_BUF_LEN;i++)
	{
		primary_buf[i] = (inbyte[i] >> 2);
        secondary_buf[i % SECONDARY_BUF_LEN] |= (((inbyte[i]&0x02)>>1) | ((inbyte[i]&0x01)<<1)) << ((i / SECONDARY_BUF_LEN)*2);
	}
	
	// XORING AND TRANSLATE
	j = 0;
    outbyte[j++] = six_two_translate(secondary_buf[0] );

    for (i=1;i < SECONDARY_BUF_LEN;i++)
        outbyte[j++] = six_two_translate(secondary_buf[i] ^ secondary_buf[i-1]);

    outbyte[j++] = six_two_translate(primary_buf[0] ^ secondary_buf[SECONDARY_BUF_LEN-1]);

    for (i=1;i < PRIMARY_BUF_LEN;i++)
        outbyte[j++] = six_two_translate(primary_buf[i] ^ primary_buf[i-1]) ;
	
	// RETURN CHECKSUM
	return six_two_translate(primary_buf[PRIMARY_BUF_LEN-1]);
}



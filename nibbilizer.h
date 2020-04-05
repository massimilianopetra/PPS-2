/*
 * nibbilizer.h 
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
   
#ifndef __NIBBILIZER_H
#define __NIBBILIZER_H

#include <stdint.h>

#define DISK_SIZE 143360L
#define NUM_TRACKS 35
#define NUM_SECTORS 16
#define BYTES_PER_SECTOR 256
#define PRIMARY_BUF_LEN 256
#define SECONDARY_BUF_LEN 86
#define DATA_LEN (PRIMARY_BUF_LEN+SECONDARY_BUF_LEN)
#define BYTES_PER_NIB_SECTOR 416
#define BYTES_PER_NIB_TRACK 6656
#define DEFAULT_VOLUME 254
#define GAP1_LEN  48
#define GAP2_LEN  5
#define GAP_BYTE 0xff

typedef struct 
{
    uint8_t prologue[3];
    uint8_t volume[2];
    uint8_t track[2];
    uint8_t sector[2];
    uint8_t checksum[2];
    uint8_t epilogue[3];
} address_field_t;

typedef struct 
{
    uint8_t prologue[3];
    uint8_t userdata[DATA_LEN];
    uint8_t checksum;
    uint8_t epilogue[3];
} data_field_t;

typedef struct {
    uint8_t gap1[GAP1_LEN];
    address_field_t addr;
    uint8_t gap2[GAP2_LEN];
    data_field_t data;
} nib_sector_t;

class nibbilizer
{
	private:
		uint8_t primary_buf[PRIMARY_BUF_LEN];
		uint8_t secondary_buf[SECONDARY_BUF_LEN];
		nib_sector_t nib_sector;
		uint8_t *track_buffer[NUM_TRACKS];

		void odd_even_encode(uint8_t inbyte, uint8_t outbyte[]);
		uint8_t six_two_translate(uint8_t inbyte);
		uint8_t six_two_encode(uint8_t inbyte[],uint8_t outbyte[]);
		
	public:
		nibbilizer();
		void convert(uint8_t *dsk, uint8_t interleave);
		int save(char* filename);
		uint8_t *get_track(uint8_t t);
		void clear();
};

#endif

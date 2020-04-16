/*
 * shell.h 
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
   
#ifndef __SHELL_H
#define __SHELL_H

#include <stdint.h>

extern uint16_t __last_address;

int shell_cmd(char *line, uint8_t offline);
void shell_prompt();
uint8_t shell_query(char *prompt);
void load(char *filename,uint8_t *RAM);
void paste(uint8_t *RAM);

#endif


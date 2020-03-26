#ifndef __SHELL_H
#define __SHELL_H

#include <stdint.h>
#include "mos6502.h"


extern uint16_t __last_address;

void shell_prompt(uint8_t *RAM,uint8_t *ROM,mos6502 *cpu);
uint8_t shell_query(char *prompt);
void load(char *filename,uint8_t *RAM,mos6502 *cpu);
void paste(uint8_t *RAM,mos6502 *cpu);

#endif


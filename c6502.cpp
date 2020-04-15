/*
 * c6502.cpp 
 *
 * Author: Massimiliano Petra <massimiliano.petra@gmail.com> April, 2020
 * Author: Salvatore Petra <salvatore.petra2006@gmail.com> April, 2020
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "memory.h"

#define NUM_OPCODE 56
#define NUM_ADDRESSING 13
#define NUM_SYMBTABLE 1024

/* ********************* Private Variables *********************** */

uint16_t __org = 0x1000;
char *SYMBTABLE[NUM_SYMBTABLE];
int SYMVALUE[NUM_SYMBTABLE];

/* *********************   Opcode Tables   *********************** */

char *MNEMONIC[NUM_OPCODE] = {"ADC","AND","ASL","BCC","BCS","BEQ","BIT","BMI","BNE",
                      "BPL","BRK","BVC","BVS","CLC","CLD","CLI","CLV","CMP","CPX","CPY", 
		              "DEC","DEX","DEY","EOR","INC","INX","INY","JMP","JSR","LDA","LDX",
					  "LDY","LSR","NOP","ORA","PHA","PHP","PLA","PLP","ROL","ROR","RTI",
					  "RTS","SBC","SEC","SED","SEI","STA","STX","STY","TAX","TAY","TSX",
					  "TXA","TXS","TYA"};
					  
uint8_t OPCODE[NUM_OPCODE][NUM_ADDRESSING] ={ 
{0xff,0xff,0x69,0x65,0x75,0xff,0x6d,0x7d,0x79,0x61,0x71,0xff,0xff}, // adc
{0xff,0xff,0x29,0x25,0x35,0xff,0x2d,0x3d,0x39,0x21,0x31,0xff,0xff}, // and
{0xff,0x0a,0xff,0x06,0x16,0xff,0x0e,0x1e,0xff,0xff,0xff,0xff,0xff}, // asl
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x90,0xff}, // bcc
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xb0,0xff}, // bcs
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xff}, // beq
{0xff,0xff,0xff,0x24,0xff,0xff,0x2c,0xff,0xff,0xff,0xff,0xff,0xff}, // bit
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x30,0xff}, // bmi
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xd0,0xff}, // bne
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x10,0xff}, // bpl
{0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // brk
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x50,0xff}, // bvc
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x70,0xff}, // bvs
{0x18,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // clc
{0xd8,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // cld
{0x58,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // cli
{0xb8,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // clv
{0xff,0xff,0xc9,0xc5,0xd5,0xff,0xcd,0xdd,0xd9,0xc1,0xd1,0xff,0xff}, // cmp
{0xff,0xff,0xe0,0xe4,0xff,0xff,0xec,0xff,0xff,0xff,0xff,0xff,0xff}, // cpx
{0xff,0xff,0xc0,0xc4,0xff,0xff,0xcc,0xff,0xff,0xff,0xff,0xff,0xff}, // cpy
{0xff,0xff,0xff,0xc6,0xd6,0xff,0xce,0xde,0xff,0xff,0xff,0xff,0xff}, // dec
{0xca,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // dex
{0x88,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // dey
{0xff,0xff,0x49,0x45,0x55,0xff,0x4d,0x5d,0x59,0x41,0x51,0xff,0xff}, // eor
{0xff,0xff,0xff,0xe6,0xf6,0xff,0xee,0xfe,0xff,0xff,0xff,0xff,0xff}, // inc
{0xe8,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // inx
{0xc8,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // iny
{0xff,0xff,0xff,0xff,0xff,0xff,0x4c,0xff,0xff,0xff,0xff,0xff,0x6c}, // jmp
{0xff,0xff,0xff,0xff,0xff,0xff,0x20,0xff,0xff,0xff,0xff,0xff,0xff}, // jsr
{0xff,0xff,0xa9,0xa5,0xb5,0xff,0xad,0xbd,0xb9,0xa1,0xb1,0xff,0xff}, // lda
{0xff,0xff,0xa2,0xa6,0xff,0xb6,0xae,0xff,0xbe,0xff,0xff,0xff,0xff}, // ldx
{0xff,0xff,0xa0,0xa4,0xb4,0xff,0xac,0xbc,0xff,0xff,0xff,0xff,0xff}, // ldy
{0xff,0x4a,0xff,0x46,0x56,0xff,0x4e,0x5e,0xff,0xff,0xff,0xff,0xff}, // lsr
{0xea,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // nop
{0xff,0xff,0x09,0x05,0x15,0xff,0x0d,0x1d,0x19,0x01,0x11,0xff,0xff}, // ora
{0x48,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // pha
{0x08,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // php
{0x68,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // pla
{0x28,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // plp
{0xff,0x2a,0xff,0x26,0x36,0xff,0x2e,0x3e,0xff,0xff,0xff,0xff,0xff}, // rol
{0xff,0x6a,0xff,0x66,0x76,0xff,0x6e,0x7e,0xff,0xff,0xff,0xff,0xff}, // ror
{0x40,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // rti
{0x60,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // rts
{0xff,0xff,0xe9,0xe5,0xf5,0xff,0xed,0xfd,0xf9,0xe1,0xf1,0xff,0xff}, // sbc
{0x38,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // sec
{0xf8,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // sed
{0x78,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // sei
{0xff,0xff,0xff,0x85,0x95,0xff,0x8d,0x9d,0x99,0x81,0x91,0xff,0xff}, // sta
{0xff,0xff,0xff,0x86,0xff,0x96,0x8e,0xff,0xff,0xff,0xff,0xff,0xff}, // stx
{0xff,0xff,0xff,0x84,0x94,0xff,0x8c,0xff,0xff,0xff,0xff,0xff,0xff}, // sty
{0xaa,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // tax
{0xa8,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // tay
{0xba,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // tsx
{0x8a,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // txa
{0x9a,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, // txs
{0x98,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}  // tya
};

uint8_t isalphadigit(char *o)
{
	int res = 1;
	int i;
	
	for(i=0;i<strlen(o) && res == 1;i++)
	{
		if (isalnum(o[i]) == 0) 
			res = 0;
	}
	
	return res;
}

int issymbol(char *o)
{
	int res = -1;
	int i;
	
	for(i=0;i<NUM_SYMBTABLE && res == -1;i++)
	{
		if (SYMBTABLE[i] != NULL &&  (strcmp(o,SYMBTABLE[i]) == 0 ))
			res = i;
	}
	
	return res;
}

void printsymbol()
{
	int i;
	
	for(i=0;i<NUM_SYMBTABLE;i++)
	{
		if (SYMBTABLE[i] != NULL)
		{
			printf("%-16s: $%04X\n",SYMBTABLE[i],SYMVALUE[i]);
		}		
	}
}


int addsymbol(char *o, int value)
{
	int res = -1;
	int i;
	
	for(i=0;i<NUM_SYMBTABLE && res == -1;i++)
	{
		if (SYMBTABLE[i] == NULL)
		{
			SYMBTABLE[i] = (char *)malloc((strlen(o)+1)*sizeof(char));
			if (SYMBTABLE[i] != NULL)
			{
				strcpy(SYMBTABLE[i],o);
				SYMVALUE[i] = value;
				res = i;
			}
			else
			{
				res = -2;
			}
		}
	}
	
	return res;	
}

uint8_t validopcode(char *o)
{
	uint8_t inst = 0xff;
	int i;
	
	for(i=0;i<NUM_OPCODE && inst == 0xff;i++)
	{
		if (strcmp(o,MNEMONIC[i]) == 0)
		{
			inst = i;
		}
	}

	return inst;	
}

int getValue(char *o)
{
	int value;
	int res;
	int i;
	
	if(o[0] == '$')
	{
		// Hexdigit
		res = sscanf(&(o[1]),"%X",&value);
		if (res == 1)
			return value;
	}
	else if (o[0] == '%')
	{
		value = 0;
		res = 1;
		for (i=strlen(o)-1;i>0 && value >= 0;i--)
		{
			if (o[i] == '1')
			{
				value = value + res;
				res = res * 2;
			}
			else if (o[i] == '0')
			{
				res = res * 2;
			}
			else
			{
				value = -1;
			}
		}	
		
		return value;	
	}
	else
	{
		// Symbol
		res = issymbol(o);
		
		if (res >= 0)
			return SYMVALUE[res];
		
		// Decimal digit
		res = sscanf(&(o[0]),"%d",&value);
		if (res == 1)
			return value;
	}
	
	return -1;
}

int dopcode(uint8_t inst, char* operand)
{
	char tmp[256];
	char *ptr;
	int len;
	uint8_t value;
	int data1,data2;
	int relative;

	// Instruction
	if (operand == NULL)
	{
		// Implied addressing
		value = OPCODE[inst][0];
		
		if (value == 0xff)
			return -3;
		else
		{
			mem->writeRAM(__org++,value);
			return 1;
		}
	}
	else
	{
		len = strlen(operand); 
		
		if (strcmp(operand,"A") == 0)
		{
			// Accumulator addressing
			value = OPCODE[inst][1];
			
			if (value == 0xff)
				return -3;
			else
			{
				mem->writeRAM(__org++,value);
				return 1;
			}				
		}
		else if (operand[0] == '#')
		{
			// Immediate
			value = OPCODE[inst][2];
			
			if (value == 0xff)
				return -3;
			else
			{
				data1 = getValue(&(operand[1]));
				
				if (data1 < 0)  
					return -2;
				
				if (data1 >255)
					return -3;	
					
				mem->writeRAM(__org++,value);
				mem->writeRAM(__org++,data1);
				return 1;
			}								
		}
		else if ((len > 2) && ((operand[len-2] == ',') && (operand[len-1] == 'X')))
		{
			// End with ,X

			strcpy(tmp,operand);
			tmp[len-2] = '\0';
			
			if ((data1 = getValue(tmp)) >= 0 )
			{
				if (data1 < 256)
				{
					// Zero  Page, X
					value = OPCODE[inst][4];
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						return 1;
					}
				}
				else if (data1 < 65536)
				{
					// Absolute, X
					value = OPCODE[inst][7];
					
					data2 = (data1 & 0xff00) >> 8;
					data1 = data1 & 0xff;
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						mem->writeRAM(__org++,data2);
						return 1;
					}					
				}
				else
				{
					return -1;
				}						
			}
			else
				return -2;			
		}
		else if ((len > 2) && ((operand[len-2] == ',') && (operand[len-1] == 'Y')))
		{
			// End with ,Y

			strcpy(tmp,operand);
			tmp[len-2] = '\0';
			
			if ((data1 = getValue(tmp)) >= 0 )
			{
				if (data1 < 256)
				{
					// Zero  Page, Y
					value = OPCODE[inst][5];
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						return 1;
					}
				}
				else if (data1 < 65536)
				{
					// Absolute, Y
					value = OPCODE[inst][8];
					
					data2 = (data1 & 0xff00) >> 8;
					data1 = data1 & 0xff;
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						mem->writeRAM(__org++,data2);
						return 1;
					}					
				}
				else
				{
					return -1;
				}						
			}
			else
				return -2;			
		}
		else if ((len > 4) && ((operand[0] == '(') && (operand[len-3] == ',') && (operand[len-2] == 'X' ) && (operand[len-1] == ')')))
		{
			// Match ( -- ,X)

			strcpy(tmp,operand);
			tmp[len-3] = '\0';
			
			if ((data1 = getValue( (&tmp[1]) )) >= 0 )
			{
				if (data1 < 256)
				{
					// Indirect,X
					value = OPCODE[inst][9];
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						return 1;
					}
				}
				else
				{
					return -3;
				}						
			}
			else
				return -2;			
		}
		else if ((len > 4) && ((operand[0] == '(') && (operand[len-3] == ')') && (operand[len-2] == ',' ) && (operand[len-1] == 'Y')))
		{
			// Match ( -- ),Y

			strcpy(tmp,operand);
			tmp[len-3] = '\0';
			
			if ((data1 = getValue( (&tmp[1]) )) >= 0 )
			{
				if (data1 < 256)
				{
					// Indirect,Y
					value = OPCODE[inst][10];
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						return 1;
					}
				}
				else
				{
					return -3;
				}						
			}
			else
				return -2;			
		}
		else if ((len > 2) && ((operand[0] == '(') && (operand[len-1] == ')')))
		{
			//Match ( -- )

			strcpy(tmp,operand);
			tmp[len-1] = '\0';
			
			if ((data1 = getValue( (&tmp[1]) )) >= 0 )
			{
				if (data1 < 65536)
				{
					// Indirect
					value = OPCODE[inst][12];
					
					data2 = (data1 & 0xff00) >> 8;
					data1 = data1 & 0xff;
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						mem->writeRAM(__org++,data2);
						return 1;
					}					
				}
				else
				{
					return -1;
				}						
			}
			else
				return -2;			
		}
		else if ((data1 = getValue(operand)) >= 0 )
		{
			value = OPCODE[inst][11];
			
			if (value != 0xff)
			{
				// Relative
				if (data1 < 65536)
				{
					relative = (data1 - __org)-2;
					
					if (relative > 127)
						return -4;
					if (relative < -128)
						return -4;
					if (relative < 0)
						relative += 256;
						
					mem->writeRAM(__org++,value);
					mem->writeRAM(__org++,relative);
					return 1;
				}
				else
					return -1;
			}
			else
			{
				if (data1 < 256 && inst != 27)  // 27 jmp Zero Page do not exist
				{
					// Zero  Page
					value = OPCODE[inst][3];
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						return 1;
					}
				}
				else if (data1 < 65536)
				{
					// Absolute
					value = OPCODE[inst][6];
					
					data2 = (data1 & 0xff00) >> 8;
					data1 = data1 & 0xff;
					
					if (value == 0xff)
						return -3;
					else
					{	
						mem->writeRAM(__org++,value);
						mem->writeRAM(__org++,data1);
						mem->writeRAM(__org++,data2);
						return 1;
					}					
				}
				else
				{
					return -1;
				}		
			}
		}
		else
			return -2;		
	} 	
}

int assembly(char* line)
{
	int param;
	int i;
	int data1;
	int data2;
	char op1[256];
	char op2[256];
	char op3[256];
	uint8_t inst = 0xff;
	char *ptr;
	
	// Delete comment
	ptr = strchr(line,';');
	if (ptr != NULL)
		ptr[0] = '\0';
	
	param = sscanf(line,"%s %s %s",op1,op2,op3);
		
	// OP1 TO UPPERCASE
	for (i=0;i<strlen(op1);i++)
	{
		op1[i] = (char)toupper((int)op1[i]);
	}
	
	// OP2 TO UPPERCASE
	for (i=0;i<strlen(op2);i++)
	{
		op2[i] = (char)toupper((int)op2[i]);
	}	

	// OP3 TO UPPERCASE
	for (i=0;i<strlen(op3);i++)
	{
		op3[i] = (char)toupper((int)op3[i]);
	}	
	
	// Empty line
	if (param < 0)
		return 0;
	
	// END Statement
	if (strcmp(op1,"END") == 0)
	{
		return -999;
	}
	
	// .BYTE Statement
	if (strcmp(op1,".BYTE") == 0)
	{
		data1 = getValue(op2);
		if (data1 < 0)
		{
			return -2;
		}
		else
		{
			if (data1 < 256)
			{
				mem->writeRAM(__org++,data1);
				return 2;
			}
			else
			{
				return -1;
			}
		}
		return 0;
	}

	// .WORD Statement
	if (strcmp(op1,".WORD") == 0)
	{
		data1 = getValue(op2);
		if (data1 < 0)
		{
			return -2;
		}
		else
		{
			if (data1 < 65536)
			{
				data2 = (data1 & 0xff00) >> 8;
				data1 = data1 & 0xff;
				
	
				mem->writeRAM(__org++,data1);
				mem->writeRAM(__org++,data2);
				return 3;
			}
			else
			{
				return -1;
			}
		}
		return 0;
	}
	
	// ORG Statement
	if (strcmp(op1,"ORG") == 0)
	{
		if (param == 2)
		{
			data1 = getValue(op2);
			
			if (data1 < 0)
				return -2;
			else
				__org = data1;
			
			return 0;
		}
		else
		{
			return -2;
		}
	}	
	
	// OP1 is OPCODE ?
	if ((inst = validopcode(op1)) != 0xff)
	{
		if (param == 1)
			return dopcode(inst,NULL);
		else
			return dopcode(inst,op2);
	}
	else
	{
		if (isalphadigit(op1))
		{

			if (issymbol(op1) != -1)
			{
				return -5;
			}
			else
			{
				// EQU STATEMENT
				if ((param == 3) && (strcmp(op2,"EQU") == 0))
				{

					data1 = getValue(op3);

					if (data1 < 0)
						return -2;
					else
						addsymbol(op1,data1);
					
					return 0;
				}
				else if ((param > 1 ) && ((inst = validopcode(op2)) != 0xff))
				{
					addsymbol(op1,__org);
					
					if (param == 2)
						return dopcode(inst,NULL);
					else if (param == 3)
						return dopcode(inst,op3);
				}
				else
					return -2;
			}
		}		
	}
	
	return -2;
}

uint16_t disassembly(uint16_t address, uint8_t source)
{
	int i,m;
	uint8_t opcode;
	uint8_t operand1;
	uint8_t operand2;
	uint16_t _address = address;
	uint8_t inst = 0xff;
	uint8_t mode = 0xff;
	
	if (source)
		opcode = mem->read(_address++);
	else
		opcode = mem->readRAM(_address++);
	
	if (opcode != 0xff)
	{
		for(i=0;i<NUM_OPCODE && inst == 0xff;i++)
		{
			for(m=0;m<NUM_ADDRESSING && inst == 0xff;m++)
			{
				
				if (opcode == OPCODE[i][m])
				{
					inst = i;
					mode = m;
				}
			}
		}
	}
	
	switch(mode)
	{
		case 0:
			// Implied
			printf("%04X   %02X         %s\n",address,opcode,MNEMONIC[inst]);
			break;
			
		case 1:
			// Accumulator
			printf("%04X   %02X         %s A\n",address,opcode,MNEMONIC[inst]);
			break;
			
		case 2:
			// Immediate
			if (source)
				operand1 = mem->read(_address++);
			else
				operand1 = mem->readRAM(_address++);
			
			printf("%04X   %02X %02X      %s #$%02X\n",address,opcode,operand1,MNEMONIC[inst],operand1);
			break;
			
		case 3:
			// Zero Page
			if (source)
				operand1 = mem->read(_address++);
			else
				operand1 = mem->readRAM(_address++);
			
			printf("%04X   %02X %02X      %s $%02X\n",address,opcode,operand1,MNEMONIC[inst],operand1);
			break;
			
		case 4:
			// Zero Page,X
			if (source)
				operand1 = mem->read(_address++);
			else
				operand1 = mem->readRAM(_address++);
			
			printf("%04X   %02X %02X      %s $%02X,X\n",address,opcode,operand1,MNEMONIC[inst],operand1);
			break;

		case 5:
			// Zero Page,Y
			if (source)
				operand1 = mem->read(_address++);
			else
				operand1 = mem->readRAM(_address++);
			
			printf("%04X   %02X %02X      %s $%02X,Y\n",address,opcode,operand1,MNEMONIC[inst],operand1);
			break;

		case 6:
			// Absolute
			if (source)
			{
				operand1 = mem->read(_address++);
				operand2 = mem->read(_address++);
			}
			else
			{
				operand1 = mem->readRAM(_address++);
				operand2 = mem->readRAM(_address++);
			}
			
			printf("%04X   %02X %02X %02X   %s $%02X%02X\n",address,opcode,operand1,operand2,MNEMONIC[inst],operand2,operand1);
			break;			
			
		case 7:
			// Absolute,X
			if (source)
			{
				operand1 = mem->read(_address++);
				operand2 = mem->read(_address++);
			}
			else
			{
				operand1 = mem->readRAM(_address++);
				operand2 = mem->readRAM(_address++);
			}
			
			printf("%04X   %02X %02X %02X   %s $%02X%02X,X\n",address,opcode,operand1,operand2,MNEMONIC[inst],operand2,operand1);
			break;
			
		case 8:
			// Absolute,Y
			if (source)
			{
				operand1 = mem->read(_address++);
				operand2 = mem->read(_address++);
			}
			else
			{
				operand1 = mem->readRAM(_address++);
				operand2 = mem->readRAM(_address++);
			}
			
			printf("%04X   %02X %02X %02X   %s $%02X%02X,Y\n",address,opcode,operand1,operand2,MNEMONIC[inst],operand2,operand1);
			break;
			
		case 9:
			// Indirect,X  Indexed Indirect
			if (source)
				operand1 = mem->read(_address++);
			else
				operand1 = mem->readRAM(_address++);
			
			printf("%04X   %02X %02X      %s ($%02X,X)\n",address,opcode,operand1,MNEMONIC[inst],operand1);
			break;
			
		case 10:
			// Indirect,Y  Indirect Indexed
			if (source)
				operand1 = mem->read(_address++);
			else
				operand1 = mem->readRAM(_address++);
			
			printf("%04X   %02X %02X      %s ($%02X),Y\n",address,opcode,operand1,MNEMONIC[inst],operand1);
			break;
			
		case 11:
			// Relative
			if (source)
				operand1 = mem->read(_address++);
			else
				operand1 = mem->readRAM(_address++);
			
			printf("%04X   %02X %02X      %s $%04X\n",address,opcode,operand1,MNEMONIC[inst],(_address + ((operand1 < 128) ? operand1 : operand1 - 256)));
			break;		
		
		case 12:
			// Indirect
			if (source)
			{
				operand1 = mem->read(_address++);
				operand2 = mem->read(_address++);
			}
			else
			{
				operand1 = mem->readRAM(_address++);
				operand2 = mem->readRAM(_address++);
			}
			
			printf("%04X   %02X %02X %02X   %s ($%02X%02X)\n",address,opcode,operand1,operand2,MNEMONIC[inst],operand2,operand1);
			break;
						
		default:
			// Illegal instruction
			printf("%04X   %02X         ???\n",address,opcode);	
			break;	
	}
	
	return _address;
}

void shell_assembly()
{
	char line[256];
	int res = 0;
	uint16_t old_org;
	int param;
	int i;
	int data1;
	char op1[256];
	char op2[256];
	
	while(res != -999)
	{
		printf("%04X: ",__org);
		fgets(line,255,stdin);

		param = sscanf(line,"%s %s",op1,op2);
		
		// OP1 TO UPPERCASE
		for (i=0;i<strlen(op1);i++)
		{
			op1[i] = (char)toupper((int)op1[i]);
		}
		
		// OP2 TO UPPERCASE
		for (i=0;i<strlen(op2);i++)
		{
			op2[i] = (char)toupper((int)op2[i]);
		}	
	
	
		
		if ((strcmp(op1,".VALUE") == 0) && (param == 2))
		{
			// .VALUE Statement
			data1 = getValue(op2);
			printf("$%04X   [%d]\n",data1,data1);
			res = 0;
		}
		else if (strcmp(op1,".SYM") == 0)
		{
			printsymbol();
			res = 0;
		}
		else
		{
			old_org = __org;
			res = assembly(line);			
		}
	
		switch(res)
		{
			case 1:
				disassembly(old_org,0);
				break;
			case 2:
				printf("%04X: %02X\n",old_org,mem->readRAM(old_org));
				break;	
			case 3:
				printf("%04X: %02X %02X\n",old_org,mem->readRAM(old_org),mem->readRAM(old_org+1));
				break;	
			case -1:
				printf("=== Value Overflow\n");
				break;
			case -2:
				printf("=== Syntax Error\n");
				break;
			case -3:
				printf("=== Wrong Addressing Mode\n");
				break;
			case -4:
				printf("=== Relative Addressing Too Far\n");
				break;
			case -5:
				printf("=== Duplicate Symbol\n");
				break;
			default:
				break;
		}		
	}
}


/*
 * cpu_6502.cpp 
 *
 * Author: Massimiliano Petra <massimiliano.petra@gmail.com> February, 2020
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
#include "cpu_6502.h"

Instr_t INSTR_TABLE[256] = {
//             00               01               02               03               04               05               06               07               08               09               0A               0B               0C               0D               0E               0F            
          &cpu_6502::__BRK,&cpu_6502::__ORA,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ORA,&cpu_6502::__ASL,&cpu_6502::__ILL,&cpu_6502::__PHP,&cpu_6502::__ORA,&cpu_6502::__ASL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ORA,&cpu_6502::__ASL,&cpu_6502::__ILL,  //  00 
          &cpu_6502::__BPL,&cpu_6502::__ORA,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ORA,&cpu_6502::__ASL,&cpu_6502::__ILL,&cpu_6502::__CLC,&cpu_6502::__ORA,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ORA,&cpu_6502::__ASL,&cpu_6502::__ILL,  //  01 
          &cpu_6502::__JSR,&cpu_6502::__AND,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__BIT,&cpu_6502::__AND,&cpu_6502::__ROL,&cpu_6502::__ILL,&cpu_6502::__PLP,&cpu_6502::__AND,&cpu_6502::__ROL,&cpu_6502::__ILL,&cpu_6502::__BIT,&cpu_6502::__AND,&cpu_6502::__ROL,&cpu_6502::__ILL,  //  02 
          &cpu_6502::__BMI,&cpu_6502::__AND,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__AND,&cpu_6502::__ROL,&cpu_6502::__ILL,&cpu_6502::__SEC,&cpu_6502::__AND,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__AND,&cpu_6502::__ROL,&cpu_6502::__ILL,  //  03 
          &cpu_6502::__RTI,&cpu_6502::__EOR,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__EOR,&cpu_6502::__LSR,&cpu_6502::__ILL,&cpu_6502::__PHA,&cpu_6502::__EOR,&cpu_6502::__LSR,&cpu_6502::__ILL,&cpu_6502::__JMP,&cpu_6502::__EOR,&cpu_6502::__LSR,&cpu_6502::__ILL,  //  04 
          &cpu_6502::__BVC,&cpu_6502::__EOR,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__EOR,&cpu_6502::__LSR,&cpu_6502::__ILL,&cpu_6502::__CLI,&cpu_6502::__EOR,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__EOR,&cpu_6502::__LSR,&cpu_6502::__ILL,  //  05 
          &cpu_6502::__RTS,&cpu_6502::__ADC,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ADC,&cpu_6502::__ROR,&cpu_6502::__ILL,&cpu_6502::__PLA,&cpu_6502::__ADC,&cpu_6502::__ROR,&cpu_6502::__ILL,&cpu_6502::__JMP,&cpu_6502::__ADC,&cpu_6502::__ROR,&cpu_6502::__ILL,  //  06 
          &cpu_6502::__BVS,&cpu_6502::__ADC,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ADC,&cpu_6502::__ROR,&cpu_6502::__ILL,&cpu_6502::__SEI,&cpu_6502::__ADC,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ADC,&cpu_6502::__ROR,&cpu_6502::__ILL,  //  07 
          &cpu_6502::__ILL,&cpu_6502::__STA,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__STY,&cpu_6502::__STA,&cpu_6502::__STX,&cpu_6502::__ILL,&cpu_6502::__DEY,&cpu_6502::__ILL,&cpu_6502::__TXA,&cpu_6502::__ILL,&cpu_6502::__STY,&cpu_6502::__STA,&cpu_6502::__STX,&cpu_6502::__ILL,  //  08 
          &cpu_6502::__BCC,&cpu_6502::__STA,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__STY,&cpu_6502::__STA,&cpu_6502::__STX,&cpu_6502::__ILL,&cpu_6502::__TYA,&cpu_6502::__STA,&cpu_6502::__TXS,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__STA,&cpu_6502::__ILL,&cpu_6502::__ILL,  //  09 
          &cpu_6502::__LDY,&cpu_6502::__LDA,&cpu_6502::__LDX,&cpu_6502::__ILL,&cpu_6502::__LDY,&cpu_6502::__LDA,&cpu_6502::__LDX,&cpu_6502::__ILL,&cpu_6502::__TAY,&cpu_6502::__LDA,&cpu_6502::__TAX,&cpu_6502::__ILL,&cpu_6502::__LDY,&cpu_6502::__LDA,&cpu_6502::__LDX,&cpu_6502::__ILL,  //  0A 
          &cpu_6502::__BCS,&cpu_6502::__LDA,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__LDY,&cpu_6502::__LDA,&cpu_6502::__LDX,&cpu_6502::__ILL,&cpu_6502::__CLV,&cpu_6502::__LDA,&cpu_6502::__TSX,&cpu_6502::__ILL,&cpu_6502::__LDY,&cpu_6502::__LDA,&cpu_6502::__LDX,&cpu_6502::__ILL,  //  0B 
          &cpu_6502::__CPY,&cpu_6502::__CMP,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__CPY,&cpu_6502::__CMP,&cpu_6502::__DEC,&cpu_6502::__ILL,&cpu_6502::__INY,&cpu_6502::__CMP,&cpu_6502::__DEX,&cpu_6502::__ILL,&cpu_6502::__CPY,&cpu_6502::__CMP,&cpu_6502::__DEC,&cpu_6502::__ILL,  //  0C 
          &cpu_6502::__BNE,&cpu_6502::__CMP,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__CMP,&cpu_6502::__DEC,&cpu_6502::__ILL,&cpu_6502::__CLD,&cpu_6502::__CMP,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__CMP,&cpu_6502::__DEC,&cpu_6502::__ILL,  //  0D 
          &cpu_6502::__CPX,&cpu_6502::__SBC,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__CPX,&cpu_6502::__SBC,&cpu_6502::__INC,&cpu_6502::__ILL,&cpu_6502::__INX,&cpu_6502::__SBC,&cpu_6502::__NOP,&cpu_6502::__ILL,&cpu_6502::__CPX,&cpu_6502::__SBC,&cpu_6502::__INC,&cpu_6502::__ILL,  //  0E 
          &cpu_6502::__BEQ,&cpu_6502::__SBC,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__SBC,&cpu_6502::__INC,&cpu_6502::__ILL,&cpu_6502::__SED,&cpu_6502::__SBC,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__ILL,&cpu_6502::__SBC,&cpu_6502::__INC,&cpu_6502::__ILL   //  0F 
};

Address_t ADDRES_METHOD[13] = {
		&cpu_6502::__IMPLIED,&cpu_6502::__ACCUMULATE,&cpu_6502::__IMMEDIATE,&cpu_6502::__ZEROPAGE,
		&cpu_6502::__ZEROPAGEX,&cpu_6502::__ZEROPAGEY,&cpu_6502::__ABSOLUTE,&cpu_6502::__ABSOLUTEX,
		&cpu_6502::__ABSOLUTEY,&cpu_6502::__INDEXEDX,&cpu_6502::__INDEXEDY,&cpu_6502::__RELATIVE,&cpu_6502::__INDIRECT
};

int ADDRESS_TABLE[256] = {
//         00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F 
            0,  9,  0,  0,  0,  3,  3,  0,  0,  2,  1,  0,  0,  6,  6,  0,  //  00 
           11, 10,  0,  0,  0,  4,  4,  0,  0,  8,  0,  0,  0,  7,  7,  0,  //  01 
            6,  9,  0,  0,  3,  3,  3,  0,  0,  2,  1,  0,  6,  6,  6,  0,  //  02 
           11, 10,  0,  0,  0,  4,  4,  0,  0,  8,  0,  0,  0,  7,  7,  0,  //  03 
            0,  9,  0,  0,  0,  3,  3,  0,  0,  2,  1,  0,  6,  6,  6,  0,  //  04 
           11, 10,  0,  0,  0,  4,  4,  0,  0,  8,  0,  0,  0,  7,  7,  0,  //  05 
            0,  9,  0,  0,  0,  3,  3,  0,  0,  2,  1,  0, 12,  6,  6,  0,  //  06 
           11, 10,  0,  0,  0,  4,  4,  0,  0,  8,  0,  0,  0,  7,  7,  0,  //  07 
            0,  9,  0,  0,  3,  3,  3,  0,  0,  0,  0,  0,  6,  6,  6,  0,  //  08 
           11, 10,  0,  0,  4,  4,  5,  0,  0,  8,  0,  0,  0,  7,  0,  0,  //  09 
            2,  9,  2,  0,  3,  3,  3,  0,  0,  2,  0,  0,  6,  6,  6,  0,  //  0A 
           11, 10,  0,  0,  4,  4,  5,  0,  0,  8,  0,  0,  7,  7,  8,  0,  //  0B 
            2,  9,  0,  0,  3,  3,  3,  0,  0,  2,  0,  0,  6,  6,  6,  0,  //  0C 
           11, 10,  0,  0,  0,  4,  4,  0,  0,  8,  0,  0,  0,  7,  7,  0,  //  0D 
            2,  9,  0,  0,  3,  3,  3,  0,  0,  2,  0,  0,  6,  6,  6,  0,  //  0E 
           11, 10,  0,  0,  0,  4,  4,  0,  0,  8,  0,  0,  0,  7,  7, 00   //  0F 
};

int CYCLES_TABLE[256] = {
//         00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F 
            7,  6,  0,  0,  2,  3,  5,  0,  3,  2,  2,  0,  0,  4,  6,  0,  //  00 
            2,  5,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0,  //  01 
            6,  6,  0,  0,  3,  3,  5,  0,  4,  2,  2,  0,  4,  4,  6,  0,  //  02 
            2,  5,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0,  //  03 
            6,  6,  0,  0,  0,  3,  5,  0,  3,  2,  2,  0,  3,  4,  6,  0,  //  04 
            2,  5,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0,  //  05 
            6,  6,  0,  0,  0,  3,  5,  0,  4,  2,  2,  0,  5,  4,  6,  0,  //  06 
            2,  6,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0,  //  07 
            0,  6,  0,  0,  3,  3,  3,  0,  2,  0,  2,  0,  4,  4,  4,  0,  //  08 
            2,  6,  0,  0,  4,  4,  4,  0,  2,  5,  2,  0,  0,  5,  0,  0,  //  09 
            2,  6,  2,  0,  3,  3,  3,  0,  2,  2,  2,  0,  4,  4,  4,  0,  //  0A 
            2,  5,  0,  0,  4,  4,  4,  0,  2,  4,  2,  0,  4,  4,  4,  0,  //  0B 
            2,  6,  0,  0,  3,  3,  5,  0,  2,  2,  2,  0,  4,  4,  6,  0,  //  0C 
            2,  3,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0,  //  0D 
            2,  6,  0,  0,  3,  3,  5,  0,  2,  2,  2,  0,  4,  4,  6,  0,  //  0E 
            2,  5,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0,  //  0F 
};

cpu_6502::cpu_6502(BusRead r, BusWrite w)
{
	read = (BusRead)r;
	write = (BusWrite)w;
	
	P = 0;
	tick = 0;
	Reset();
}

void cpu_6502::Reset()
{
	A = 0x00;
	Y = 0x00;
	X = 0x00;
	PC = (read(0xFFFD) << 8) + read(0xFFFC); 
	SP = 0xFD;
	P |= _CONSTANT;

	illegal = false;
}

void cpu_6502::push(uint8_t byte)
{
	write(0x0100 + SP--, byte);
}

uint8_t cpu_6502::pop()
{	
	SP++;
	return read(0x0100+SP);
}

void cpu_6502::IRQ()
{
	if(!(P & _INTERRUPT))
	{
		P &= ~_BREAK;
		
		push(PC >> 8);
		push(PC & 0xFF);
		push(P);
		
		P |= _INTERRUPT;

		PC = (read(0xFFFF) << 8) + read(0xFFFE);
	}
}

   
void cpu_6502::NMI()
{
	P &= ~_BREAK;
	
	push(PC >> 8);
	push(PC & 0xFF);
	push(P);
	
	P |= _INTERRUPT;

	PC = (read(0xFFFB) << 8) + read(0xFFFA);
}

uint16_t cpu_6502::Print()
{
	printf("A-%02X X-%02X Y-%02X S-%02X P-%02X ",A,X,Y,SP,P);

	if (P & _NEGATIVE) printf("N");
	if (P & _OVERFLOW) printf("O");
	if (P & _BREAK) printf("B");
	if (P & _DECIMAL) printf("D");
	if (P & _INTERRUPT) printf("I");
	if (P & _ZERO) printf("Z");
	if (P & _CARRY) printf("C");
	
	printf("\n");
	return PC;	
}

uint16_t cpu_6502::Dump(uint8_t* _A, uint8_t* _X,uint8_t* _Y,uint8_t* _SP, uint8_t* _P)
{
	*_A=A;
	*_X=X;
	*_Y=Y;
	*_SP=SP;
	*_P=P;
	
	return PC;	
}

uint16_t cpu_6502::Step() 
{
	uint8_t opcode;
	uint8_t addressing;

	if (!illegal)
	{
		FROM = PC;
		opcode = read(PC++);
		addressing = ADDRESS_TABLE[opcode];
		
		// update elapsedCycles
		elapsedCycles = CYCLES_TABLE[opcode];
		tick += elapsedCycles;
	
		// Call instruction method
		(this->*(INSTR_TABLE[opcode]))(addressing);
	}
	
	return PC;
}

void cpu_6502::setPC(uint16_t _pc)
{
	PC = _pc;
}

uint16_t cpu_6502::getPC()
{
	return PC;
}

uint64_t cpu_6502::getTick()
{
	return tick;
}

uint16_t cpu_6502::getFROM()
{
	return FROM;
}
	
uint64_t cpu_6502::getElapsedCycles()
{
	return elapsedCycles;
}

void cpu_6502::resetElapsedCycles()
{
	elapsedCycles = 0;
}

uint8_t cpu_6502::getIllegalOpcode()
{
	if (illegal)
		return 1;
	else
		return 0;
}

void cpu_6502::resetIllegalOpcode()
{
	illegal = false;
}


void cpu_6502::__ADC(int a)
{

	uint16_t address;
	uint8_t m; 
	uint16_t tmp;

	address = (this->*(ADDRES_METHOD[a]))();
	m = read(address);
	
	tmp = m + A + (P & _CARRY);
	
	// ZERO
	if (!(tmp & 0xFF)) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
		
	if (P & _DECIMAL)
	{
		if (((A & 0xF) + (m & 0xF) + (P & _CARRY)) > 9) 
			tmp += 6;
			
		// NEGATIVE
		if (tmp & 0x80) 
			P |= _NEGATIVE;
		else
			P &= ~_NEGATIVE;
			
		// OVERFLOW
		if (!((A ^ m) & 0x80) && ((A ^ tmp) & 0x80))
			P |= _OVERFLOW;
		else
			P &= ~_OVERFLOW;
			
		if (tmp > 0x99)
		{
			tmp += 96;
		}
		
		// CARRY
		if(tmp > 0x99)
			P |= _CARRY;
		else
			P &= ~_CARRY;
	}
	else
	{
		// NEGATIVE
		if (tmp & 0x80) 
			P |= _NEGATIVE;
		else
			P &= ~_NEGATIVE;
		
		// OVERFLOW
		if (!((A ^ m) & 0x80) && ((A ^ tmp) & 0x80))
			P |= _OVERFLOW;
		else
			P &= ~_OVERFLOW;

		// CARRY
		if(tmp > 0xFF)
			P |= _CARRY;
		else
			P &= ~_CARRY;		
	}

	A = tmp & 0xFF;
}

void cpu_6502::__AND(int a)
{
	uint16_t address;
	uint8_t m; 

	address = (this->*(ADDRES_METHOD[a]))();
	m = read(address);	
	
	A &= m;
	
	// NEGATIVE
	if (A & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!A) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__ASL(int a)
{
	uint16_t address;
	uint8_t m; 
	
	if (a == 1)
	{
		// ACCUMULATOR
		m = A;
	}
	else
	{
		// DO ADDRESSING
		address = (this->*(ADDRES_METHOD[a]))();
		m = read(address);
	}
	
	// CARRY
	if (m & 0x80) 
		P |= _CARRY;
	else
		P &= ~_CARRY;
	
	m <<= 1;
	
	// NEGATIVE
	if (m & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
	
	// ZERO
	if (!m) 
		P |= _ZERO;
	else
		P &= ~_ZERO;

	if (a == 1)
	{
		// ACCUMULATOR
		A = m;
	}
	else
	{
		// WRITE MEMORY
		write(address,m);
	}	
}

void cpu_6502::__BCC(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	if (!(P & _CARRY)) PC = address;		
}

void cpu_6502::__BCS(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	if (P & _CARRY) PC = address;	
}

void cpu_6502::__BEQ(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	if (P & _ZERO) PC = address;	
}

void cpu_6502::__BIT(int a)
{

	uint16_t address;
	uint8_t m; 
	uint8_t compare;
	
	address = (this->*(ADDRES_METHOD[a]))();
	m = read(address);
	compare = m & A;

	// NEGATIVE
	if (compare & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
	
	P = (P & 0x3F) | (uint8_t)(m & 0xC0);
	
	// ZERO
	if (!compare) 
		P |= _ZERO;
	else
		P &= ~_ZERO;	
	
}

void cpu_6502::__BMI(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	if (P & _NEGATIVE) PC = address;		
}

void cpu_6502::__BNE(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	if (!(P & _ZERO)) PC = address;		
}

void cpu_6502::__BPL(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	if (!(P & _NEGATIVE)) PC = address;	
}

void cpu_6502::__BRK(int a)
{
	PC++;
	push(PC >> 8);
	push(PC & 0xFF);
	push(P | _BREAK);
	P |= _INTERRUPT;
	PC = (read(0xFFFF) << 8) + read(0xFFFE); 
}

void cpu_6502::__BVC(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	if (!(P & _OVERFLOW)) PC = address;
}

void cpu_6502::__BVS(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	if (P & _OVERFLOW) PC = address;	
}

void cpu_6502::__CLC(int a)
{
	P &= ~_CARRY;
}

void cpu_6502::__CLD(int a)
{
	P &= ~_DECIMAL;
}

void cpu_6502::__CLI(int a)
{
	P &= ~_INTERRUPT;
}

void cpu_6502::__CLV(int a)
{
	P &= ~_OVERFLOW;
}

void cpu_6502::__CMP(int a)
{
	uint16_t address;
	uint16_t compare;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	compare = A - read(address);
	
	// CARRY
	if (compare < 0x100) 
		P |= _CARRY;
	else
		P &= ~_CARRY;

	// NEGATIVE
	if (compare & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!(compare & 0xFF)) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__CPX(int a)
{
	uint16_t address;
	uint16_t compare;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	compare = X - read(address);
	
	// CARRY
	if (compare < 0x100) 
		P |= _CARRY;
	else
		P &= ~_CARRY;

	// NEGATIVE
	if (compare & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!(compare & 0xFF)) 
		P |= _ZERO;
	else
		P &= ~_ZERO;	
}

void cpu_6502::__CPY(int a)
{
	uint16_t address;
	uint16_t compare;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	compare = Y - read(address);
	
	// CARRY
	if (compare < 0x100) 
		P |= _CARRY;
	else
		P &= ~_CARRY;

	// NEGATIVE
	if (compare & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!(compare & 0xFF)) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__DEC(int a)
{
	uint16_t address;
	uint8_t m;	
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	m = read(address);
	m--;
	write(address,m);
	
	// NEGATIVE
	if (m & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!m) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__DEX(int a)
{
	X--;
	
	// NEGATIVE
	if (X & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!X) 
		P |= _ZERO;
	else
		P &= ~_ZERO;	
}

void cpu_6502::__DEY(int a)
{
	Y--;
	
	// NEGATIVE
	if (Y & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!Y) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__EOR(int a)
{
	uint16_t address;
	uint8_t m;	
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	m = read(address);
	A ^= m;
	
	// NEGATIVE
	if (A & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!A) 
		P |= _ZERO;
	else
		P &= ~_ZERO;	
}

void cpu_6502::__INC(int a)
{
	uint16_t address;
	uint8_t m;	
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	m = read(address);
	m++;
	write(address,m);
	
	// NEGATIVE
	if (m & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!m) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__INX(int a)
{
	X++;
	
	// NEGATIVE
	if (X & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!X) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__INY(int a)
{
	Y++;
	
	// NEGATIVE
	if (Y & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!Y) 
		P |= _ZERO;
	else
		P &= ~_ZERO;	
}

void cpu_6502::__JMP(int a)
{
	uint16_t address;	
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	PC = address;
}

void cpu_6502::__JSR(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	
	PC--;
	push(PC >> 8);
	push(PC & 0xFF);
	PC = address;
}

void cpu_6502::__LDA(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	A = read(address);
	
	// NEGATIVE
	if (A & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!A) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__LDX(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	X = read(address);
	
	// NEGATIVE
	if (X & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!X) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__LDY(int a)
{
	uint16_t address;
	
	address = (this->*(ADDRES_METHOD[a]))();
	Y = read(address);
	
	// NEGATIVE
	if (Y & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!Y) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__LSR(int a)
{
	uint16_t address;
	uint8_t m; 
	
	if (a == 1)
	{
		// ACCUMULATOR
		m = A;
	}
	else
	{
		// DO ADDRESSING
		address = (this->*(ADDRES_METHOD[a]))();
		m = read(address);
	}
	
	// CARRY
	if (m & 0x01) 
		P |= _CARRY;
	else
		P &= ~_CARRY;
	
	m >>= 1;
	
	P &= ~_NEGATIVE;
	
	// ZERO
	if (!m) 
		P |= _ZERO;
	else
		P &= ~_ZERO;

	if (a == 1)
	{
		// ACCUMULATOR
		A = m;
	}
	else
	{
		// WRITE MEMORY
		write(address,m);
	}				
}

void cpu_6502::__NOP(int a)
{
	return;
}

void cpu_6502::__ORA(int a)
{
	uint16_t address;
	uint8_t m; 

	address = (this->*(ADDRES_METHOD[a]))();
	m = read(address);	
	
	A |= m;
	
	// NEGATIVE
	if (A & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!A) 
		P |= _ZERO;
	else
		P &= ~_ZERO;	
}

void cpu_6502::__PHA(int a)
{
	push(A);
}

void cpu_6502::__PHP(int a)
{
	push(P | _BREAK);
}

void cpu_6502::__PLA(int a)
{
	A = pop();
	
	// NEGATIVE
	if (A & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!A) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__PLP(int a)
{
	P = pop();
	P |= _CONSTANT;
}

void cpu_6502::__ROL(int a)
{	
	uint16_t address;
	uint16_t m; 
	
	if (a == 1)
	{
		// ACCUMULATOR
		m = A;
	}
	else
	{
		// DO ADDRESSING
		address = (this->*(ADDRES_METHOD[a]))();
		m = read(address);
	}
	
	m <<= 1;
	
	if (P & _CARRY)
		m |= 0x01;

	// CARRY
	if (m > 0xFF) 
		P |= _CARRY;
	else
		P &= ~_CARRY;

	m &= 0xFF;
		
	// NEGATIVE
	if (m & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!m) 
		P |= _ZERO;
	else
		P &= ~_ZERO;

	if (a == 1)
	{
		// ACCUMULATOR
		A = m;
	}
	else
	{
		// WRITE MEMORY
		write(address,m);
	}	
}

void cpu_6502::__ROR(int a)
{
	uint16_t address;
	uint16_t m; 
	
	if (a == 1)
	{
		// ACCUMULATOR
		m = A;
	}
	else
	{
		// DO ADDRESSING
		address = (this->*(ADDRES_METHOD[a]))();
		m = read(address);
	}
	
	if (P & _CARRY)
		m |= 0x100;
	
	// CARRY
	if (m & 0x01)
		P |= _CARRY;
	else
		P &= ~_CARRY;
			
	m >>= 1;
	m &= 0xFF;
	
	// NEGATIVE
	if (m & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!m) 
		P |= _ZERO;
	else
		P &= ~_ZERO;

	if (a == 1)
	{
		// ACCUMULATOR
		A = m;
	}
	else
	{
		// WRITE MEMORY
		write(address,m);
	}
}

void cpu_6502::__RTI(int a)
{
	uint8_t PCL;
	uint8_t PCH;

	P = pop();

	PCL = pop();
	PCH = pop();

	PC = (PCH << 8) | PCL;	
}

void cpu_6502::__RTS(int a)
{
	uint8_t PCL;
	uint8_t PCH;

	PCL = pop();
	PCH = pop();

	PC = (PCH << 8) | PCL + 1;	
}

void cpu_6502::__SBC(int a)
{
	uint16_t address;
	uint16_t tmp;
	uint8_t m; 
	uint8_t c;

	address = (this->*(ADDRES_METHOD[a]))();
	m = read(address);
	
	if (P & _CARRY)
		c = 0;
	else
		c = 1;
		
	tmp = A - m - c;
	
	// NEGATIVE
	if (tmp & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;

	// NEGATIVE
	if (!(tmp & 0xFF))
		P |= _ZERO;
	else
		P &= ~_ZERO;
		
		
	// OVERFLOW
	if (((A ^ tmp) & 0x80) && ((A ^ m) & 0x80))
		P |= _OVERFLOW;
	else
		P &= ~_OVERFLOW;	

	if (P & _DECIMAL)
	{
		if (((A & 0x0F) - c) < (m & 0x0F)) 
			tmp -= 6;
			
		if (tmp > 0x99)
		{
			tmp -= 0x60;
		}
	}
	
	if (tmp < 0x100)
	{
		P |= _CARRY;
	}
	else
	{
		P &= ~_CARRY; 
	}
	
	A = (tmp & 0xFF);
}

void cpu_6502::__SEC(int a)
{
	P |= _CARRY;	
}

void cpu_6502::__SED(int a)
{
	P |= _DECIMAL;
}

void cpu_6502::__SEI(int a)
{
	P |= _INTERRUPT;
}

void cpu_6502::__STA(int a)
{
	uint16_t address;

	address = (this->*(ADDRES_METHOD[a]))();
	
	write(address,A);
}

void cpu_6502::__STX(int a)
{
	uint16_t address;

	address = (this->*(ADDRES_METHOD[a]))();
	
	write(address,X);
}

void cpu_6502::__STY(int a)
{
	uint16_t address;

	address = (this->*(ADDRES_METHOD[a]))();
	
	write(address,Y);	
}

void cpu_6502::__TAX(int a)
{
	X = A;

	// NEGATIVE
	if (X & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!X) 
		P |= _ZERO;
	else
		P &= ~_ZERO;	
}

void cpu_6502::__TAY(int a)
{
	Y = A;

	// NEGATIVE
	if (Y & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!Y) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__TSX(int a)
{
	X = SP;

	// NEGATIVE
	if (X & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!X) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__TXA(int a)
{
	A = X;

	// NEGATIVE
	if (A & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!A) 
		P |= _ZERO;
	else
		P &= ~_ZERO;
}

void cpu_6502::__TXS(int a)
{
	SP = X;
}

void cpu_6502::__TYA(int a)
{
	A = Y;

	// NEGATIVE
	if (A & 0x80) 
		P |= _NEGATIVE;
	else
		P &= ~_NEGATIVE;
		
	// ZERO
	if (!A) 
		P |= _ZERO;
	else
		P &= ~_ZERO;	
}

void cpu_6502::__ILL(int a)
{
	illegal = true;
}

uint16_t cpu_6502::__IMPLIED()
{
    return 0;
}

uint16_t cpu_6502::__ACCUMULATE()
{
    return 0;
}

uint16_t cpu_6502::__IMMEDIATE()
{
    return PC++;
}

uint16_t cpu_6502::__ZEROPAGE()
{
	uint16_t address;
	
	address = read(PC++);
	
    return address;
}

uint16_t cpu_6502::__ZEROPAGEX()
{
	uint16_t address;
	
	address = (read(PC++)+X) & 0XFF;
	
    return address;    
}

uint16_t cpu_6502::__ZEROPAGEY()
{
	uint16_t address;
	
	address = (read(PC++)+Y) & 0XFF;
	
    return address;    
}

uint16_t cpu_6502::__ABSOLUTE()
{
	uint16_t address;
	
	address = read(PC++);		// Low byte
	address += read(PC++)<<8;	// High byte
	
    return address;	
}

uint16_t cpu_6502::__ABSOLUTEX()
{
	uint16_t address;
	
	address = read(PC++);		// Low byte
	address += read(PC++)<<8;	// High byte
	address += X;
	
    return address;	
}

uint16_t cpu_6502::__ABSOLUTEY()
{
	uint16_t address;
	
	address = read(PC++);		// Low byte
	address += read(PC++)<<8;	// High byte
	address += Y;
	
    return address;	
}

uint16_t cpu_6502::__INDEXEDX()
{
	uint16_t address;
	uint8_t  low;
	uint8_t  high;

	// Indexed indirect	
	address = (read(PC++)+X) & 0XFF;

	low = read(address);
	high = read(address+1);
	
	address = low + (high << 8);
	
    return address; 
}

uint16_t cpu_6502::__INDEXEDY()
{
	uint16_t address;
	uint8_t  low;
	uint8_t  high;
	
	// Indirect indexed
	address = read(PC++);
	
	low = read(address);
	high = read(address+1);
	
	address = low + (high << 8) + Y;
    
	return address;
}

uint16_t cpu_6502::__RELATIVE()
{
	uint8_t offset;
	uint16_t address;
	
	offset = read(PC++);
	
	if (offset < 128) 
	{
		address = PC + offset;
	}
	else
	{
		address = PC + offset - 256;
	}
	
	return address;
}

uint16_t cpu_6502::__INDIRECT()
{
	uint16_t address;
	uint8_t  low;
	uint8_t  high;
	
	address = read(PC++);		// Low byte
	address += read(PC++)<<8;	// High byte
	
	low = read(address);
	high = read(address+1);
	
	address = low + (high << 8);
	
	return address;
}

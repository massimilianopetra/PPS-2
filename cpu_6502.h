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

#ifndef __CPU_6502_H
#define __CPU_6502_H

#include <stdint.h>

#define _NEGATIVE  0x80
#define _OVERFLOW  0x40
#define _CONSTANT  0x20
#define _BREAK     0x10
#define _DECIMAL   0x08
#define _INTERRUPT 0x04
#define _ZERO      0x02
#define _CARRY     0x01

typedef void (*BusWrite)(uint16_t, uint8_t);
typedef uint8_t (*BusRead)(uint16_t);
	
class cpu_6502
{
	private:
		
		// 6502 Registers
		uint8_t A;
		uint8_t X;
		uint8_t Y;
		uint8_t SP;
		uint8_t P;
		
		// Program counter
		uint16_t PC;
		uint16_t FROM;
		
		// Bus accessing method
		BusRead read;
		BusWrite write;
		
		// Stack
		uint8_t pop();
		void push(uint8_t byte);
		
		// Flasg
		bool illegal;
		
		// Cycles
		uint64_t elapsedCycles;
		uint64_t tick;

	public:
		
		cpu_6502(BusRead r, BusWrite w);
		
		// Interrupt
		void NMI();
		void IRQ();
		void Reset();	
		
		uint16_t Step();
		uint16_t getPC();
		uint64_t getTick();
		uint16_t getFROM();
		void setPC(uint16_t _pc);
		uint16_t Print();
		uint16_t Dump(uint8_t* _A, uint8_t* _X,uint8_t* _Y,uint8_t* _SP, uint8_t* _P);
		uint8_t getIllegalOpcode();
		uint64_t getElapsedCycles();
		void resetElapsedCycles();
		void resetIllegalOpcode();
		
		// Addressing modes
		uint16_t __IMPLIED();
		uint16_t __ACCUMULATE();
		uint16_t __IMMEDIATE();
		uint16_t __ZEROPAGE();
		uint16_t __ZEROPAGEX();
		uint16_t __ZEROPAGEY();
		uint16_t __ABSOLUTE();
		uint16_t __ABSOLUTEX();
		uint16_t __ABSOLUTEY();
		uint16_t __INDEXEDX();
		uint16_t __INDEXEDY();
		uint16_t __RELATIVE();
		uint16_t __INDIRECT();
		
		// Intstructions set
		void __ADC(int a);
		void __AND(int a);
		void __ASL(int a);
		void __BCC(int a);
		void __BCS(int a);
		void __BEQ(int a);
		void __BIT(int a);
		void __BMI(int a);
		void __BNE(int a);
		void __BPL(int a);
		void __BRK(int a);
		void __BVC(int a);
		void __BVS(int a);
		void __CLC(int a);
		void __CLD(int a);
		void __CLI(int a);
		void __CLV(int a);
		void __CMP(int a);
		void __CPX(int a);
		void __CPY(int a);
		void __DEC(int a);
		void __DEX(int a);
		void __DEY(int a);
		void __EOR(int a);
		void __INC(int a);
		void __INX(int a);
		void __INY(int a);
		void __JMP(int a);
		void __JSR(int a);
		void __LDA(int a);
		void __LDX(int a);
		void __LDY(int a);
		void __LSR(int a);
		void __NOP(int a);
		void __ORA(int a);
		void __PHA(int a);
		void __PHP(int a);
		void __PLA(int a);
		void __PLP(int a);
		void __ROL(int a);
		void __ROR(int a);
		void __RTI(int a);
		void __RTS(int a);
		void __SBC(int a);
		void __SEC(int a);
		void __SED(int a);
		void __SEI(int a);
		void __STA(int a);
		void __STX(int a);
		void __STY(int a);
		void __TAX(int a);
		void __TAY(int a);
		void __TSX(int a);
		void __TXA(int a);
		void __TXS(int a);
		void __TYA(int a);
		void __ILL(int a);
};

typedef void (cpu_6502::*Instr_t)(int);
typedef uint16_t (cpu_6502::*Address_t)();

#endif

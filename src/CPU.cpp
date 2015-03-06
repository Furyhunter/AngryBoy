#include "CPU.h"

#include <iostream>

#include "common.h"

using namespace std;

// CPU operation implementations

#define INSTRUCTION(name) FORCEINLINE static u32 name(shared_ptr<FCPU> CPU, u16 OpCode)

INSTRUCTION(LD_nn_n)
{
	int Param = (OpCode - 0x06) / 8;
	u8* Register = &CPU->b;
	switch (Param)
	{
	case 0: Register = &CPU->b;
	case 1: Register = &CPU->c;
	case 2: Register = &CPU->d;
	case 3: Register = &CPU->e;
	case 4: Register = &CPU->h;
	case 5: Register = &CPU->l;
	default:
		CPU->Stopped = true;
		return 8;
	}
	*Register = CPU->MemoryController[CPU->pc + 1];
	CPU->pc += 1;
	return 8;
}

INSTRUCTION(LD_r1_r2)
{
	u8* Register1 = nullptr;
	u8* Register2 = nullptr;
	u16 CurrentHLValue = CPU->hl();
	u32 Cycles = 4;

	if (BETWEEN(OpCode, 0x78, 0x7F))
		Register1 = &CPU->a;
	if (BETWEEN(OpCode, 0x40, 0x47))
		Register1 = &CPU->b;
	if (BETWEEN(OpCode, 0x48, 0x4F))
		Register1 = &CPU->c;
	if (BETWEEN(OpCode, 0x50, 0x57))
		Register1 = &CPU->d;
	if (BETWEEN(OpCode, 0x58, 0x5F))
		Register1 = &CPU->e;
	if (BETWEEN(OpCode, 0x60, 0x67))
		Register1 = &CPU->h;
	if (BETWEEN(OpCode, 0x68, 0x70))
		Register1 = &CPU->l;
	if (BETWEEN(OpCode, 0x70, 0x77))
		Register1 = (u8*)42;
	
	u8 Low = OpCode & 0x0F;
	if (Register1 == &CPU->a)
	{
		switch (Low)
		{
		case 0x8: Register2 = &CPU->b; break;
		case 0x9: Register2 = &CPU->c; break;
		case 0xa: Register2 = &CPU->d; break;
		case 0xb: Register2 = &CPU->e; break;
		case 0xc: Register2 = &CPU->h; break;
		case 0xd: Register2 = &CPU->l; break;
		case 0xe: Register2 = (u8*)42; Cycles = 8; break;
		case 0xf: Register2 = &CPU->a; break;
		default: break;
		}
	}
	else
	{
		switch (Low)
		{
		case 0x0: Register2 = &CPU->b; break;
		case 0x1: Register2 = &CPU->c; break;
		case 0x2: Register2 = &CPU->d; break;
		case 0x3: Register2 = &CPU->e; break;
		case 0x4: Register2 = &CPU->h; break;
		case 0x5: Register2 = &CPU->l; break;
		case 0x6: Register2 = (u8*)42; Cycles = 8; break;

		case 0x8: Register2 = &CPU->b; break;
		case 0x9: Register2 = &CPU->c; break;
		case 0xa: Register2 = &CPU->d; break;
		case 0xb: Register2 = &CPU->e; break;
		case 0xc: Register2 = &CPU->h; break;
		case 0xd: Register2 = &CPU->l; break;
		case 0xe: Register2 = (u8*)42; Cycles = 8; break;
		default: break;
		}
	}
	
	if (Register1 == (u8 *)42)
	{
		CPU->MemoryController.SetByte(CurrentHLValue, *Register2);
	}
	else if (Register2 == (u8 *)42)
	{
		*Register1 = CPU->MemoryController[CPU->hl()];
	}
	else if (OpCode == 0x36)
	{
		CPU->MemoryController[CPU->hl()] = CPU->NextByte();
	}
	else
	{
		*Register1 = *Register2;
	}
	
	return Cycles;
}

INSTRUCTION(LD_A_n)
{
	u8 * RegisterA = &CPU->a;
	u8 * RegisterN = nullptr;
	u8 High = OpCode & 0xF0;
	u8 Low = OpCode & 0x0F;
	u32 Cycles = 4;
	
	if (High == 0x70)
	{
		switch (Low)
		{
			case 0xf: RegisterN = &CPU->a; break;
			case 0x8: RegisterN = &CPU->b; break;
			case 0x9: RegisterN = &CPU->c; break;
			case 0xa: RegisterN = &CPU->d; break;
			case 0xb: RegisterN = &CPU->e; break;
			case 0xc: RegisterN = &CPU->h; break;
			case 0xd: RegisterN = &CPU->l; break;
			case 0xe: RegisterN = (u8 *)42; Cycles = 8; break;
			default: break;
		}
		
		if (RegisterN == (u8 *)42)
		{
			*RegisterA = CPU->MemoryController[CPU->hl()];
		}
		else
		{
			*RegisterA = *RegisterN;
		}
	}
	else if (OpCode == 0x0A)
	{
		u16 Loc = ((u16)(CPU->b) << 8) + ((u16)(CPU->c));
		*RegisterA = CPU->MemoryController[Loc];
		Cycles = 8;
	}
	else if (OpCode == 0x1A)
	{
		u16 Loc = ((u16)(CPU->d) << 8) + ((u16)(CPU->e));
		*RegisterA = CPU->MemoryController[Loc];
		Cycles = 8;
	}
	else if (OpCode == 0xFA)
	{
		u16 Loc = CPU->NextWord();
		*RegisterA = CPU->MemoryController[Loc];
		Cycles = 16;
	}
	else if (OpCode == 0x3E)
	{
		*RegisterA = CPU->NextByte();
		Cycles = 8;
	}
	return Cycles;
}

INSTRUCTION(LD_n_A)
{
	u32 Cycles = 4;
	u16 Loc = 0;
	
	switch (OpCode)
	{
		case 0x7F: CPU->a = CPU->a; break;
		case 0x47: CPU->b = CPU->a; break;
		case 0x4F: CPU->c = CPU->a; break;
		case 0x57: CPU->d = CPU->a; break;
		case 0x5F: CPU->e = CPU->a; break;
		case 0x67: CPU->h = CPU->a; break;
		case 0x6F: CPU->l = CPU->a; break;
		case 0x02:
			Loc = ((u16)(CPU->b) << 8) + ((u16)(CPU->c));
			CPU->MemoryController[Loc] = CPU->a;
			Cycles = 8;
			break;
		case 0x12:
			Loc = ((u16)(CPU->d) << 8) + ((u16)(CPU->e));
			CPU->MemoryController[Loc] = CPU->a;
			Cycles = 8;
			break;
		case 0x77:
			Loc = ((u16)(CPU->h) << 8) + ((u16)(CPU->l));
			CPU->MemoryController[Loc] = CPU->a;
			Cycles = 8;
			break;
		case 0xEA:
			Loc = ((u16)(CPU->MemoryController[++CPU->pc])) + ((u16)(CPU->MemoryController[++CPU->pc]) << 8);
			CPU->MemoryController[Loc] = CPU->a;
			Cycles = 16;
			break;
		default:
			break;
	}
	
	return Cycles;
}

INSTRUCTION(LD_A_PC)
{
	CPU->a = CPU->MemoryController[0xFF00 + CPU->c];
	return 8;
}

INSTRUCTION(LD_PC_A)
{
	CPU->MemoryController[0xFF00 + CPU->c] = CPU->a;
	return 8;
}

INSTRUCTION(LD_A_PHLD)
{
	CPU->a = CPU->MemoryController[CPU->hl()];
	CPU->hl(CPU->hl() - 1);
	return 8;
}

INSTRUCTION(LD_PHLD_A)
{
	CPU->MemoryController[CPU->hl()] = CPU->a;
	CPU->hl(CPU->hl() - 1);
	return 8;
}

INSTRUCTION(LD_A_PHLI)
{
	CPU->a = CPU->MemoryController[CPU->hl()];
	CPU->hl(CPU->hl() + 1);
	return 8;
}

INSTRUCTION(LD_PHLI_A)
{
	CPU->MemoryController[CPU->hl()] = CPU->a;
	CPU->hl(CPU->hl() + 1);
	return 8;
}

INSTRUCTION(LDH_Pn_A)
{
	CPU->MemoryController[0xFF00 + CPU->NextByte()] = CPU->a;
	return 12;
}

INSTRUCTION(LDH_A_Pn)
{
	CPU->a = CPU->MemoryController[0xFF00 + CPU->NextByte()];
	return 12;
}

/* #####  16-bit Loads  #####*/
INSTRUCTION(LD_sn_snn)
{
	u8 High = OpCode & 0xF0;
	
	switch (High)
	{
	case 0: CPU->bc(CPU->NextWord()); break;
	case 1: CPU->de(CPU->NextWord()); break;
	case 2: CPU->hl(CPU->NextWord()); break;
	case 3: CPU->sp = CPU->NextWord(); break;
	default: break;
	}
	return 12;
}

INSTRUCTION(LD_SP_HL)
{
	CPU->sp = CPU->hl();
	return 8;
}

INSTRUCTION(LD_SP_n)
{
	CPU->hl(CPU->sp + CPU->NextByte());
	return 12;
}

INSTRUCTION(LD_Pnn_SP)
{
	CPU->MemoryController[CPU->NextWord()] = CPU->sp;
	return 20;
}

INSTRUCTION(PUSH_nn)
{
	CPU->sp -= 2;
	u8 High = OpCode & 0xF0;
	u16 Value = 0;
	
	switch (High)
	{
		case 0xF: Value = CPU->af(); break;
		case 0xC: Value = CPU->bc(); break;
		case 0xD: Value = CPU->de(); break;
		case 0xE: Value = CPU->hl(); break;
		default: break;
	}
	CPU->MemoryController[CPU->sp] = Value;

	return 16;
}

INSTRUCTION(POP_nn)
{
	u8 High = OpCode & 0xF0;
	u16 Value = CPU->MemoryController.GetWord(CPU->sp);
	
	switch (High)
	{
	case 0xF: CPU->af(Value); break;
	case 0xC: CPU->bc(Value); break;
	case 0xD: CPU->de(Value); break;
	case 0xE: CPU->hl(Value); break;
	}
	CPU->sp += 2;
	return 12;
}

INSTRUCTION(ADD_A_n)
{
	u8 Low = OpCode & 0x0F;
	u8 Value = 0;
	
	if (OpCode == 0xC6)
	{
		Value = CPU->NextByte();
	}
	else
	{
		switch (Low)
		{
		case 0x7: Value = CPU->a; break;
		case 0x0: Value = CPU->b; break;
		case 0x1: Value = CPU->c; break;
		case 0x2: Value = CPU->d; break;
		case 0x3: Value = CPU->e; break;
		case 0x4: Value = CPU->h; break;
		case 0x5: Value = CPU->l; break;
		case 0x6: Value = CPU->MemoryController[CPU->hl()]; break;
		default: break;
		}
	}
	
	u32 Result = (u32)CPU->a + (u32)Value;
	u32 CarryBits = (u32)CPU->a ^ Value ^ Result;
	CPU->ClearFlags();
	if ((CarryBits & 0x100) != 0) CPU->ToggleFlagC();
	if ((CarryBits & 0x10)  != 0) CPU->ToggleFlagH();
	if ((Result & 0xFF)     == 0) CPU->ToggleFlagZ();
	CPU->a = Result & 0xFF;
	return 4;
}

INSTRUCTION(ADC_A_n)
{
	u8 Low = OpCode & 0x0F;
	u8 Value = 0;
	
	if (OpCode == 0xCE)
	{
		Value = CPU->NextByte();
	}
	else
	{
		switch (Low)
		{
		case 0xF: Value = CPU->a; break;
		case 0x8: Value = CPU->b; break;
		case 0x9: Value = CPU->c; break;
		case 0xA: Value = CPU->d; break;
		case 0xB: Value = CPU->e; break;
		case 0xC: Value = CPU->h; break;
		case 0xD: Value = CPU->l; break;
		case 0xE: Value = CPU->MemoryController[CPU->hl()]; break;
		default: break;
		}
	}

	u32 Carry = (CPU->GetFlagC() ? 1 : 0);
	u32 Result = (u32)CPU->a + Value + Carry;
	CPU->ClearFlags();
	if ((Result & 0xFFFF) == 0) CPU->ToggleFlagZ();
	if ((Result & 0xFFFF) > 0xFF) CPU->ToggleFlagC();
	if ((Result & 0xFFFF) > 0xF) CPU->ToggleFlagH();
	return 4;
}

INSTRUCTION(SUB_n)
{
	u8 Low = OpCode & 0x0F;
	u8 Value = 0;
	
	if (OpCode == 0xD6)
	{
		Value = CPU->NextByte();
	}
	else
	{
		switch (Low)
		{
		case 0x7: Value = CPU->a; break;
		case 0x0: Value = CPU->b; break;
		case 0x1: Value = CPU->c; break;
		case 0x2: Value = CPU->d; break;
		case 0x3: Value = CPU->e; break;
		case 0x4: Value = CPU->h; break;
		case 0x5: Value = CPU->l; break;
		case 0x6: Value = CPU->MemoryController[CPU->hl()]; break;
		default: break;
		}
	}
	
	u32 Result = (u32)CPU->a - (u32)Value;
	u32 CarryBits = (u32)CPU->a ^ Value ^ Result;
	CPU->ClearFlags();
	if ((CarryBits & 0x100) != 0) CPU->ToggleFlagC();
	if ((CarryBits & 0x10)  != 0) CPU->ToggleFlagH();
	if ((Result & 0xFF)     == 0) CPU->ToggleFlagZ();
	CPU->a = Result & 0xFF;
	return 4;
}

INSTRUCTION(SBC_A_n)
{
	u8 Low = OpCode & 0x0F;
	u8 Value = 0;
	
	if (OpCode == 0xDE)
	{
		Value = CPU->NextByte();
	}
	else
	{
		switch (Low)
		{
			case 0xF: Value = CPU->a; break;
			case 0x8: Value = CPU->b; break;
			case 0x9: Value = CPU->c; break;
			case 0xA: Value = CPU->d; break;
			case 0xB: Value = CPU->e; break;
			case 0xC: Value = CPU->h; break;
			case 0xD: Value = CPU->l; break;
			case 0xE: Value = CPU->MemoryController[CPU->hl()]; break;
			default: break;
		}
	}
	
	u32 Carry = (CPU->GetFlagC() ? 1 : 0);
	u32 Result = (u32)CPU->a - Value - Carry;
	CPU->ClearFlags();
	if ((Result & 0xFF) == 0) CPU->ToggleFlagZ();
	if ((Result & 0xFFFF) > 0xFF) CPU->ToggleFlagC();
	if ((Result & 0xFF) > 0xF) CPU->ToggleFlagH();
	return 4;
}

INSTRUCTION(AND_n)
{
	return 4;
}

INSTRUCTION(OR_n)
{
	return 4;
}

INSTRUCTION(XOR_n)
{
	return 4;
}

INSTRUCTION(CP_n)
{
	return 4;
}

INSTRUCTION(INC_n)
{
	return 4;
}

INSTRUCTION(DEC_n)
{
	return 4;
}

INSTRUCTION(ADD_HL_n)
{
	return 8;
}

INSTRUCTION(ADD_SP_n)
{
	return 16;
}

INSTRUCTION(INC_nn)
{
	return 8;
}

INSTRUCTION(DEC_nn)
{
	return 8;
}

INSTRUCTION(SWAP_n)
{
	return 8;
}

INSTRUCTION(DAA)
{
	return 4;
}

INSTRUCTION(CPL)
{
	return 4;
}

INSTRUCTION(CCF)
{
	return 4;
}

INSTRUCTION(SCF)
{
	return 4;
}

INSTRUCTION(NOP)
{
	return 4;
}

INSTRUCTION(HALT)
{
	return 4;
}

INSTRUCTION(STOP)
{
	return 4;
}

INSTRUCTION(DI)
{
	return 4;
}

INSTRUCTION(EI)
{
	return 4;
}

INSTRUCTION(RLCA)
{
	return 4;
}

INSTRUCTION(RLA)
{
	return 4;
}

INSTRUCTION(RRCA)
{
	return 4;
}

INSTRUCTION(RRA)
{
	return 4;
}

INSTRUCTION(RLC_n)
{
	return 8;
}

INSTRUCTION(RL_n)
{
	return 8;
}

INSTRUCTION(RRC_n)
{
	return 8;
}

INSTRUCTION(RR_n)
{
	return 8;
}

INSTRUCTION(SLA_n)
{
	return 8;
}

INSTRUCTION(SRA_n)
{
	return 8;
}

INSTRUCTION(SRL_n)
{
	return 8;
}

INSTRUCTION(BIT_b_r)
{
	return 8;
}

INSTRUCTION(SET_b_r)
{
	return 8;
}

INSTRUCTION(RES_b_r)
{
	return 8;
}

INSTRUCTION(JP_nn)
{
	return 12;
}

INSTRUCTION(JP_cc_nn)
{
	return 12;
}

INSTRUCTION(JP_PHL)
{
	return 4;
}

INSTRUCTION(JR_n)
{
	return 8;
}

INSTRUCTION(JR_cc_n)
{
	return 8;
}

INSTRUCTION(CALL_nn)
{
	return 12;
}

INSTRUCTION(CALL_cc_nn)
{
	return 12;
}

INSTRUCTION(RST_n)
{
	return 32;
}

INSTRUCTION(RET)
{
	return 8;
}

INSTRUCTION(RET_cc)
{
	return 8;
}

INSTRUCTION(RETI)
{
	return 8;
}

INSTRUCTION(STUB) { cout << "Stubbed: $" << hex << OpCode << ". Stopping." << endl << "PC: $" << CPU->pc << endl << dec; CPU->Stopped = true; return 4; }

#undef INSTRUCTION

FCPU::FCPU(shared_ptr<FCart> Cartridge) : Cartridge(Cartridge), MemoryController(Cartridge)
{
}

FCPU::~FCPU()
{
}

void FCPU::Init()
{
	a = 0;
	b = 0;
	c = 0;
	d = 0;
	e = 0;
	f = 0;
	h = 0;
	l = 0;

	pc = 0x100;
	sp = 0;

	TotalInstructionsRun = 0;
	Stopped = false;
}

void FCPU::Step()
{
	u8 Instruction = MemoryController[pc];

	if (Instruction == 0x00) { // NOP
		// nop, continue
		pc += 1;
		return;
	}

	// ???
	pc += 1;
	return;
}

void FCPU::Execute(const u32 ExecCycles)
{
	u32 Cycles = 0;

	while (Cycles < ExecCycles)
	{
		u8 Instruction = MemoryController[pc];

		cout << "$" << hex << ((u16)Instruction & 0x00FF) << dec << endl;

		if (Stopped) break;

#define C(op) Cycles += op(This(), Instruction)

		switch ((u16)Instruction)
		{
		case 0x00: C(NOP); break;
		case 0x01: C(LD_sn_snn); break;
		case 0x02: C(LD_n_A); break;
		case 0x03: C(STUB); break;
		case 0x04: C(STUB); break;
		case 0x05: C(STUB); break;
		case 0x06: C(LD_nn_n); break;
		case 0x07: C(STUB); break;
		case 0x08: C(LD_Pnn_SP); break;
		case 0x09: C(STUB); break;
		case 0x0A: C(LD_A_n); break;
		case 0x0B: C(STUB); break;
		case 0x0C: C(STUB); break;
		case 0x0D: C(STUB); break;
		case 0x0E: C(LD_nn_n); break;
		case 0x0F: C(STUB); break;

		case 0x10: C(STUB); break;
		case 0x11: C(LD_sn_snn); break;
		case 0x12: C(LD_n_A); break;
		case 0x13: C(STUB); break;
		case 0x14: C(STUB); break;
		case 0x15: C(STUB); break;
		case 0x16: C(LD_nn_n); break;
		case 0x17: C(STUB); break;
		case 0x18: C(STUB); break;
		case 0x19: C(STUB); break;
		case 0x1A: C(LD_A_n); break;
		case 0x1B: C(STUB); break;
		case 0x1C: C(STUB); break;
		case 0x1D: C(STUB); break;
		case 0x1E: C(LD_nn_n); break;
		case 0x1F: C(STUB); break;

		case 0x20: C(STUB); break;
		case 0x21: C(LD_sn_snn); break;
		case 0x22: C(LD_PHLI_A); break;
		case 0x23: C(STUB); break;
		case 0x24: C(STUB); break;
		case 0x25: C(STUB); break;
		case 0x26: C(LD_nn_n); break;
		case 0x27: C(STUB); break;
		case 0x28: C(STUB); break;
		case 0x29: C(STUB); break;
		case 0x2A: C(LD_A_PHLI); break;
		case 0x2B: C(STUB); break;
		case 0x2C: C(STUB); break;
		case 0x2D: C(STUB); break;
		case 0x2E: C(LD_nn_n); break;
		case 0x2F: C(STUB); break;

		case 0x30: C(STUB); break;
		case 0x31: C(LD_sn_snn); break;
		case 0x32: C(LD_PHLD_A); break;
		case 0x33: C(STUB); break;
		case 0x34: C(STUB); break;
		case 0x35: C(STUB); break;
		case 0x36: C(LD_r1_r2); break;
		case 0x37: C(STUB); break;
		case 0x38: C(STUB); break;
		case 0x39: C(STUB); break;
		case 0x3A: C(LD_A_PHLD); break;
		case 0x3B: C(STUB); break;
		case 0x3C: C(STUB); break;
		case 0x3D: C(STUB); break;
		case 0x3E: C(LD_A_n); break;
		case 0x3F: C(STUB); break;

		case 0x40: C(LD_r1_r2); break;
		case 0x41: C(LD_r1_r2); break;
		case 0x42: C(LD_r1_r2); break;
		case 0x43: C(LD_r1_r2); break;
		case 0x44: C(LD_r1_r2); break;
		case 0x45: C(LD_r1_r2); break;
		case 0x46: C(LD_r1_r2); break;
		case 0x47: C(LD_n_A); break;
		case 0x48: C(LD_r1_r2); break;
		case 0x49: C(LD_r1_r2); break;
		case 0x4A: C(LD_r1_r2); break;
		case 0x4B: C(LD_r1_r2); break;
		case 0x4C: C(LD_r1_r2); break;
		case 0x4D: C(LD_r1_r2); break;
		case 0x4E: C(LD_r1_r2); break;
		case 0x4F: C(LD_n_A); break;

		case 0x50: C(LD_r1_r2); break;
		case 0x51: C(LD_r1_r2); break;
		case 0x52: C(LD_r1_r2); break;
		case 0x53: C(LD_r1_r2); break;
		case 0x54: C(LD_r1_r2); break;
		case 0x55: C(LD_r1_r2); break;
		case 0x56: C(LD_r1_r2); break;
		case 0x57: C(LD_n_A); break;
		case 0x58: C(LD_r1_r2); break;
		case 0x59: C(LD_r1_r2); break;
		case 0x5A: C(LD_r1_r2); break;
		case 0x5B: C(LD_r1_r2); break;
		case 0x5C: C(LD_r1_r2); break;
		case 0x5D: C(LD_r1_r2); break;
		case 0x5E: C(LD_r1_r2); break;
		case 0x5F: C(LD_n_A); break;

		case 0x60: C(LD_r1_r2); break;
		case 0x61: C(LD_r1_r2); break;
		case 0x62: C(LD_r1_r2); break;
		case 0x63: C(LD_r1_r2); break;
		case 0x64: C(LD_r1_r2); break;
		case 0x65: C(LD_r1_r2); break;
		case 0x66: C(LD_r1_r2); break;
		case 0x67: C(LD_n_A); break;
		case 0x68: C(LD_r1_r2); break;
		case 0x69: C(LD_r1_r2); break;
		case 0x6A: C(LD_r1_r2); break;
		case 0x6B: C(LD_r1_r2); break;
		case 0x6C: C(LD_r1_r2); break;
		case 0x6D: C(LD_r1_r2); break;
		case 0x6E: C(LD_r1_r2); break;
		case 0x6F: C(LD_n_A); break;

		case 0x70: C(LD_r1_r2); break;
		case 0x71: C(LD_r1_r2); break;
		case 0x72: C(LD_r1_r2); break;
		case 0x73: C(LD_r1_r2); break;
		case 0x74: C(LD_r1_r2); break;
		case 0x75: C(LD_r1_r2); break;
		case 0x76: C(STUB); break;
		case 0x77: C(LD_n_A); break;
		case 0x78: C(LD_r1_r2); break;
		case 0x79: C(LD_r1_r2); break;
		case 0x7A: C(LD_r1_r2); break;
		case 0x7B: C(LD_r1_r2); break;
		case 0x7C: C(LD_r1_r2); break;
		case 0x7D: C(LD_r1_r2); break;
		case 0x7E: C(LD_r1_r2); break;
		case 0x7F: C(LD_n_A); break;

		case 0x80: C(ADD_A_n); break;
		case 0x81: C(ADD_A_n); break;
		case 0x82: C(ADD_A_n); break;
		case 0x83: C(ADD_A_n); break;
		case 0x84: C(ADD_A_n); break;
		case 0x85: C(ADD_A_n); break;
		case 0x86: C(ADD_A_n); break;
		case 0x87: C(ADD_A_n); break;
		case 0x88: C(ADC_A_n); break;
		case 0x89: C(ADC_A_n); break;
		case 0x8A: C(ADC_A_n); break;
		case 0x8B: C(ADC_A_n); break;
		case 0x8C: C(ADC_A_n); break;
		case 0x8D: C(ADC_A_n); break;
		case 0x8E: C(ADC_A_n); break;
		case 0x8F: C(ADC_A_n); break;

		case 0x90: C(SUB_n); break;
		case 0x91: C(SUB_n); break;
		case 0x92: C(SUB_n); break;
		case 0x93: C(SUB_n); break;
		case 0x94: C(SUB_n); break;
		case 0x95: C(SUB_n); break;
		case 0x96: C(SUB_n); break;
		case 0x97: C(SUB_n); break;
		case 0x98: C(SBC_A_n); break;
		case 0x99: C(SBC_A_n); break;
		case 0x9A: C(SBC_A_n); break;
		case 0x9B: C(SBC_A_n); break;
		case 0x9C: C(SBC_A_n); break;
		case 0x9D: C(SBC_A_n); break;
		case 0x9E: C(SBC_A_n); break;
		case 0x9F: C(SBC_A_n); break;

		case 0xA0: C(STUB); break;
		case 0xA1: C(STUB); break;
		case 0xA2: C(STUB); break;
		case 0xA3: C(STUB); break;
		case 0xA4: C(STUB); break;
		case 0xA5: C(STUB); break;
		case 0xA6: C(STUB); break;
		case 0xA7: C(STUB); break;
		case 0xA8: C(STUB); break;
		case 0xA9: C(STUB); break;
		case 0xAA: C(STUB); break;
		case 0xAB: C(STUB); break;
		case 0xAC: C(STUB); break;
		case 0xAD: C(STUB); break;
		case 0xAE: C(STUB); break;
		case 0xAF: C(STUB); break;

		case 0xB0: C(STUB); break;
		case 0xB1: C(STUB); break;
		case 0xB2: C(STUB); break;
		case 0xB3: C(STUB); break;
		case 0xB4: C(STUB); break;
		case 0xB5: C(STUB); break;
		case 0xB6: C(STUB); break;
		case 0xB7: C(STUB); break;
		case 0xB8: C(STUB); break;
		case 0xB9: C(STUB); break;
		case 0xBA: C(STUB); break;
		case 0xBB: C(STUB); break;
		case 0xBC: C(STUB); break;
		case 0xBD: C(STUB); break;
		case 0xBE: C(STUB); break;
		case 0xBF: C(STUB); break;

		case 0xC0: C(STUB); break;
		case 0xC1: C(POP_nn); break;
		case 0xC2: C(STUB); break;
		case 0xC3: C(STUB); break;
		case 0xC4: C(STUB); break;
		case 0xC5: C(PUSH_nn); break;
		case 0xC6: C(ADD_A_n); break;
		case 0xC7: C(STUB); break;
		case 0xC8: C(STUB); break;
		case 0xC9: C(STUB); break;
		case 0xCA: C(STUB); break;
		case 0xCB: C(STUB); break; // Extended Operations
		case 0xCC: C(STUB); break;
		case 0xCD: C(STUB); break;
		case 0xCE: C(ADC_A_n); break;
		case 0xCF: C(STUB); break;

		case 0xD0: C(STUB); break;
		case 0xD1: C(POP_nn); break;
		case 0xD2: C(STUB); break;
		case 0xD3: C(STUB); break;
		case 0xD4: C(STUB); break;
		case 0xD5: C(PUSH_nn); break;
		case 0xD6: C(SUB_n); break;
		case 0xD7: C(STUB); break;
		case 0xD8: C(STUB); break;
		case 0xD9: C(STUB); break;
		case 0xDA: C(STUB); break;
		case 0xDB: C(STUB); break;
		case 0xDC: C(STUB); break;
		case 0xDD: C(STUB); break;
		case 0xDE: C(SBC_A_n); break;
		case 0xDF: C(STUB); break;

		case 0xE0: C(LDH_Pn_A); break;
		case 0xE1: C(POP_nn); break;
		case 0xE2: C(LD_PC_A); break;
		case 0xE3: C(STUB); break;
		case 0xE4: C(STUB); break;
		case 0xE5: C(PUSH_nn); break;
		case 0xE6: C(STUB); break;
		case 0xE7: C(STUB); break;
		case 0xE8: C(STUB); break;
		case 0xE9: C(STUB); break;
		case 0xEA: C(LD_n_A); break;
		case 0xEB: C(STUB); break;
		case 0xEC: C(STUB); break;
		case 0xED: C(STUB); break;
		case 0xEE: C(STUB); break;
		case 0xEF: C(STUB); break;

		case 0xF0: C(LDH_A_Pn); break;
		case 0xF1: C(POP_nn); break;
		case 0xF2: C(LD_A_PC); break;
		case 0xF3: C(STUB); break;
		case 0xF4: C(STUB); break;
		case 0xF5: C(PUSH_nn); break;
		case 0xF6: C(STUB); break;
		case 0xF7: C(STUB); break;
		case 0xF8: C(LD_SP_n); break;
		case 0xF9: C(LD_SP_HL); break;
		case 0xFA: C(LD_A_n); break;
		case 0xFB: C(STUB); break;
		case 0xFC: C(STUB); break;
		case 0xFD: C(STUB); break;
		case 0xFE: C(STUB); break;
		case 0xFF: C(STUB); break;
		}

#undef C

		TotalInstructionsRun += 1;
		pc += 1;
	}
}

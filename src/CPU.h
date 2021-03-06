#pragma once

#include "common.h"

#include "Cart.h"
#include "MemoryController.h"

class FCPU : public std::enable_shared_from_this<FCPU>
{
private:

public:
	typedef FCPU StaticClass;
	std::shared_ptr<StaticClass> This() { return shared_from_this(); }

	FCPU(std::shared_ptr<FCart> Cartridge);
	virtual ~FCPU();

	std::shared_ptr<FCart> Cartridge;
	FMemoryController MemoryController;

	u8 a = 0;
	u8 f = 0;
	u8 b = 0;
	u8 c = 0;
	u8 d = 0;
	u8 e = 0;
	u8 h = 0;
	u8 l = 0;

	u16 sp = 0;
	u16 pc = 0;

	u32 TotalInstructionsRun = 0;

	u16 bc() const { return ((u16)b << 8) + c; }
	u16 de() const { return ((u16)d << 8) + e; }
	u16 hl() const { return ((u16)h << 8) + l; }
	u16 af() const { return ((u16)a << 8) + f; }
	void bc(u16 v) { b = (v & 0xFF00) >> 8; c = v & 0x00FF; }
	void de(u16 v) { d = (v & 0xFF00) >> 8; e = v & 0x00FF; }
	void hl(u16 v) { h = (v & 0xFF00) >> 8; l = v & 0x00FF; }
	void af(u16 v) { a = (v & 0xFF00) >> 8; f = v & 0x00FF; }

	bool Stopped = false;
	
	u8 NextByte() { return MemoryController[++pc]; }
	u16 NextWord() { pc += 2; return ((u16)MemoryController[pc - 1]) + ((u16)MemoryController[pc] << 8); }

	bool GetFlagZ() const { return (f & 64) > 0; }
	bool GetFlagC() const { return (f & 32) > 0; }
	bool GetFlagH() const { return (f & 16) > 0; }
	bool GetFlagN() const { return (f & 8) > 0; }
	void ClearFlags() { f = 0; }
	void ToggleFlagZ() { f ^= 64; }
	void ToggleFlagC() { f ^= 32; }
	void ToggleFlagH() { f ^= 16; }
	void ToggleFlagN() { f ^= 8; }

	void Init();
	void Step();
	void Execute(const u32 ExecCycles);
};

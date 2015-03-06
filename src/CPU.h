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

	u16 hl() const { return ((u16)h << 8) + l; }
	u16 af() const { return ((u16)a << 8) + f; }
	void hl(u16 v) { h = (v & 0xFF00) >> 8; l = v & 0x00FF; }
	void af(u16 v) { a = (v & 0xFF00) >> 8; f = v & 0x00FF; }

	bool Stopped = false;

	bool GetFlagZ() { return (f & 64) > 0; }
	bool GetFlagC() { return (f & 32) > 0; }
	bool GetFlagH() { return (f & 16) > 0; }
	bool GetFlagN() { return (f & 8) > 0; }

	void Init();
	void Step();
	void Execute(const u32 ExecCycles);
};

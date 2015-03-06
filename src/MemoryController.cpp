#include "MemoryController.h"

#include "CartridgeException.h"

using namespace std;
using namespace AngryBoy;

FMemoryController::FMemoryController(shared_ptr<FCart> InCartridge)
{
	Cartridge = InCartridge;

	VRAM = new u8[VRAMSize];
	WRAM = new u8[WRAMSize];

	// figure out eram size from cartridge
	ERAMBanks = Cartridge->GetCartHeader()->RAMSize == 3 ? 4 : 1;
	switch (Cartridge->GetCartHeader()->RAMSize)
	{
	case 0:
		ERAMBanks = 1;
		ERAMSize = (2 * 1024);
		break;
	case 1:
		ERAMBanks = 1;
		ERAMSize = (8 * 1024);
		break;
	case 2:
		ERAMBanks = 4;
		ERAMSize = (32 * 1024);
		break;
	default:
		throw FCartridgeException("Invalid ERAM banks size in cartridge header");
	}

	ERAM = new u8[ERAMSize];
	Init();
}

FMemoryController::~FMemoryController()
{
	if (VRAM != nullptr)
	{
		delete[] VRAM;
	}

	if (ERAM != nullptr)
	{
		delete[] ERAM;
	}

	if (WRAM != nullptr)
	{
		delete[] WRAM;
	}
}

void FMemoryController::Init()
{
	CurrentERAMBank = 0;
	CurrentVRAMBank = 0;
	CurrentWRAMBank = 1;
	CurrentROMBank  = 1;
	InterruptEnableRegister = 1;

	memset(VRAM, 0, VRAMSize);
	// we don't reset ERAM because it comes from cartridge, but the bank is reset
	memset(WRAM, 0, WRAMSize);
	memset(IOREG, 0, sizeof(IOREG));
	memset(HRAM, 0, sizeof(HRAM));
}

void FMemoryController::SetByte(const u16 Location, const u8 Value)
{
	Byte(Location) = Value;
}

u8& FMemoryController::Byte(const u16 Location)
{
	static u8 invalid = 0;
	invalid = 0;

	// TODO make this faster...
	if (BETWEEN(Location, MEMR_ROM0, MEMR_ROMN))
	{
		return Cartridge->ROM[Location];
	}
	if (BETWEEN(Location, MEMR_ROMN, MEMR_VRAM))
	{
		u32 LocationOffset = Location - MEMR_ROMN + CurrentROMBank * (8 * 1024);
		return Cartridge->ROM[LocationOffset];
	}
	if (BETWEEN(Location, MEMR_VRAM, MEMR_ERAM))
	{
		u32 LocationOffset = Location - MEMR_VRAM + CurrentVRAMBank * (8 * 1024);
		return VRAM[LocationOffset];
	}
	if (BETWEEN(Location, MEMR_ERAM, MEMR_WRAM0))
	{
		u32 LocationOffset = Location - MEMR_ERAM;
	}
	if (BETWEEN(Location, MEMR_WRAM0, MEMR_WRAMN))
	{
		u32 LocationOffset = Location - MEMR_WRAM0;
		return WRAM[LocationOffset];
	}
	if (BETWEEN(Location, MEMR_WRAMN, MEMR_MIRR))
	{
		// banked work ram is only available on gbc
		if (!IsGBC())
		{
			return invalid; // TODO what does a real Gameboy do?
		}
		u32 LocationOffset = Location - MEMR_WRAMN + CurrentWRAMBank * (4 * 1024);
		return WRAM[LocationOffset];
	}
	if (BETWEEN(Location, MEMR_MIRR, MEMR_OAM))
	{
		// TODO make !gbc not able to read from wram bank
		// work ram mirror
		u32 LocationOffset = Location - MEMR_MIRR + (Location >= MEMR_MIRR + (4 * 1024) ? CurrentWRAMBank * (4 * 1024) : 0);
		return WRAM[LocationOffset];
	}
	if (BETWEEN(Location, MEMR_OAM, MEMR_UNUS))
	{
		u32 LocationOffset = Location - MEMR_OAM;
		u8 * const OAMMem = (u8 * const)OAM;
		return OAMMem[LocationOffset];
	}
	if (BETWEEN(Location, MEMR_UNUS, MEMR_IOREG))
	{
		return invalid; // What does a real gameboy do?
	}
	if (BETWEEN(Location, MEMR_IOREG, MEMR_HRAM))
	{
		return invalid; // SOMETHING???
	}
	if (BETWEEN(Location, MEMR_HRAM, MEMR_INTE))
	{
		u32 LocationOffset = Location - MEMR_HRAM;
		return HRAM[LocationOffset];
	}
	if (Location == MEMR_INTE)
	{
		return InterruptEnableRegister;
	}

	return invalid;
}

u8& FMemoryController::operator[](const u16 Location)
{
	return Byte(Location);
}

u16 FMemoryController::GetWord(const u16 Location)
{
	u16 Lower = Byte(Location);
	u16 Upper = Byte(Location + 1) << 8;
	return Lower + Upper;
}

void FMemoryController::SetWord(const u16 Location, const u16 Value)
{
	u8 Lower = Value & 0x00FF;
	u8 Upper = Value & 0xFF00;
	Byte(Location) = Lower;
	Byte(Location) = Upper;
}
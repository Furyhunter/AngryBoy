#pragma once

#include <memory>

#include "common.h"

#include "Cart.h"

/* Memory map */
// Rom fixed bank (from FCart)
#define MEMR_ROM0  (u16)0x0000
// Rom switchable bank
#define MEMR_ROMN  (u16)0x4000
// Vram (GBC)
#define MEMR_VRAM  (u16)0x8000
// Cartridge ram
#define MEMR_ERAM  (u16)0xA000
// Work RAM bank 0
#define MEMR_WRAM0 (u16)0xC000
// Work RAM bank 1-7 (GBC)
#define MEMR_WRAMN (u16)0xD000
// Mirror of 0xC000-0xDDFF (echo)
#define MEMR_MIRR  (u16)0xC000
// Sprite attr table (OAM)
#define MEMR_OAM   (u16)0xFE00
// Unused (disallow r/w?)
#define MEMR_UNUS  (u16)0xFEA0
// I/O registers
#define MEMR_IOREG (u16)0xFF00
// High ram
#define MEMR_HRAM  (u16)0xFF80
// Interrupt enable register
#define MEMR_INTE  (u16)0xFFFF

struct FOAMEntry
{
	u8 YPosition;
	u8 XPosition;
	u8 Pattern;
	u8 Flags;

	bool Priority() const { return (Flags & 64) > 0; }
	bool YFlip() const { return (Flags & 32) > 0; }
	bool XFlip() const { return (Flags & 16) > 0; }
	bool PaletteNumber() const { return (Flags & 8) > 0; }
	bool TileBank() const { return (Flags & 4) > 0; }
	u8 GBCPaletteNumber() const { return (Flags & 3); }
};

class FMemoryController
{
private:
	u8 VRAMBanks = 2;
	u8 * VRAM = nullptr;
	u32 VRAMSize = (8 * 1024) * 2;

	u16 ERAMBanks = 0;
	u8 * ERAM = nullptr;
	u32 ERAMSize;

	u8 WRAMBanks = 8;
	u8 * WRAM = nullptr;
	u32 WRAMSize = (4 * 1024) * 8;

	u8 IOREG[0x7F];
	u8 HRAM[0x7E];

	u8 InterruptEnableRegister = 1;

	FOAMEntry OAM[40];

	// Values to be initialized on reset
	u8 CurrentVRAMBank = 0;
	u8 CurrentERAMBank = 0;
	u8 CurrentWRAMBank = 1;
	u8 CurrentROMBank  = 1;

	std::shared_ptr<FCart> Cartridge;

	bool GBC;

public:
	FMemoryController(std::shared_ptr<FCart> InCartridge);
	virtual ~FMemoryController();

	void SetGBC(const bool InGBC) { GBC = InGBC; }
	bool IsGBC() const { return GBC; }

	void Init();

	void SetByte(const u16 Location, const u8 Value);
	u8& Byte(const u16 Location);

	u8& operator[](const u16 Location);

	u8& LCDControl() { return IOREG[0xFF40 - MEMR_IOREG]; }
	u8& LCDStatus() { return IOREG[0xFF41 - MEMR_IOREG]; }
	u8& LCDSCY() { return IOREG[0xFF42 - MEMR_IOREG]; }
	u8& LCDSCX() { return IOREG[0xFF43 - MEMR_IOREG]; }
	u8& LCDYCoord() { return IOREG[0xFF44 - MEMR_IOREG]; }
	u8& LCDLYCompare() { return IOREG[0xFF45 - MEMR_IOREG]; }

	bool IsLCDDisplayEnabled()        { return (LCDControl() & 128) > 0; }
	bool WindowTileMapDisplaySelect() { return (LCDControl() & 64) > 0; }
	bool IsWindowDisplayEnabled()     { return (LCDControl() & 32) > 0; }
	bool BGWindowTileDataSelect()     { return (LCDControl() & 16) > 0; }
	bool BGTileMapDisplaySelect()     { return (LCDControl() & 8) > 0; }
	bool OBJSize()                    { return (LCDControl() & 4) > 0; }
	bool OBJDisplayEnable()           { return (LCDControl() & 2) > 0; }
	bool BGDisplay()                  { return (LCDControl() & 1) > 0; } // meaning is different depending on gameboy type (GB, GBC, GBC as GB)

	bool LYCEqualYCoincidenceInterrupt() { return (LCDStatus() & 128) > 0; }
	// TODO more

	u16 GetWord(const u16 Location);
	void SetWord(const u16 Location, const u16 Value);
};
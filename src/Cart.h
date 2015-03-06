#pragma once

#include <string>

#include "common.h"

#define CART_HEADER 0x0100

#pragma pack(push, 1)
struct FCartHeader
{
	u32 EntryPoint;
	u8  CompanyLogo[0x30];
	u8  Title[11];
	u8  ManufacturerCode[4];
	u8  GBCFlag; // 0x80 -- GB, 0xC0 -- GBC
	u8  LicenseeCode[2];
	u8  SGBFlag; // 0x00 -- Normal, 0x03 -- SGB
	u8  CartType;
	u8  ROMSize;
	u8  RAMSize; // 0 - 0, 1 - 2KB, 2 - 8KB, 3 - 32KB (4 banks 8K each)
	u8  DestCode; // 0 - Japan, 1 - International
	u8  OldLicenseeCode; // 0x33 means use LicenseeCode above (SGB doesn't work != 0x33)
	u8  VersionNum;
	u8  HeaderChecksum;
	u16 ROMChecksum;
};
#pragma pack(pop)

struct FCart
{
	u8 * ROM = nullptr;
	u32 ROMSize = 0;
	u16 ROMBanks = 0;

public:
	FCart();
	FCart(std::string Path);
	FCart(FCart&& In);
	virtual ~FCart();

	bool ChecksumHeader() const;
	bool ChecksumROM() const;
	void InitializeROMArray(u8 InROMSize);

	/** Retrieves a pointer to the cartridge header. */
	FCartHeader* GetCartHeader();
};

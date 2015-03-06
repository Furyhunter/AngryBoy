#include "Cart.h"

#include <iostream>
#include <fstream>
#include <memory>

#include "CPU.h"
#include "CartridgeException.h"

using namespace std;
using namespace AngryBoy;

FCart::FCart()
{
	ROMSize = 0x4000; // 16k rom by default or whatever
	ROM = new u8[ROMSize];
}

FCart::FCart(string Path)
{
	ifstream file(Path, ifstream::binary);
	if (!file)
	{
		throw FCartridgeException("File not found: "s + Path);
	}
	u8* Buf = new u8[0x150]; // for getting the initial header
	FCartHeader* Header = reinterpret_cast<FCartHeader*>(Buf+0x100);
	file.read(Buf, 0x150);
	file.seekg(0);

	InitializeROMArray(Header->ROMSize);

	file.read(ROM, ROMSize);
	if (file.gcount() != ROMSize)
	{
		file.close();

		throw FCartridgeException("Incorrect file size for "s + Path + ": Got " + to_string(file.gcount()) + ", expected " + to_string(ROMSize));
	}

	file.close();
	delete[] Buf;
}

FCart::FCart(FCart&& In)
{
	ROMSize = In.ROMSize;
	ROM = In.ROM;
	ROMBanks = In.ROMBanks;

	In.ROM = nullptr;
}

void FCart::InitializeROMArray(u8 InROMSize)
{
	switch (InROMSize)
	{
	case 0x0:
		ROMBanks = 0;
		break;
	case 0x1:
		ROMBanks = 4;
		break;
	case 0x2:
		ROMBanks = 8;
		break;
	case 0x3:
		ROMBanks = 16;
		break;
	case 0x4:
		ROMBanks = 32;
		break;
	case 0x5:
		ROMBanks = 64;
		break;
	case 0x6:
		ROMBanks = 128;
		break;
	case 0x7:
		ROMBanks = 256;
		break;
	case 0x52:
		ROMBanks = 72;
	case 0x53:
		ROMBanks = 80;
		break;
	case 0x54:
		ROMBanks = 96;
		break;
	default:
		throw exception();
	}

	ROMSize = (32 * 1024) + ROMBanks * (8 * 1024);

	ROM = new u8[ROMSize];
}

bool FCart::ChecksumHeader() const
{
	// STUB
	return true;
}

bool FCart::ChecksumROM() const
{
	// STUB
	return true;
}

FCart::~FCart()
{
	if (ROM != nullptr)
	{
		delete[] ROM;
	}
}

FCartHeader* FCart::GetCartHeader()
{
	return reinterpret_cast<FCartHeader*>(&ROM[CART_HEADER]);
}

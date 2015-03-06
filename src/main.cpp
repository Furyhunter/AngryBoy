#include <iostream>
#include <memory>

#include "CPU.h"
#include "MemoryController.h"
#include "Exception.h"

using namespace std;
using namespace AngryBoy;

static bool running = true;

int WrappedMain(int argc, char** argv)
{
	cout << "Loading cartridge C:\\Emulation\\VBA-M\\Rom\\Metroid II - Return of Samus (World).gb" << endl;
	shared_ptr<FCart> Cart(new FCart("C:\\Emulation\\VBA-M\\Rom\\Metroid II - Return of Samus (World).gb"));
	cout << "Initializing emulator" << endl;
	shared_ptr<FCPU> CPU(new FCPU(Cart));

	cout << "Title: " << string(Cart->GetCartHeader()->Title, 16) << endl;
	cout << "Version: " << to_string(Cart->GetCartHeader()->VersionNum) << endl;
	cout << "Publisher: " << to_string((u16)*Cart->GetCartHeader()->LicenseeCode) << endl;
	cout << "ROM Size: " << to_string(Cart->GetCartHeader()->ROMSize) << endl;
	cout << "ERAM size: " << to_string(Cart->GetCartHeader()->RAMSize) << endl;

	cout << endl;

	CPU->Init();

	// Begin loop
	while (running)
	{
		if (CPU->Stopped)
		{
			running = false;
			break;
		}
		CPU->Execute(70224);
	}

	return 0;
}

int main(int argc, char** argv)
{
	try
	{
		return WrappedMain(argc, argv);
	}
	catch (FException ex)
	{
		cout << "FException: " << ex.Message() << endl;
		return -1;
	}
	catch (std::exception ex)
	{
		cout << "std::exception: " << ex.what() << endl;
		return -1;
	}
	return 0;
}
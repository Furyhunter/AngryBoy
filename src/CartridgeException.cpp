#include "CartridgeException.h"

using namespace AngryBoy;

FCartridgeException::FCartridgeException(std::string Message) : Super(Message)
{

}

FCartridgeException::FCartridgeException(const FCartridgeException& Copy) : Super(Copy)
{

}

FCartridgeException::~FCartridgeException() throw()
{
	Super::~Super();
}

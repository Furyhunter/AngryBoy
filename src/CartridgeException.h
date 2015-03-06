#pragma once

#include "Exception.h"

namespace AngryBoy
{
	class FCartridgeException : public FException
	{
	public:
		typedef AngryBoy::FException Super;

		FCartridgeException(std::string Message);
		FCartridgeException(const FCartridgeException & Copy);
		virtual ~FCartridgeException();
	};
}

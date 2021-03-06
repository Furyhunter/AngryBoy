#include "Exception.h"

#include <iostream>

using namespace AngryBoy;
using namespace std;

FException::FException() : MessageString("")
{

}

FException::FException(const FException & Copy) : MessageString(Copy.MessageString)
{

}

FException::FException(std::string InMessage) : MessageString(InMessage)
{

}

FException::~FException() throw()
{
}

string FException::Message() const
{
	return MessageString;
}

const char * FException::what() const throw()
{
	return MessageString.c_str();
}

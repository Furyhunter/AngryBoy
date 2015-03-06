#pragma once

#include <exception>
#include <string>

namespace AngryBoy
{
	class FException : public std::exception
	{
	protected:
		std::string MessageString;
	public:
		typedef std::exception Super;

		FException();
		FException(const FException & Copy);
		FException(std::string InMessage);
		virtual ~FException() throw();
		virtual std::string Message() const;

		// begin std::exception
		virtual const char* what() const throw() override;
	};
}
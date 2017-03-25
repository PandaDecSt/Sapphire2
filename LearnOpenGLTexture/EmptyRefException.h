#pragma once

#include <exception>
#include <string>

namespace Sapphire
{
	class EmptyRefException : public std::exception
	{
	public:
		explicit EmptyRefException(const char *reason):exception(reason)  //��ֹ��ʽ����
		{

		}
		~EmptyRefException() throw()
		{}
		const char *what() const throw() { return exception.c_str(); }
	private:
		std::string exception;

	};
}

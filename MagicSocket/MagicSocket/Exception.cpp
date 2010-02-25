#include "Exception.h"
#include <string>
#include <sstream>

namespace Magic {
	Win32Exception::Win32Exception(const std::string &apiName, DWORD dwErrCode)
	{
		std::ostringstream oss;
		char *buffer;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, dwErrCode, 0, (LPSTR)&buffer, 0, 0);
		oss << "Win32 Err " << dwErrCode << " at " << apiName << ": " << buffer;
		LocalFree(buffer);
		s = oss.str();
	}

	Win32Exception::~Win32Exception()
	{
	}

	const char * Win32Exception::what() const
	{
		return s.c_str();
	}
}
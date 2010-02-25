#pragma once

#include "Platform.h"
#include <stdexcept>
#include <string>

namespace Magic {
	class Win32Exception : public std::exception {
	public:
		Win32Exception(const std::string &apiName, DWORD dwErrCode = GetLastError());
		~Win32Exception();

		const char * what() const;
	private:
		std::string s;
	};
}
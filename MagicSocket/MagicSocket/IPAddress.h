#pragma once

#include "Platform.h"
#include <string>

namespace Magic {
	class IPAddress {
	public:
		IPAddress(u_long address);
		u_long Address();
		bool operator ==(IPAddress ipaddr);
		bool operator !=(IPAddress ipaddr);
		static IPAddress Parse(const std::string &s);
	private:
		u_long address;
	};
}
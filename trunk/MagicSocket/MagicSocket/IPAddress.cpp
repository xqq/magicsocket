#include "Platform.h"
#include "IPAddress.h"
#include <string>

namespace Magic {
	IPAddress::IPAddress(u_long address)
		: address(address)
	{
	}

	u_long IPAddress::Address()
	{
		return address;
	}

	bool IPAddress::operator ==(IPAddress ipaddr)
	{
		return address == ipaddr.address;
	}

	bool IPAddress::operator !=(IPAddress ipaddr)
	{
		return address != ipaddr.address;
	}

	IPAddress IPAddress::Parse(const std::string &s)
	{
		u_long address = ntohl(inet_addr(s.c_str()));
		return IPAddress(address);
	}
}
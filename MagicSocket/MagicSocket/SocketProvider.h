#pragma once

#include "Platform.h"
#include "Socket.h"
#include "Exception.h"

namespace Magic {
	class SocketProvider {
		friend class Dispatcher;
		friend class Socket;

		static SocketProvider & GetSingleton();

		SocketProvider();
		~SocketProvider();
		DWORD GetCpuCount();
		DispatcherPtr GetDispatcher();

		LPFN_TRANSMITFILE TransmitFile;
		LPFN_ACCEPTEX AcceptEx;
		LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
		LPFN_TRANSMITPACKETS TransmitPackets;
		LPFN_CONNECTEX ConnectEx;
		LPFN_DISCONNECTEX DisconnectEx;
		LPFN_WSARECVMSG WSARecvMsg;

		static void GetExtensionFunctionPtr(const SocketPtr &socket, GUID *guid, LPVOID pfn);

		DispatcherPtr dispatcher;
	};
}
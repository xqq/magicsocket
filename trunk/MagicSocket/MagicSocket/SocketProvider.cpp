#include "Platform.h"
#include "SocketProvider.h"
#include "Socket.h"

namespace Magic {
	SocketProvider & SocketProvider::GetSingleton()
	{
		static SocketProvider msp;
		return msp;
	}

	SocketProvider::SocketProvider()
	{
		WSAData wsaData;

		if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
			throw Win32Exception("WSAStartup");
		}

		static GUID guidTransmitFile = WSAID_TRANSMITFILE;
		static GUID guidAcceptEx = WSAID_ACCEPTEX;
		static GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
		static GUID guidTransmitPackets = WSAID_TRANSMITPACKETS;
		static GUID guidConnectEx = WSAID_CONNECTEX;
		static GUID guidDisconnectEx = WSAID_DISCONNECTEX;
		static GUID guidWSARecvMsg = WSAID_WSARECVMSG;

		SocketPtr socket;
		GetExtensionFunctionPtr(socket, &guidTransmitFile, &TransmitFile);
		GetExtensionFunctionPtr(socket, &guidAcceptEx, &AcceptEx);
		GetExtensionFunctionPtr(socket, &guidGetAcceptExSockaddrs, &GetAcceptExSockaddrs);
		GetExtensionFunctionPtr(socket, &guidTransmitPackets, &TransmitPackets);
		GetExtensionFunctionPtr(socket, &guidConnectEx, &ConnectEx);
		GetExtensionFunctionPtr(socket, &guidDisconnectEx, &DisconnectEx);
		GetExtensionFunctionPtr(socket, &guidWSARecvMsg, &WSARecvMsg);
	}

	SocketProvider::~SocketProvider()
	{
	}

	DWORD SocketProvider::GetCpuCount()
	{
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		return systemInfo.dwNumberOfProcessors;
	}

	void SocketProvider::GetExtensionFunctionPtr(const SocketPtr &socket, GUID *guid, LPVOID pfn)
	{
		BufferPtr inBuffer(sizeof(GUID), NoRebindTag());
		BufferPtr outBuffer(sizeof(void *), NoRebindTag());

		RtlMoveMemory(inBuffer->Pointer(), guid, sizeof(GUID));
		socket->IOControl(SIO_GET_EXTENSION_FUNCTION_POINTER, inBuffer, outBuffer);
		RtlMoveMemory(pfn, outBuffer->Pointer(), sizeof(void *));
	}

	DispatcherPtr SocketProvider::GetDispatcher()
	{
		return dispatcher;
	}
}
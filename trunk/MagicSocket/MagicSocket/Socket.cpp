#include "Platform.h"
#include "Socket.h"
#include "SocketProvider.h"
#include "Exception.h"

namespace Magic {
	Socket::Socket(int type, int protocol)
	{
		SocketProvider &msp = SocketProvider::GetSingleton();
		hSocket = socket(AF_INET, type, protocol);

		if (hSocket == INVALID_SOCKET) {
			throw Win32Exception("socket");
		}

		DispatcherPtr dispatcher = msp.GetDispatcher();
		dispatcher->Bind((HANDLE)hSocket);
	}

	Socket::~Socket()
	{
		closesocket(hSocket);
	}

	void Socket::IOControl(DWORD dwIoControlCode, const BufferPtr &inBuffer, const BufferPtr &outBuffer)
	{
		DWORD bytesReturned;

		if (0 != WSAIoctl(hSocket, dwIoControlCode, inBuffer->Pointer(), inBuffer->Size(),
			outBuffer->Pointer(), outBuffer->Size(), &bytesReturned, 0, 0))
		{
			throw Win32Exception("WSAIoCtl");
		}

		outBuffer->Resize(bytesReturned);
	}

	void Socket::Bind(u_short port, IPAddress address)
	{
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_port = htons(port);
		service.sin_addr.s_addr = htonl(address.Address());

		if (0 != bind(hSocket, (const SOCKADDR *)&service, sizeof(SOCKADDR))) {
			throw Win32Exception("bind");
		}
	}

	void Socket::Listen(int backlog)
	{
		if (0 != listen(hSocket, backlog)) {
			throw Win32Exception("listen");
		}
	}

	size_t Socket::EndAsyncOperation(const AsyncResultPtr &asyncResult)
	{
		DWORD size;
		if (0 == GetOverlappedResult((HANDLE)hSocket, asyncResult.operator ->(), &size, TRUE)) {
			throw Win32Exception("GetOverlappedResult");
		}
		return size;
	}

	SocketPtr Socket::Accept()
	{
		return EndAccept(BeginAccept());
	}

	AsyncResultPtr Socket::BeginAccept(AsyncCallback callback, const ObjectPtr &asyncState)
	{
		BufferPtr acceptBuffer((sizeof(sockaddr_in) + 16) * 2, NoRebindTag());
		SocketPtr acceptSocket;
		AsyncResultPtr asyncResult(this, acceptBuffer, asyncState, acceptSocket, callback);
		asyncResult->AddRef();

		SocketProvider &msp = SocketProvider::GetSingleton();
		if (!msp.AcceptEx(hSocket, acceptSocket->hSocket, acceptBuffer->Pointer(), 0,
			sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, asyncResult.operator ->())
			&& WSAGetLastError() != ERROR_IO_PENDING)
		{
			asyncResult->Release();
			throw Win32Exception("AcceptEx");
		}

		return asyncResult;
	}

	SocketPtr Socket::EndAccept(const AsyncResultPtr &asyncResult)
	{
		EndAsyncOperation(asyncResult);
		return asyncResult->internalState;
	}

	void Socket::Connect(u_short port, IPAddress address)
	{
		EndConnect(BeginConnect(port, address));
	}

	AsyncResultPtr Socket::BeginConnect(u_short port, IPAddress address, AsyncCallback callback, const ObjectPtr &asyncState)
	{
		AsyncResultPtr asyncResult(this, Nothing, asyncState, Nothing, callback);
		asyncResult->AddRef();

		sockaddr_in name;
		name.sin_family = AF_INET;
		name.sin_port = htons(port);
		name.sin_addr.s_addr = htonl(address.Address());

		SocketProvider &msp = SocketProvider::GetSingleton();
		if (!msp.ConnectEx(hSocket, (SOCKADDR *)&name, sizeof(SOCKADDR), 0, 0, 0, asyncResult.operator ->())
			&& WSAGetLastError() != WSA_IO_PENDING)
		{
			asyncResult->Release();
			throw Win32Exception("ConnectEx");
		}

		return asyncResult;
	}

	void Socket::EndConnect(const AsyncResultPtr &asyncResult)
	{
		EndAsyncOperation(asyncResult);
	}

	void Socket::Disconnect(bool reuseSocket)
	{
		EndDisconnect(BeginDisconnect(reuseSocket));
	}

	AsyncResultPtr Socket::BeginDisconnect(bool reuseSocket, AsyncCallback callback, const ObjectPtr &asyncState)
	{
		AsyncResultPtr asyncResult(this, Nothing, asyncState, Nothing, callback);
		asyncResult->AddRef();

		DWORD dwFlags = reuseSocket ? TF_REUSE_SOCKET : 0;

		SocketProvider &msp = SocketProvider::GetSingleton();
		if (!msp.DisconnectEx(hSocket, asyncResult.operator ->(), dwFlags, 0)
			&& WSAGetLastError() != WSA_IO_PENDING)
		{
			asyncResult->Release();
			throw Win32Exception("DisconnectEx");
		}

		return asyncResult;
	}

	void Socket::EndDisconnect(const AsyncResultPtr &asyncResult)
	{
		EndAsyncOperation(asyncResult);
	}

	size_t Socket::Receive(const BufferPtr &buffer)
	{
		return EndReceive(BeginReceive(buffer));
	}

	AsyncResultPtr Socket::BeginReceive(const BufferPtr &buffer, AsyncCallback callback, const ObjectPtr &asyncState)
	{
		AsyncResultPtr asyncResult(this, buffer, asyncState, Nothing, callback);
		asyncResult->AddRef();

		WSABUF wsabuf;
		wsabuf.buf = (char *)buffer->Pointer();
		wsabuf.len = buffer->Size();

		DWORD flags = 0;

		if (0 != WSARecv(hSocket, &wsabuf, 1, 0, &flags, asyncResult.operator ->(), 0)
			&& WSAGetLastError() != WSA_IO_PENDING)
		{
			asyncResult->Release();
			throw Win32Exception("WSARecv");
		}

		return asyncResult;
	}

	size_t Socket::EndReceive(const AsyncResultPtr &asyncResult)
	{
		return EndAsyncOperation(asyncResult);
	}

	size_t Socket::Send(const BufferPtr &buffer)
	{
		return EndSend(BeginSend(buffer));
	}

	AsyncResultPtr Socket::BeginSend(const BufferPtr &buffer, AsyncCallback callback, const ObjectPtr &asyncState)
	{
		AsyncResultPtr asyncResult(this, buffer, asyncState, Nothing, callback);
		asyncResult->AddRef();

		WSABUF wsabuf;
		wsabuf.buf = (char *)buffer->Pointer();
		wsabuf.len = buffer->Size();

		DWORD flags = 0;

		if (0 != WSASend(hSocket, &wsabuf, 1, 0, flags, asyncResult.operator ->(), 0)
			&& WSAGetLastError() != WSA_IO_PENDING)
		{
			asyncResult->Release();
			throw Win32Exception("WSASend");
		}

		return asyncResult;
	}

	size_t Socket::EndSend(const AsyncResultPtr &asyncResult)
	{
		return EndAsyncOperation(asyncResult);
	}

	DispatcherPtr Socket::GetDispatcher()
	{
		SocketProvider &msp = SocketProvider::GetSingleton();
		return msp.GetDispatcher();
	}
}
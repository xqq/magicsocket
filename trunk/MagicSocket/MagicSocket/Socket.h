#pragma once

#include "Platform.h"
#include "Dispatcher.h"
#include "IPAddress.h"
#include "Object.h"
#include "Buffer.h"

namespace Magic {
	class Socket;
	typedef Pointer <Socket> SocketPtr;

	class Socket : public Object {
	public:
		Socket(int type = SOCK_STREAM, int protocol = IPPROTO_TCP);
		~Socket();

		void IOControl(DWORD dwIoControlCode, const BufferPtr &inBuffer, const BufferPtr &outBuffer);
		void Bind(u_short port = 0, IPAddress address = INADDR_ANY);
		void Listen(int backlog = SOMAXCONN);

		SocketPtr Accept();
		AsyncResultPtr BeginAccept(AsyncCallback callback = 0, const ObjectPtr &asyncState = Nothing);
		SocketPtr EndAccept(const AsyncResultPtr &asyncResult);

		// N.B. User should call Bind() manually before Connect or BeginConnect
		void Connect(u_short port, IPAddress address);
		AsyncResultPtr BeginConnect(u_short port, IPAddress address, AsyncCallback callback = 0, const ObjectPtr &asyncState = Nothing);
		void EndConnect(const AsyncResultPtr &asyncResult);

		void Disconnect(bool reuseSocket = false);
		AsyncResultPtr BeginDisconnect(bool reuseSocket = false, AsyncCallback callback = 0, const ObjectPtr &asyncState = Nothing);
		void EndDisconnect(const AsyncResultPtr &asyncResult);

		size_t Receive(const BufferPtr &buffer);
		AsyncResultPtr BeginReceive(const BufferPtr &buffer, AsyncCallback callback = 0, const ObjectPtr &asyncState = Nothing);
		size_t EndReceive(const AsyncResultPtr &asyncResult);

		size_t Send(const BufferPtr &buffer);
		AsyncResultPtr BeginSend(const BufferPtr &buffer, AsyncCallback callback = 0, const ObjectPtr &asyncState = Nothing);
		size_t EndSend(const AsyncResultPtr &asyncResult);

		static DispatcherPtr GetDispatcher();
	private:
		SOCKET hSocket;

		size_t EndAsyncOperation(const AsyncResultPtr &asyncResult);
	};
}
#include "../MagicSocket/Socket.h"
#include <windows.h>
#include <stdexcept>
using namespace Magic;

void OnReceive(const AsyncResultPtr &asyncResult)
{
	try {
		SocketPtr acceptSocket = asyncResult->asyncState;
		BufferPtr buffer = asyncResult->buffer;
		size_t size = acceptSocket->EndReceive(asyncResult);
		if (size != 0) {
			buffer->Resize(size);
			acceptSocket->BeginSend(buffer);
			acceptSocket->BeginReceive(BufferPtr(8192, NoRebindTag()), &OnReceive, acceptSocket);
		} else {
			acceptSocket->BeginDisconnect();
		}
	} catch (const std::exception &) {
	}
}

void OnAccept(const AsyncResultPtr &asyncResult)
{
	try {
		SocketPtr listenSocket = asyncResult->asyncState;
		SocketPtr acceptSocket = listenSocket->EndAccept(asyncResult);
		acceptSocket->BeginReceive(BufferPtr(8192, NoRebindTag()), &OnReceive, acceptSocket);
		listenSocket->BeginAccept(&OnAccept, listenSocket);
	} catch (const std::exception &) {
	}
}

int main()
{
	try {
		SocketPtr listenSocket;
		listenSocket->Bind(7777);
		listenSocket->Listen();
		listenSocket->BeginAccept(&OnAccept, listenSocket);
		Sleep(INFINITE);
	} catch (const std::exception &) {
	}
}

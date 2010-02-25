#include "Platform.h"
#include "Dispatcher.h"
#include "SocketProvider.h"
#include "Exception.h"
#include <vector>

namespace Magic {
	Dispatcher::Dispatcher()
	{
		SocketProvider &msp = SocketProvider::GetSingleton();
		DWORD cpuCount = msp.GetCpuCount();
		hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, cpuCount);

		if (!hPort) {
			throw Win32Exception("CreateIoCompletionPort");
		}

		try {
			const int threadsPerCPU = 2;
			threadList.reserve(cpuCount * threadsPerCPU);

			for (DWORD index = 0; index < cpuCount * threadsPerCPU; ++index) {

				HANDLE hThread = CreateThread(0, 0, &ThreadEntryStub, (LPVOID)this, 0, 0);

				if (!hThread) {
					throw Win32Exception("CreateThread");
				}

				threadList.push_back(hThread);
			}
		} catch (const std::exception &ex) {
			Cleanup();
			throw ex;
		}
	}

	Dispatcher::~Dispatcher()
	{
		Cleanup();
	}

	void Dispatcher::Cleanup()
	{
		for (ThreadListIter iter = threadList.begin();
			iter != threadList.end(); ++iter)
		{
			PostQueuedCompletionStatus(hPort, 0, ~0, 0);
		}

		if (!threadList.empty()) {
			for (ThreadListIter iter = threadList.begin(); ;
				iter += MAXIMUM_WAIT_OBJECTS)
			{
				DWORD nCount = threadList.end() - iter;

				if (nCount > MAXIMUM_WAIT_OBJECTS) {
					WaitForMultipleObjects(MAXIMUM_WAIT_OBJECTS, &*iter, TRUE, INFINITE);
				} else {
					WaitForMultipleObjects(nCount, &*iter, TRUE, INFINITE);
					break;
				}
			}
		}

		for (ThreadListIter iter = threadList.begin();
			iter != threadList.end(); ++iter)
		{
			CloseHandle(*iter);
		}

		CloseHandle(hPort);
	}

	DWORD WINAPI Dispatcher::ThreadEntryStub(LPVOID pObj)
	{
		((Dispatcher *)pObj)->ThreadEntry();
		return 0;
	}

	void Dispatcher::ThreadEntry()
	{
		while (1) {
			DWORD size;
			ULONG_PTR key;
			OVERLAPPED *overlapped;

			GetQueuedCompletionStatus(hPort, &size, &key, &overlapped, INFINITE);
			if (key != 0) return;

			AsyncResultPtr asyncResult = (AsyncResult *)overlapped;
			asyncResult->Release();

			if (asyncResult->callback) {
				try {
					asyncResult->callback(asyncResult);
				} catch (const std::exception &) {
				}
			}
		}
	}

	void Dispatcher::Bind(HANDLE hObject)
	{
		if (!CreateIoCompletionPort(hObject, hPort, 0, 0)) {
			throw Win32Exception("CreateIoCompletionPort");
		}
	}

	void Dispatcher::Queue(const AsyncResultPtr &ar)
	{
		ar->AddRef();
		if (0 == PostQueuedCompletionStatus(hPort, 0, 0, ar.operator ->())) {
			throw Win32Exception("PostQueuedCompletionStatus");
		}
	}

	AsyncResult::AsyncResult(const ObjectPtr &sender, const ObjectPtr &buffer,
		const ObjectPtr &asyncState, const ObjectPtr &internalState, AsyncCallback callback)
		: sender(sender), buffer(buffer), asyncState(asyncState), internalState(internalState), callback(callback)
	{
		RtlZeroMemory((OVERLAPPED *)this, sizeof(OVERLAPPED));
	}
}
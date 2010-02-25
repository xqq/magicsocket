#pragma once

#include "Platform.h"
#include "Object.h"
#include <vector>

namespace Magic {
	struct AsyncResult;
	typedef Pointer <AsyncResult> AsyncResultPtr;
	typedef void (*AsyncCallback)(const AsyncResultPtr &asyncResult);

	struct AsyncResult : public OVERLAPPED, public Object {
		AsyncResult(const ObjectPtr &sender, const ObjectPtr &buffer, const ObjectPtr &asyncState, const ObjectPtr &internalState, AsyncCallback callback);

		ObjectPtr sender;
		ObjectPtr buffer;
		ObjectPtr asyncState;
		ObjectPtr internalState;
		AsyncCallback callback;
	};


	class Dispatcher : public Object {
	public:
		Dispatcher();
		virtual ~Dispatcher();
		void ThreadEntry();
		void Bind(HANDLE hObject);
		void Queue(const AsyncResultPtr &ar);
	    
	private:
		HANDLE hPort;
		std::vector <HANDLE> threadList;
		typedef std::vector <HANDLE>::iterator ThreadListIter;

		void Cleanup();
		static DWORD WINAPI ThreadEntryStub(LPVOID pObj);
	};

	typedef Pointer <Dispatcher> DispatcherPtr;
}
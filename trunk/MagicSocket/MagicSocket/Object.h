#pragma once

#include "Platform.h"
#include "Heap.h"
#include "Pointer.h"

namespace Magic {
	class Object {
	public:
		Object();
		virtual ~Object();
		void AddRef();
		void Release();
		void * operator new(size_t size);
		void operator delete(void *pointer);
		static Heap & GetObjectHeap();
	private:
		volatile LONG refCount;
	};

	typedef Pointer <Object> ObjectPtr;
	extern ObjectPtr Nothing;
}

#include "Platform.h"
#include "Object.h"

namespace Magic {
	ObjectPtr Nothing;

	Object::Object()
	{
		refCount = 0;
	}

	Object::~Object()
	{
	}

	void Object::AddRef()
	{
		InterlockedIncrement(&refCount);
	}

	void Object::Release()
	{
		if (InterlockedDecrement(&refCount) == 0) {
			delete this;
		}
	}

	void * Object::operator new(size_t size)
	{
		Heap &heap = GetObjectHeap();
		return heap.Allocate(size);
	}

	void Object::operator delete(void *pointer)
	{
		Heap &heap = GetObjectHeap();
		heap.Free(pointer);
	}

	Heap & Object::GetObjectHeap()
	{
		static Heap heap;
		return heap;
	}
}
#include "Platform.h"
#include "Heap.h"
#include "Exception.h"

namespace Magic {
	Heap::Heap()
	{
		hHeap = HeapCreate(0, 0, 0);

		if (!hHeap) {
			throw Win32Exception("HeapCreate");
		}

		ULONG HeapFragValue = 2;
		HeapSetInformation(hHeap, HeapCompatibilityInformation,
			&HeapFragValue, sizeof(HeapFragValue));
	}

	Heap::~Heap()
	{
		HeapDestroy(hHeap);
	}

	void * Heap::Allocate(size_t size)
	{
		void *result;

		result = HeapAlloc(hHeap, 0, size);
		if (!result) {
			throw std::bad_alloc("HeapAlloc failed");
		}

		return result;
	}

	void Heap::Free(void *pointer)
	{
		if (!HeapFree(hHeap, 0, pointer)) {
			throw Win32Exception("HeapFree");
		}
	}
}
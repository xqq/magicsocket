#pragma once

#include "Platform.h"

namespace Magic {
	class Heap {
	public:
		Heap();
		~Heap();
		void * Allocate(size_t size);
		void Free(void *pointer);

	private:
		Heap(const Heap &);
		Heap & operator =(const Heap &);

		HANDLE hHeap;
	};
}
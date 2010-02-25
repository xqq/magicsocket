#pragma once

#include "Object.h"

namespace Magic {
	typedef unsigned char Byte;

	class Buffer : public Object {
	public:
		Buffer(size_t size = 8192);
		Buffer(const char *str);
		virtual ~Buffer();
		size_t Size() const;
		void Resize(size_t newSize);
		const Byte * Pointer(size_t offset = 0) const;
		Byte * Pointer(size_t offset = 0);
		static Heap & GetBufferHeap();
	private:
		Byte *buffer;
		size_t bufferSize;
		size_t allocSize;
	};

	typedef Pointer <Buffer> BufferPtr;
}
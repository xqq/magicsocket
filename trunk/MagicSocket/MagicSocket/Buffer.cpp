#include "Buffer.h"
#include <cstring>

namespace Magic {
	Buffer::Buffer(size_t size)
	{
		allocSize = bufferSize = size;
		if (allocSize < 4) allocSize = 4;

		Heap &heap = GetBufferHeap();
		buffer = (Byte *)heap.Allocate(allocSize);
	}

	Buffer::Buffer(const char *str)
	{
		allocSize = bufferSize = strlen(str);
		if (allocSize < 4) allocSize = 4;

		Heap &heap = GetBufferHeap();
		buffer = (Byte *)heap.Allocate(allocSize);
		RtlMoveMemory(buffer, str, bufferSize);
	}

	Buffer::~Buffer()
	{
		Heap &heap = GetBufferHeap();
		heap.Free(buffer);
	}

	size_t Buffer::Size() const
	{
		return bufferSize;
	}

	void Buffer::Resize(size_t size)
	{
		if (size < allocSize) {
			bufferSize = size;
		} else {
			if (size < allocSize * 2) {
				allocSize *= 2;
			} else {
				allocSize = size;
			}

			Heap &heap = GetBufferHeap();
			Byte *newBuffer = (Byte *)heap.Allocate(allocSize);
			RtlMoveMemory(newBuffer, buffer, bufferSize);
			heap.Free(buffer);
			buffer = newBuffer;
			bufferSize = size;
		}
	}

	Byte * Buffer::Pointer(size_t offset)
	{
		return &buffer[offset];
	}

	const Byte * Buffer::Pointer(size_t offset) const
	{
		return &buffer[offset];
	}

	Heap & Buffer::GetBufferHeap()
	{
		static Heap heap;
		return heap;
	}
}
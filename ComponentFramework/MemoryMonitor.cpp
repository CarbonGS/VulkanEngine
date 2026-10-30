#include "MemoryMonitor.h"
#include <cstdio>
#include <cstdlib>
#include <new>

std::size_t totalMemoryAllocated = 0;
std::size_t totalMemoryFreed = 0;

namespace {
	// Header to keep track of memory sizes allocated
	// malloc & free do not expose bookkeeping so we are just writing out our own so we know exact sizes of allocated memory
	constexpr std::size_t kHeader = alignof(std::max_align_t); 

	void* trackedAlloc(std::size_t size, const char* type) {
		void* raw = std::malloc(size + kHeader); // Raw pointer (total size)
		if (!raw)
		{
			throw std::bad_alloc(); // Failed to allocate memory
		}
		*static_cast<std::size_t*>(raw) = size; // Write size into the first 8 bytes of memory in our header (bookkeeping)
		totalMemoryAllocated += size;

		return static_cast<std::byte*>(raw) + kHeader; // Returns the memory address including the byte offset of our header
	}

	void trackedFree(void* ptr, const char* type) noexcept {
		if (!ptr) return;
		void* raw = static_cast<std::byte*>(ptr) - kHeader; // Raw pointer (including own header)

		std::size_t size = *static_cast<std::size_t*>(raw); // Returns total size of our deallocation
		totalMemoryFreed += size;
	
		std::free(raw);
	}
}

void* operator new(std::size_t size) { return trackedAlloc(size, "memory"); }
void* operator new[](std::size_t size) { return trackedAlloc(size, "array"); }

void operator delete(void* ptr) noexcept { trackedFree(ptr, "memory"); }
void operator delete(void* ptr, std::size_t) noexcept { trackedFree(ptr, "memory"); }
void operator delete[](void* ptr) noexcept { trackedFree(ptr, "array"); }
void operator delete[](void* ptr, std::size_t) noexcept { trackedFree(ptr, "array"); }

void checkForLeaks() {
	if (totalMemoryAllocated != totalMemoryFreed)
	{
		std::printf("Memory leaks detected, total leaked memory: %zu Bytes\n",
			totalMemoryAllocated - totalMemoryFreed);
	}
	else
	{
		std::printf("No leaks detected\n");
	}
}
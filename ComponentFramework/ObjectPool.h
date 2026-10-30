#pragma once
#include <cassert>

template<typename T>
class ObjectPool
{
public:
	explicit ObjectPool(size_t capacity); // Create object pool with predifined size
	~ObjectPool();

	T* Acquire();
	void Release(T* object);

	size_t FreeCount() const;

private:
	void* block; // Memory block allocated
	void* freeListHead; // Next free head (pointer head)
	size_t capacity; // Capacity of memory block allocated
};

template<typename T>
inline ObjectPool<T>::ObjectPool(size_t capacity_)
{
	// Example:
	// Block is 1000-1064, current slot is the one we assign each new block position in bytes, next slot dictates the next avaiable slot in bytes
	// i = 0; currentSlot is 1000; nextSlot is 1032
	// i = 1; currentSlot is 1032; nextSlot is 1064
	// etc... Basically its a list of pointers that points to the next object in the pool
	// Finally we assign the freeListHead to the first pointer (ie start of the pool's block)

	capacity = capacity_; // Slot count
	size_t totalBytes = capacity * sizeof(T); // Bytes is the capacity * size of the object type (assumes any type that can be pooled)

	block = operator new(totalBytes, std::align_val_t(alignof(T))); // Assign block the pointer to aligned memory
	
	for (size_t i = 0; i < capacity; ++i)
	{
		void* currentSlot = static_cast<std::byte*>(block) + i * sizeof(T); // Calculate current slot block start pos 

		void* nextSlot = nullptr; // Default to nullptr assuming end of list 

		if (i != capacity - 1)
		{
			nextSlot = static_cast<std::byte*>(block) + (i + 1) * sizeof(T); // Overwrite nextSlot nullptr to next slot in memory
		}

		*reinterpret_cast<void**>(currentSlot) = nextSlot; // Current pointer assigned the pointer for next slot in block

	}
	freeListHead = block;
}

template<typename T>
inline ObjectPool<T>::~ObjectPool()
{
	assert(FreeCount() == capacity); // Ensure pool is emptied

	operator delete(block, std::align_val_t(alignof(T))); // Assign whole block raw memory - ie delete block
}

template<typename T>
inline T* ObjectPool<T>::Acquire()
{
	assert(freeListHead != nullptr); // Ensures head isn't the end

	void* nextSlot = *reinterpret_cast<void**>(freeListHead); // Get the next slots pointer (actually gets the byte address but interperted as a pointer)

	void* currentSlot = freeListHead; // Keep a temp reference to the current head

	freeListHead = nextSlot;

	T* newSlot = new (currentSlot) T(); //  Create new object from our current available slot in our block

	return newSlot;
}

template<typename T>
inline void ObjectPool<T>::Release(T* object)
{
	assert(object != nullptr); // Object is invalid

	object->~T(); // Call objects destructor - object is now raw memory in our slot

	*reinterpret_cast<void**>(object) = freeListHead; // Assign our object the head - start of the memory at our raw slot

	freeListHead = object; // Assign our head to our object (object is now empty, ie free)

}

template<typename T>
inline size_t ObjectPool<T>::FreeCount() const
{
	size_t count = 0;

	void* currentSlot = freeListHead; // Get current head

	while (currentSlot != nullptr)
	{
		count++;

		currentSlot = *reinterpret_cast<void**>(currentSlot); // Move to next slot
	}

	return count;
}

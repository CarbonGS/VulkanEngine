#ifndef MEM_MONITOR_H
#define MEM_MONITOR_H

#include <cstddef>

extern std::size_t totalMemoryAllocated;
extern std::size_t totalMemoryFreed;

void checkForLeaks();

#endif // !MEM_MONITOR_H
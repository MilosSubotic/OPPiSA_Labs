#ifndef __RESOURCE_ALLOCATOR__
#define __RESOURCE_ALLOCATOR__

#include <vector>

struct LprsInstruction;


struct ResourceAllocator
{
	void allocateResources(std::vector<LprsInstruction*>& instList);
};

#endif

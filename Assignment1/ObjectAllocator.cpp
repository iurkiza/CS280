#include "ObjectAllocator.h"

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig & config)
{
    configuration = config;

    stats.ObjectSize_ = ObjectSize;

    int linkPointer = 8;
    stats.PageSize_ = (ObjectSize * config.ObjectsPerPage_) + linkPointer;

    PageList_->Next = nullptr;
    FreeList_->Next = nullptr;
    
}

ObjectAllocator::~ObjectAllocator()
{

}

void * ObjectAllocator::Allocate(const char * label)
{
    
}

void ObjectAllocator::Free(void * Object)
{


}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{


}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{

}

unsigned ObjectAllocator::FreeEmptyPages(void)
{

}

bool ObjectAllocator::ImplementedExtraCredit(void)
{

}

void ObjectAllocator::SetDebugState(bool State)
{

}

const void * ObjectAllocator::GetFreeList(void) const
{
    return FreeList_->Next;
}

const void * ObjectAllocator::GetPageList(void) const
{
    return PageList_->Next;
}  

OAConfig ObjectAllocator::GetConfig(void) const
{
    return configuration;
}  

OAStats ObjectAllocator::GetStats(void) const
{
    return stats;
}
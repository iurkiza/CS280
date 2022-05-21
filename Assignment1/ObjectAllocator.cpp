#include "ObjectAllocator.h"
#include <cstring>

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig & config)
{
    configuration = config;

    stats.ObjectSize_ = ObjectSize;

    stats.PageSize_ = (ObjectSize * config.ObjectsPerPage_) + + sizeof(GenericObject);

    allocate_new_page();


    
}

void ObjectAllocator::allocate_new_page(void)
{
    
    if(stats.PagesInUse_ == 0)
    {
        PageList_ = reinterpret_cast<GenericObject*>(malloc(stats.PageSize_));
        PageList_->Next = nullptr;


        navigator = reinterpret_cast<unsigned char*>(PageList_);
        navigator += 8;
        memset(navigator, UNALLOCATED_PATTERN, stats.ObjectSize_);
        FreeList_ = reinterpret_cast<GenericObject*>(navigator);
        FreeList_->Next = nullptr;
        


    }
    else 
    {
        GenericObject* temp = PageList_;
        PageList_ = reinterpret_cast<GenericObject*>(malloc(stats.PageSize_));
        PageList_->Next = temp;

        navigator = reinterpret_cast<unsigned char*>(PageList_);
        navigator += 8;
        memset(navigator , UNALLOCATED_PATTERN, stats.ObjectSize_);
        GenericObject* temp2 = FreeList_;
        FreeList_ = reinterpret_cast<GenericObject*>(navigator);
        FreeList_->Next = temp;
        

    }

    for(unsigned i = 0; i < configuration.ObjectsPerPage_; i++)
    {

        GenericObject* temp = FreeList_;
        FreeList_ = reinterpret_cast<GenericObject*>(navigator);
        navigator += stats.ObjectSize_;
        memset(FreeList_, UNALLOCATED_PATTERN, stats.ObjectSize_);
        FreeList_->Next = temp;
    }

    stats.PagesInUse_++;
    stats.FreeObjects_ += configuration.ObjectsPerPage_;
    stats.MostObjects_ += configuration.ObjectsPerPage_;

}

ObjectAllocator::~ObjectAllocator()
{

}

void * ObjectAllocator::Allocate(const char * label)
{
    

    if(stats.FreeObjects_ == 0)
    {
        if(configuration.MaxPages_ == stats.PagesInUse_)
        {
            throw OAException(OAException::OA_EXCEPTION::E_NO_PAGES, "Exception thrown from Allocate (2) in DoStudents.");
        }
        allocate_new_page();
    }
    
    stats.ObjectsInUse_++;
    stats.Allocations_++;
    stats.FreeObjects_--;

    GenericObject* temp = FreeList_;
    FreeList_ = temp->Next;

    memset(temp, ALLOCATED_PATTERN, stats.ObjectSize_);

    
    

    return temp;

}

void ObjectAllocator::Free(void * Object)
{
    GenericObject* temp = FreeList_;

    FreeList_ = temp->Next;

    //delete Object;

    //memset(temp, FREED_PATTERN, stats.ObjectSize_);

    stats.ObjectsInUse_--;
    stats.FreeObjects_++;
    stats.Deallocations_++;

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
    configuration.DebugOn_ = State;
}

const void * ObjectAllocator::GetFreeList(void) const
{
    return FreeList_->Next;
}

const void * ObjectAllocator::GetPageList(void) const
{
    return PageList_;
}  

OAConfig ObjectAllocator::GetConfig(void) const
{
    return configuration;
}  

OAStats ObjectAllocator::GetStats(void) const
{
    return stats;
}
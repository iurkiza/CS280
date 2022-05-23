#include "ObjectAllocator.h"
#include <cstring>

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig & config)
{
    configuration = config;

    stats.ObjectSize_ = ObjectSize;

    stats.PageSize_ = (ObjectSize * config.ObjectsPerPage_) + (config.HBlockInfo_.size_ * config.ObjectsPerPage_) + (config.PadBytes_ * 2 * config.ObjectsPerPage_) + sizeof(GenericObject);

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

        if (configuration.HBlockInfo_.size_ != 0)
        {
            memset(navigator, 00, configuration.HBlockInfo_.size_);
            navigator += configuration.HBlockInfo_.size_;
       
        }

        memset(navigator, PAD_PATTERN, configuration.PadBytes_);
        navigator += configuration.PadBytes_;

        GenericObject* temp = FreeList_;
        FreeList_ = reinterpret_cast<GenericObject*>(navigator);

        navigator += stats.ObjectSize_;
        memset(FreeList_, UNALLOCATED_PATTERN, stats.ObjectSize_);

        memset(navigator, PAD_PATTERN, configuration.PadBytes_);
        navigator += configuration.PadBytes_;
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

    if (configuration.HBlockInfo_.size_ != 0)
    {
        navigator = reinterpret_cast<unsigned char*>(temp);
        memset(navigator - configuration.PadBytes_ -1, 01, 1);
    
    }
    
    

    return temp;

}

bool ObjectAllocator::is_on_freelist(void* Object)
{
    GenericObject* iterator = FreeList_;

    while (iterator->Next != nullptr)
    {
        if (iterator->Next == Object)
        {
            return true;
        }

        FreeList_ = FreeList_->Next;

    }

    return false;


}

bool ObjectAllocator::is_in_pageList(void* Object)
{

    GenericObject* iterator = PageList_;

    while (iterator)
    {
        if ((uintptr_t)Object >= (uintptr_t)iterator && (uintptr_t)Object < (uintptr_t)iterator + (uintptr_t)stats.PageSize_)
        {
        }
        else
        {
            return false;
        }


        iterator = iterator->Next;

    }

    return true;

}

void ObjectAllocator::Free(void * Object)
{
    //iterate trough FreeList_ to check where the object is
    //if (is_on_freelist(Object))
    //{
    //    throw OAException(OAException::E_MULTIPLE_FREE, "FreeObject: Object has already been freed.");
    //}
    //if (!is_in_pageList(Object))
    //{
    //    throw OAException(OAException::E_BAD_BOUNDARY, "validate_object: Object on bad boundary in page.");
    //}


    GenericObject* temp = FreeList_;

    FreeList_ = reinterpret_cast<GenericObject*>(Object);
    FreeList_->Next = temp;

    memset(Object, FREED_PATTERN, stats.ObjectSize_);

    if (configuration.HBlockInfo_.size_ != 0)
    {
        navigator = reinterpret_cast<unsigned char*>(FreeList_);
        memset(navigator - configuration.PadBytes_ - 1, 00, 1);
    }

    stats.ObjectsInUse_--;
    stats.FreeObjects_++;
    stats.Deallocations_++;

}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{

    return 0;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{


    return 0;
}

unsigned ObjectAllocator::FreeEmptyPages(void)
{
    return 0;
}

bool ObjectAllocator::ImplementedExtraCredit(void)
{
    return false;
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
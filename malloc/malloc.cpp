#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <iostream>


///===================================Struct Headers and Declarations====================================================
struct MallocMetaData {
    unsigned int size; //metadata included
    bool is_free;
    MallocMetaData* next;
    MallocMetaData* prev;
    MallocMetaData* hist_next;
    MallocMetaData* hist_prev;
    void* start_address; //INFO start address

    MallocMetaData(size_t size, MallocMetaData* next = NULL, MallocMetaData* prev = NULL, bool is_free = 0, void* start = NULL) :
            size(size), next(next), prev(prev),hist_next(NULL), hist_prev(NULL), is_free(is_free), start_address(start){}
};

struct MallocMetaDataList {
    size_t allocd_blocks_num;
    size_t allocd_bytes_num; //including metadata
    size_t free_blocks_num;
    size_t free_bytes_num; //including metadata
    MallocMetaData* head;


    MallocMetaDataList() : head(nullptr), allocd_blocks_num(0), allocd_bytes_num(0), free_blocks_num(0), free_bytes_num(0) {}
    void InsertByAddress(MallocMetaData* to_insert);
    void InsertBySize(MallocMetaData* to_insert);
    void InsertFromHead(MallocMetaData* to_insert);
    void remove(MallocMetaData* to_remove);
    MallocMetaData* findFitAndRemove(size_t size);
    MallocMetaData* getWildernessBlock();
};



///===============================================Global Variables and Declarations===========================================================
#define KB 1024
#define SBRK_MAX_SIZE 128*1024
#define ALLOCATION_ERROR ((void*)-1)
#define MIN_REQ_SIZE 128
#define BINS_NUM 128
#define ALLOC_MAX_SIZE 1e8

static MallocMetaDataList data_list = MallocMetaDataList();
static MallocMetaDataList mmap_data_list = MallocMetaDataList();
static MallocMetaDataList hist [BINS_NUM];
const size_t STRUCT_SIZE = sizeof(MallocMetaData);

void _split(MallocMetaData* removed, size_t effective);
void _merge(MallocMetaData* left, MallocMetaData* right, bool left_in_hist); //assuming left is not in hist and right is




void MallocMetaDataList::InsertByAddress(MallocMetaData* to_insert)
{
    
    to_insert->prev = NULL;
    to_insert->next = NULL;
    if (!head)
    {
        head = to_insert;
        return;
    }
    // else
    // {
        MallocMetaData* runner = head;
        //while(runner && runner->next && (unsigned long)(runner->start_address) < (unsigned long)(to_insert->start_address))
        while(runner->next)
        {
            if (runner->start_address < to_insert->start_address && runner->next > to_insert->start_address)
            {
                auto temp = runner->next;
                runner->next = to_insert;
                to_insert->prev = runner;
                to_insert->next = temp;
                temp->prev = to_insert;
                return;
            }
            runner = runner->next;
        }
   
        runner->next = to_insert;
        to_insert ->prev = runner;
       
}

void MallocMetaDataList::InsertBySize(MallocMetaData* to_insert)
{
    to_insert->hist_prev = NULL;
    to_insert->hist_next = NULL;

    assert(to_insert->is_free);
    if (!head)
    {
        head = to_insert;
    }
    else
    {
        MallocMetaData* runner = head;
        while (runner)
        {
            if (to_insert->size >= runner->size)
            {
                if (runner->hist_next && runner->hist_next->size >= to_insert->size)
                {
                    MallocMetaData* temp = runner->hist_next;
                    runner->hist_next = to_insert;
                    temp->hist_prev = to_insert;
                    to_insert->hist_prev = runner;
                    to_insert->hist_next = temp;
                    break;
                }
                else if (!runner->hist_next)
                {
                    runner->hist_next = to_insert;
                    to_insert->hist_prev = runner;
                    break;
                }
                
            }
            runner = runner->hist_next;
        }
    }
    free_blocks_num++;
    allocd_blocks_num++;
    free_bytes_num += to_insert->size;
    allocd_bytes_num += to_insert->size;
}

MallocMetaData* MallocMetaDataList::findFitAndRemove(size_t size)
{//size parameter here includes metadata
    MallocMetaData* runner = head;
    while(runner)
    {
        if(runner->size >= size) //every block should be free
            break;
        runner = runner->hist_next;
    }

    if(!runner) //loop ended, fit not found
        return NULL;

    remove(runner);
    return runner;

}

void MallocMetaDataList::remove(MallocMetaData* to_remove)
{
    if(to_remove == head)
    {
        head = to_remove->hist_next;
        if(head)
            head->hist_prev = NULL;
    }
    else
    {
        to_remove-> hist_prev->hist_next = to_remove->hist_next;
        if(to_remove->hist_next)
            to_remove->hist_next->hist_prev = to_remove->hist_prev;
    }

    allocd_blocks_num--;
    allocd_bytes_num -= to_remove->size;
    if (to_remove->is_free)
    {
        free_blocks_num--;
        free_bytes_num -= to_remove->size;
    }

}

MallocMetaData *MallocMetaDataList::getWildernessBlock()
{
    auto runner = head;
    if (!runner)
        return NULL;
    while(runner->next)
        runner = runner->next;
    return runner;
}

void MallocMetaDataList::InsertFromHead(MallocMetaData *to_insert)
{
    to_insert->prev = NULL;
    to_insert->next = NULL;
    if (!head)
        head = to_insert;
    else
    {
        auto temp = head;
        head = to_insert;
        to_insert->next = temp;
        temp->prev = to_insert;
    }
}

void _split(MallocMetaData* removed, size_t effective)
{
    size_t rem_size = removed->size - effective;
    removed->size = effective;

    MallocMetaData remainder = MallocMetaData(rem_size, NULL,NULL, \
                                              true, (void*)((unsigned long)(removed->start_address) + effective+STRUCT_SIZE)); ////added struct size

    (*((MallocMetaData*)((void*)((unsigned long)(removed->start_address) + effective)))) = remainder; //put new remainder metadata on heap
    int insertBlock = (rem_size)/KB < BINS_NUM ? (rem_size)/KB : BINS_NUM-1;
    hist[insertBlock].InsertBySize((MallocMetaData*)(void*)((unsigned long)(removed->start_address) + effective)); //INSERT BY SIZE DOES CHANGE LIST SIZES

    if (removed->is_free)
    {
        insertBlock = (removed->size)/KB < BINS_NUM ? (removed->size)/KB : BINS_NUM-1;

        hist[insertBlock].InsertBySize(removed);
    }

    data_list.InsertByAddress((MallocMetaData*)(void*)((unsigned long)(removed->start_address) + effective)); //new effective block is already in list

    data_list.free_blocks_num++;
    data_list.free_bytes_num += remainder.size;
    data_list.allocd_blocks_num++;
  
}

void _merge(MallocMetaData* left, MallocMetaData* right, bool left_in_hist)
{//assuming left is not in hist and right is

    MallocMetaData* remove = left_in_hist ? left : right;

    int insertBlock = (remove->size)/KB <BINS_NUM ? (remove->size)/KB : BINS_NUM-1;

    hist[insertBlock].remove(remove);

    left->size += right->size;

}

///=====================================================Malloc Implementation==============================================

void* smalloc (size_t size)
{
    if (!size || size > ALLOC_MAX_SIZE)
        return NULL;
    size += STRUCT_SIZE;
    size+= (!(size%8)) ? 0 : 8-size%8;
    if (size-STRUCT_SIZE >= SBRK_MAX_SIZE) //use mmap instead of sbrk for large allocations
    {
        void* ret = mmap(NULL, size, PROT_READ | PROT_WRITE , MAP_ANONYMOUS | MAP_PRIVATE, -1, 0); //maybe change to MAP_SHARED need to check that
        if (ret == MAP_FAILED)
        {
            return NULL;
        }
        auto new_block = MallocMetaData(size);
        new_block.start_address = (void*)((unsigned long)ret + STRUCT_SIZE);
        (*((MallocMetaData*)(ret))) = new_block;
        mmap_data_list.InsertFromHead((MallocMetaData*)(ret));
        mmap_data_list.allocd_bytes_num += size;
        mmap_data_list.allocd_blocks_num++;
        return (void*)((unsigned long)ret + STRUCT_SIZE);
    }
    int search_block = size/KB < BINS_NUM ? size/KB : BINS_NUM-1;
    MallocMetaData* allocated  = NULL;
    while(!allocated && search_block < BINS_NUM)
    {
        allocated = (hist[search_block]).findFitAndRemove(size);
        search_block++;
    }

    if(allocated) //alocate from allocated free memory
    {
        allocated->is_free = false;
        data_list.free_blocks_num--;
        data_list.free_bytes_num -= allocated->size;
        //hist list sizes changed in remove called by findFitAndRemove

        //split if large enough
        if(allocated->size-size >= MIN_REQ_SIZE)
            _split(allocated,size); //all list and struct size changes are handled inside split

        return allocated->start_address;
    }

    //try to enlarge the wilderness block if its free
    auto wilderness_block = data_list.getWildernessBlock();
    if (wilderness_block && wilderness_block->is_free)
    {
        size_t bytes_to_add = size - wilderness_block->size; //both include metadata size
        void* ret = sbrk(bytes_to_add);
        if (ret == ALLOCATION_ERROR)
            return NULL;
        //remove block from the hist since its no unsigned longer free
        auto hist_idx = (wilderness_block->size) / KB < BINS_NUM ? (wilderness_block->size) / KB : BINS_NUM-1;
        hist[hist_idx].remove(wilderness_block);
        data_list.free_blocks_num--;
        data_list.free_bytes_num -= wilderness_block->size;
        data_list.allocd_bytes_num += bytes_to_add;
        wilderness_block->size += bytes_to_add;
        wilderness_block->is_free = false;
        return wilderness_block->start_address;
    }

    //ask for more space from OS
    void* ret = sbrk(size);
    if(ret == ALLOCATION_ERROR)
        return NULL;

    data_list.allocd_blocks_num++;
    data_list.allocd_bytes_num += size;
    MallocMetaData added = MallocMetaData(size);
    added.start_address = (void*)((unsigned long)ret + STRUCT_SIZE);
    (*((MallocMetaData*)(ret))) = added;

    data_list.InsertByAddress((MallocMetaData*)(ret));

    return (void*)((unsigned long)ret + STRUCT_SIZE);
}


void* scalloc (size_t num, size_t size)
{
    void* addr = smalloc(size*num); //Assuming only one metadata is needed
    if(!addr)
        return NULL;
    memset(addr,0,num*size); //remember according to smalloc, addr is start of information
    return addr;
}


void detach(MallocMetaData* det)
{
    if (!det)
        return;
    if(det->next)
        det->next->prev = det->prev;

    if(det->prev)
        det->prev->next = det->next;

    if (det->is_free)
    {
        data_list.free_blocks_num--;
        //data_list.free_bytes_num -= det->size;
    }
    data_list.allocd_blocks_num--;
   // data_list.allocd_bytes_num -= det->size;
}


void sfree(void* p)
{
    if (!p)
        return;
    auto to_free = (MallocMetaData*)((void*)((unsigned long)p - STRUCT_SIZE));
    if (to_free->size-STRUCT_SIZE >= SBRK_MAX_SIZE) //meaning the block was allocated by mmap
    {
        if (to_free->prev)
            to_free->prev->next = to_free->next;
        if (to_free->next)
            to_free->next->prev = to_free->prev;
        if (to_free == mmap_data_list.head)
            mmap_data_list.head = to_free->next;
        mmap_data_list.allocd_blocks_num--;
        mmap_data_list.allocd_bytes_num -= to_free->size;
        munmap((void*)((unsigned long)to_free->start_address - STRUCT_SIZE), to_free->size);
        return;
    }
    MallocMetaData* runner = data_list.head;
    while (runner)
    {
        if (runner->start_address == p)
        {
            if (!runner->is_free)
            {
                runner->is_free = true;
                data_list.free_bytes_num += runner->size;
                data_list.free_blocks_num++;
            }
            break;
        }
        runner = runner->next;
    }
    if(!runner) //just to avoid seg fault in bad usage - should never be true
        return;
    if(runner->next && runner->next->is_free)
    {
        _merge(runner, runner->next, 0);
        detach(runner->next);
    }

    if(runner->prev && runner->prev->is_free)
    {
        _merge(runner->prev, runner, 1);
        detach(runner);
        runner = runner->prev; //to insert correct metadata to hist
    }

    int block = runner->size/KB < BINS_NUM ? runner->size/KB : BINS_NUM-1;
    hist[block].InsertBySize(runner);
}


void* srealloc(void* oldp, size_t size)
{
    if (size == 0 || size > ALLOC_MAX_SIZE)
        return NULL;
    if (!oldp)
    {
        return smalloc(size);
    }
    size += STRUCT_SIZE;
    size+= (!(size%8)) ? 0 : 8-size%8;
    auto wilderness = data_list.getWildernessBlock();

    auto to_realloc = (MallocMetaData*)((void*)((unsigned long)oldp - STRUCT_SIZE));
    if (to_realloc->size-STRUCT_SIZE >= SBRK_MAX_SIZE) //block was allocated by mmap
    { /*not sure what to do in case the new size is less than old size and both are > SBRK_MAX_SIZE: free the remainder and return oldp or
       *reallocate new block (with different address)? meanwhile this code reallocates new block (piazza 584)*/
        void* new_block = smalloc(size-STRUCT_SIZE);
        if (new_block == MAP_FAILED)
            return NULL;
        size_t n = to_realloc->size - STRUCT_SIZE;
        if (to_realloc->size > size)
            n = size - STRUCT_SIZE;
        memcpy(new_block, oldp, n);
        sfree(oldp);
        return new_block;
    }
    if (to_realloc->size-STRUCT_SIZE >= size)
    {//reuse the same block
        if (to_realloc->size-STRUCT_SIZE - size >= MIN_REQ_SIZE)
            _split(to_realloc, size);
        return oldp;
    }
    if (to_realloc->prev && to_realloc->prev->is_free && to_realloc->prev->size + to_realloc->size -STRUCT_SIZE >= size)
    {//try to merge with left block
        auto prev = to_realloc->prev;
        auto prev_size = prev->size;
        _merge(prev, to_realloc, true);
        detach(to_realloc);
        prev->is_free = false;
        data_list.free_bytes_num -= prev_size;
        if (prev->size - size >= MIN_REQ_SIZE)
            _split(prev, size);
        memcpy(prev->start_address, oldp, to_realloc->size-STRUCT_SIZE);

        data_list.free_blocks_num--;
        return prev->start_address;
    }
    else if (to_realloc->next && to_realloc->next->is_free && to_realloc->next->size + to_realloc->size -STRUCT_SIZE >= size)
    {//try to merge with right block
        auto next = to_realloc->next;
        auto next_size = next->size;
        auto to_realloc_original_size = to_realloc->size;
        _merge(to_realloc, to_realloc->next, false);
        detach(to_realloc->next);
        if (to_realloc->size - size >= MIN_REQ_SIZE)
            _split(to_realloc, size);
        data_list.free_bytes_num -= next_size;
        return next->start_address;
    }
    else if (to_realloc->next && to_realloc->prev && to_realloc->next->is_free && to_realloc->prev->is_free && \
                        to_realloc->prev->size + to_realloc->size + to_realloc->next->size -STRUCT_SIZE >= size)
    {//try to merge with both left and right blocks
        auto prev = to_realloc->prev;
        auto prev_size = prev->size;
        auto next = to_realloc->next;
        auto next_size = next->size;
        auto to_realloc_original_size = to_realloc->size;
        _merge(prev, to_realloc, true);
        detach(to_realloc);
        prev->is_free = false;
        data_list.free_bytes_num -= prev_size;
        data_list.free_blocks_num--;
        _merge(prev, next, false);
        data_list.free_bytes_num -= next_size;
        detach(to_realloc->next);
        if (prev->size - size >= MIN_REQ_SIZE)
            _split(prev, size);
        memcpy(prev->start_address, oldp, to_realloc_original_size-STRUCT_SIZE);
        return prev->start_address;
    }
    else if (wilderness && wilderness->start_address == oldp)
    {
        size_t bytes_to_add = size - wilderness->size;
        void* ret = sbrk(bytes_to_add);
        if (ret == ALLOCATION_ERROR)
            return NULL;
        wilderness->size += bytes_to_add;
        data_list.allocd_bytes_num += bytes_to_add;
       
        return oldp;
    }
    void* new_block = smalloc(size-STRUCT_SIZE); //allocate new block
    if (!new_block)
        return NULL;
    sfree(oldp);
    memcpy(new_block, oldp, to_realloc->size-STRUCT_SIZE);
    return new_block;
}


size_t _num_free_blocks()
{
    return data_list.free_blocks_num;
}

size_t _num_free_bytes()
{
    return data_list.free_bytes_num - data_list.free_blocks_num*STRUCT_SIZE;
}

size_t _num_allocated_blocks()
{
    return data_list.allocd_blocks_num + mmap_data_list.allocd_blocks_num;
}

size_t _num_allocated_bytes()
{
    return data_list.allocd_bytes_num - data_list.allocd_blocks_num*STRUCT_SIZE \
             + mmap_data_list.allocd_bytes_num - mmap_data_list.allocd_blocks_num*STRUCT_SIZE;
}

size_t _num_meta_data_bytes()
{
    return _num_allocated_blocks()*STRUCT_SIZE;
}

size_t _size_meta_data()
{
    return STRUCT_SIZE;
}


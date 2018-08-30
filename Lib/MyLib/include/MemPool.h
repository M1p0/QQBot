#pragma once
#include <list>
#include <mutex>
using namespace std;
struct  Buffer
{
    bool bUsed;
    size_t uBufferSize;
    void* pBufferAddress;
    Buffer() :bUsed(false), uBufferSize(0), pBufferAddress(nullptr) {};
};

class MemPool
{
public:
    MemPool();
    ~MemPool();
    void* Allocate(size_t uSize);
    void Deallocate(void *p);
    void SetBlockSize(size_t uSize);
    size_t Capacity();
private:
    mutex mtx;
    list<Buffer> BufferList;
    size_t uBlockSize;

};

inline void* operator new(size_t uSize, MemPool &mempool)
{
    return mempool.Allocate(uSize);
};




template<class T> inline void Destroy(T *p, MemPool &mempool)
{
    if (p)
    {
        p->~T();
        mempool.Deallocate(p);
    }
};

inline void operator delete(void *p, MemPool &mempool)
{
    Destroy(p, mempool);
}

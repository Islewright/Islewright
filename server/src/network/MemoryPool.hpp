#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <vector>
#include <queue>
#include <mutex>


struct BufferBlock
{
    char* memoryPtr = nullptr;
    size_t blockId = 0;

    BufferBlock(char* pMemoryPos, size_t id) 
    : memoryPtr(pMemoryPos), blockId(id) {}
};

class MemoryPool
{
    public:
    MemoryPool(const size_t blockCount,  const size_t blockSize = 1024)
    {
        buffer_.resize(blockCount * blockSize);

        for(int i = 0; i < blockCount; i++)
        {
            blocks_.push(BufferBlock(buffer_.data() + (i * blockSize), i));
        }
    }

    ~MemoryPool() = default;

    // Return memory block to memory pool
    void Enqueue(BufferBlock newBlock)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        blocks_.push(newBlock);
    }


    // Acquire free memory block from memory pool
    BufferBlock Dequeue()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if(blocks_.empty()) return BufferBlock(nullptr,0);

        BufferBlock block = blocks_.front();
        blocks_.pop();
        return block;
    }

    private:
    std::queue<BufferBlock> blocks_;

    std::vector<char> buffer_;

    std::mutex mutex_;
};


#endif
#ifndef EXECUTOR_CONTEXT_HPP
#define EXECUTOR_CONTEXT_HPP

#include "storage/my_custom_buffer_pool_manager.hpp"

class ExecutorContext {
private:
    BufferPoolManager &bpm_;

public:
    explicit ExecutorContext(BufferPoolManager &bpm) : bpm_(bpm) {}
    ~ExecutorContext() = default;

    // Disallow copying for memory safety
    ExecutorContext(const ExecutorContext&) = delete;
    ExecutorContext& operator=(const ExecutorContext&) = delete;

    BufferPoolManager* GetBufferPoolManager() { return &bpm_; }
};

#endif
#ifndef ABSTRACT_EXECUTOR_HPP
#define ABSTRACT_EXECUTOR_HPP

#include "types/tuple.hpp"
#include "types/value.hpp"
#include "schema.hpp"
#include "executor_context.hpp"
#include <memory>

class AbstractExecutor {
protected:
    ExecutorContext *exec_ctx_;

public:
    explicit AbstractExecutor(ExecutorContext *exec_ctx) : exec_ctx_(exec_ctx) {}
    virtual ~AbstractExecutor() = default;

    /**
     * Initializes the executor. 
     * Inside this method, internal states (like pointers or counters) should be reset.
     */
    virtual void Init() = 0;

    /**
     * Yields the next tuple from this operator.
     * @param tuple The next tuple produced by this operator (output parameter)
     * @param rid The physical record identifier of the returned tuple (output parameter)
     * @return true if a tuple was successfully produced, false if there are no more tuples (EOF)
     */
    virtual bool Next(Tuple *tuple, RID *rid) = 0;

    /** @return The schema of the tuples produced by this executor */
    virtual const Schema& GetOutputSchema() const = 0;
};

#endif
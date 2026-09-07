#include "../src/types/schema.hpp"
#include "../src/executor_context.hpp"
#include "../src/executors/mock_executor.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void TestVolcanoPipeline() {
    // 1. Setup minimum context requirements
    std::string filename = "mock_db.bin";
    DiskManager disk(filename);
    BufferPoolManager bpm(disk);
    ExecutorContext exec_ctx(bpm);

    // 2. Setup a basic 1-column schema (ID Only)
    std::vector<Column> cols = {{"id", TypeId::INT32, 4, 0}};
    Schema schema(cols);

    // 3. Instantiate MockExecutor to yield 5 records (0 through 4)
    MockExecutor mock_exec(&exec_ctx, schema, 5);
    mock_exec.Init();

    Tuple res_tuple;
    RID res_rid;

    // Pull first tuple
    assert(mock_exec.Next(&res_tuple, &res_rid) == true);
    assert(res_tuple.GetInt32(schema, 0) == 0);

    // Pull second tuple
    assert(mock_exec.Next(&res_tuple, &res_rid) == true);
    assert(res_tuple.GetInt32(schema, 0) == 1);

    // Skip two tuples
    mock_exec.Next(&res_tuple, &res_rid);
    mock_exec.Next(&res_tuple, &res_rid);

    // Pull fifth tuple
    assert(mock_exec.Next(&res_tuple, &res_rid) == true);
    assert(res_tuple.GetInt32(schema, 0) == 4);

    // Next pull should return false (EOF)
    assert(mock_exec.Next(&res_tuple, &res_rid) == false);

    // Test Init resets pipeline state
    mock_exec.Init();
    assert(mock_exec.Next(&res_tuple, &res_rid) == true);
    assert(res_tuple.GetInt32(schema, 0) == 0); // Back to start!

    std::cout << "Day 3 Volcano Base Interface Tests Passed Successfully!" << std::endl;
}

int main() {
    TestVolcanoPipeline();
    return 0;
}
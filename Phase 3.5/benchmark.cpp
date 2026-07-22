#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <cstdio>

#include "custom_catalog.hpp"
#include "abstract_expression.hpp"

// Benchmark Metrics Helper
struct BenchmarkResult {
    std::string name;
    size_t total_ops;
    double total_time_ms;
    double avg_time_ms;
    double min_time_ms;
    double max_time_ms;
};

void PrintBenchmarkResult(const BenchmarkResult &res) {
    std::cout << "========================================================\n";
    std::cout << " Benchmark: " << res.name << "\n";
    std::cout << " Total Operations: " << res.total_ops << "\n";
    std::cout << " Total Time      : " << std::fixed << std::setprecision(3) << res.total_time_ms << " ms\n";
    std::cout << " Avg Time / Op   : " << std::fixed << std::setprecision(5) << res.avg_time_ms << " ms\n";
    std::cout << " Min Time / Op   : " << std::fixed << std::setprecision(5) << res.min_time_ms << " ms\n";
    std::cout << " Max Time / Op   : " << std::fixed << std::setprecision(5) << res.max_time_ms << " ms\n";
    std::cout << "========================================================\n\n";
}

BenchmarkResult ComputeMetrics(const std::string &name, const std::vector<double> &latencies_ms) {
    BenchmarkResult res;
    res.name = name;
    res.total_ops = latencies_ms.size();

    if (latencies_ms.empty()) return res;

    res.total_time_ms = 0.0;
    res.min_time_ms = latencies_ms[0];
    res.max_time_ms = latencies_ms[0];

    for (double lat : latencies_ms) {
        res.total_time_ms += lat;
        if (lat < res.min_time_ms) res.min_time_ms = lat;
        if (lat > res.max_time_ms) res.max_time_ms = lat;
    }

    res.avg_time_ms = res.total_time_ms / res.total_ops;
    return res;
}

// Predefined Expression Generator (Where Clause Builder)
std::unique_ptr<AbstractExpression> BuildRandomPredicate(std::mt19937 &gen) {
    std::uniform_int_distribution<int> expr_dist(0, 2);
    std::uniform_int_distribution<int> val_dist(1, 10000);

    int type = expr_dist(gen);
    int target_val = val_dist(gen);

    if (type == 0) {
        // Condition: col 0 (id) == target_val
        return std::make_unique<EqualExpression>(
            std::make_unique<ColumnValueExpression>(0),
            std::make_unique<ConstantValueExpression>(Value(target_val))
        );
    } else if (type == 1) {
        // Condition: col 1 (age) > target_val
        return std::make_unique<ComparisonExpression>(
            std::make_unique<ColumnValueExpression>(0),
            std::make_unique<ConstantValueExpression>(Value(target_val))
        );
    } else {
        // Condition: col 1 (age) < target_val
        return std::make_unique<ComparisonLesserExpression>(
            std::make_unique<ColumnValueExpression>(0),
            std::make_unique<ConstantValueExpression>(Value(target_val))
        );
    }
}

int main() {
    const std::string db_filename = "benchmark_cpp.db";
    std::remove(db_filename.c_str());

    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> table_dist(0, 49);
    std::uniform_int_distribution<int> val_dist(1, 50000);

    // Setup 50 tables with identical schema
    std::vector<std::string> tables;
    for (int i = 0; i < 50; ++i) {
        tables.push_back("table_" + std::to_string(i));
    }

    std::vector<Column> cols = {
        {const_cast<char *>("id"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("age"), TypeId::INT32, 4, 0, 0},
        {const_cast<char *>("name"), TypeId::VARCHAR, 20, 0, 0}
    };
    Schema schema(cols);

    {
        catalog cata(db_filename);
        std::cout << "Creating 50 tables in catalog..." << std::endl;
        for (const auto &tname : tables) {
            cata.createTable(tname, schema);
        }
    }

    catalog cata(db_filename);
    std::vector<std::string> query_cols = {"id", "age", "name"};

    // -------------------------------------------------------------
    // Workload 1: 10k Inserts
    // -------------------------------------------------------------
    {
        std::vector<double> latencies;
        latencies.reserve(10000);

        for (int i = 1; i <= 10000; ++i) {
            std::string tname = tables[table_dist(gen)];
            std::vector<std::vector<Value>> row = {{Value(i), Value(val_dist(gen)), Value("User_" + std::to_string(i))}};

            auto start = std::chrono::high_resolution_clock::now();
            cata.InsertRow(tname, row);
            auto end = std::chrono::high_resolution_clock::now();

            latencies.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        }
        PrintBenchmarkResult(ComputeMetrics("1. 10k Sequential Inserts", latencies));
    }

    // -------------------------------------------------------------
    // Workload 2: 10k Reads
    // -------------------------------------------------------------
    {
        std::vector<double> latencies;
        latencies.reserve(10000);

        for (int i = 0; i < 10000; ++i) {
            std::string tname = tables[table_dist(gen)];
            auto pred = BuildRandomPredicate(gen);

            auto start = std::chrono::high_resolution_clock::now();
            cata.Query(tname, query_cols, std::move(pred));
            auto end = std::chrono::high_resolution_clock::now();

            latencies.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        }
        PrintBenchmarkResult(ComputeMetrics("2. 10k Reads", latencies));
    }

    // -------------------------------------------------------------
    // Workload 3: 10k Updates
    // -------------------------------------------------------------
    {
        std::vector<double> latencies;
        latencies.reserve(10000);

        for (int i = 0; i < 10000; ++i) {
            std::string tname = tables[table_dist(gen)];
            auto pred = BuildRandomPredicate(gen);
            std::vector<std::pair<std::string, Value>> update_vals = {{"age", Value(val_dist(gen))}};

            auto start = std::chrono::high_resolution_clock::now();
            cata.UpdateRow(tname, update_vals, std::move(pred));
            auto end = std::chrono::high_resolution_clock::now();

            latencies.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        }
        PrintBenchmarkResult(ComputeMetrics("3. 10k Updates", latencies));
    }

    // -------------------------------------------------------------
    // Workload 4: 10k Deletes
    // -------------------------------------------------------------
    {
        std::vector<double> latencies;
        latencies.reserve(10000);

        for (int i = 0; i < 10000; ++i) {
            std::string tname = tables[table_dist(gen)];
            auto pred = BuildRandomPredicate(gen);

            auto start = std::chrono::high_resolution_clock::now();
            cata.DeleteRow(tname, std::move(pred));
            auto end = std::chrono::high_resolution_clock::now();

            latencies.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        }
        PrintBenchmarkResult(ComputeMetrics("4. 10k Deletes", latencies));
    }

    // -------------------------------------------------------------
    // Workload 5: 10k Insert + Read Alternate (5k insert, 5k read)
    // -------------------------------------------------------------
    {
        std::vector<double> latencies;
        latencies.reserve(10000);
        int current_id = 100001;

        for (int i = 0; i < 5000; ++i) {
            // 1. Insert
            std::string tname_ins = tables[table_dist(gen)];
            std::vector<std::vector<Value>> row = {{Value(current_id++), Value(val_dist(gen)), Value("AltUser")}};

            auto start_ins = std::chrono::high_resolution_clock::now();
            cata.InsertRow(tname_ins, row);
            auto end_ins = std::chrono::high_resolution_clock::now();
            latencies.push_back(std::chrono::duration<double, std::milli>(end_ins - start_ins).count());

            // 2. Read
            std::string tname_read = tables[table_dist(gen)];
            auto pred = BuildRandomPredicate(gen);

            auto start_read = std::chrono::high_resolution_clock::now();
            cata.Query(tname_read, query_cols, std::move(pred));
            auto end_read = std::chrono::high_resolution_clock::now();
            latencies.push_back(std::chrono::duration<double, std::milli>(end_read - start_read).count());
        }
        PrintBenchmarkResult(ComputeMetrics("5. 10k Alternating Insert & Read", latencies));
    }

    // -------------------------------------------------------------
    // Workload 6: 50k Random Insert, Update, Read, Delete
    // -------------------------------------------------------------
    {
        std::vector<double> latencies;
        latencies.reserve(50000);
        std::uniform_int_distribution<int> op_dist(0, 3);
        int current_id = 200001;

        for (int i = 0; i < 50000; ++i) {
            std::string tname = tables[table_dist(gen)];
            int op = op_dist(gen);

            auto start = std::chrono::high_resolution_clock::now();

            if (op == 0) { // Insert
                std::vector<std::vector<Value>> row = {{Value(current_id++), Value(val_dist(gen)), Value("RandUser")}};
                cata.InsertRow(tname, row);
            } else if (op == 1) { // Read
                auto pred = BuildRandomPredicate(gen);
                cata.Query(tname, query_cols, std::move(pred));
            } else if (op == 2) { // Update
                auto pred = BuildRandomPredicate(gen);
                std::vector<std::pair<std::string, Value>> update_vals = {{"age", Value(val_dist(gen))}};
                cata.UpdateRow(tname, update_vals, std::move(pred));
            } else { // Delete
                auto pred = BuildRandomPredicate(gen);
                cata.DeleteRow(tname, std::move(pred));
            }

            auto end = std::chrono::high_resolution_clock::now();
            latencies.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        }
        PrintBenchmarkResult(ComputeMetrics("6. 50k Random Workload (Insert, Read, Update, Delete)", latencies));
    }

    std::remove(db_filename.c_str());
    return 0;
}
import sqlite3
import time
import random
import os

DB_FILENAME = "benchmark_sqlite.db"

def remove_db():
    if os.path.exists(DB_FILENAME):
        os.remove(DB_FILENAME)

def print_result(name, latencies_ms):
    total_ops = len(latencies_ms)
    total_time = sum(latencies_ms)
    avg_time = total_time / total_ops if total_ops > 0 else 0
    min_time = min(latencies_ms) if total_ops > 0 else 0
    max_time = max(latencies_ms) if total_ops > 0 else 0

    print("========================================================")
    print(f" Benchmark: {name}")
    print(f" Total Operations: {total_ops}")
    print(f" Total Time      : {total_time:.3f} ms")
    print(f" Avg Time / Op   : {avg_time:.5f} ms")
    print(f" Min Time / Op   : {min_time:.5f} ms")
    print(f" Max Time / Op   : {max_time:.5f} ms")
    print("========================================================\n")

def get_random_predicate():
    expr_type = random.randint(0, 2)
    val = random.randint(1, 10000)
    if expr_type == 0:
        return f"id = {val}"
    elif expr_type == 1:
        return f"age > {val}"
    else:
        return f"age < {val}"

def run_sqlite_benchmark():
    remove_db()
    
    # autocommit mode (isolation_level=None) to measure individual query latency without holding open transactions
    conn = sqlite3.connect(DB_FILENAME, isolation_level=None)
    cursor = conn.cursor()

    tables = [f"table_{i}" for i in range(50)]
    
    print("Creating 50 tables in SQLite...")
    for tname in tables:
        cursor.execute(f"CREATE TABLE {tname} (id INTEGER PRIMARY KEY, age INTEGER, name TEXT)")

    random.seed(42)

    # -------------------------------------------------------------
    # Workload 1: 10k Inserts
    # -------------------------------------------------------------
    latencies = []
    for i in range(1, 10001):
        tname = random.choice(tables)
        age = random.randint(1, 50000)
        name = f"User_{i}"
        
        start = time.perf_counter()
        cursor.execute(f"INSERT OR IGNORE INTO {tname} (id, age, name) VALUES (?, ?, ?)", (i, age, name))
        end = time.perf_counter()
        latencies.append((end - start) * 1000.0)
    print_result("1. 10k Sequential Inserts", latencies)

    # -------------------------------------------------------------
    # Workload 2: 10k Reads
    # -------------------------------------------------------------
    latencies = []
    for _ in range(10000):
        tname = random.choice(tables)
        where_clause = get_random_predicate()
        
        start = time.perf_counter()
        cursor.execute(f"SELECT id, age, name FROM {tname} WHERE {where_clause}")
        cursor.fetchall()
        end = time.perf_counter()
        latencies.append((end - start) * 1000.0)
    print_result("2. 10k Reads", latencies)

    # -------------------------------------------------------------
    # Workload 3: 10k Updates
    # -------------------------------------------------------------
    latencies = []
    for _ in range(10000):
        tname = random.choice(tables)
        where_clause = get_random_predicate()
        new_age = random.randint(1, 50000)
        
        start = time.perf_counter()
        cursor.execute(f"UPDATE {tname} SET age = ? WHERE {where_clause}", (new_age,))
        end = time.perf_counter()
        latencies.append((end - start) * 1000.0)
    print_result("3. 10k Updates", latencies)

    # -------------------------------------------------------------
    # Workload 4: 10k Deletes
    # -------------------------------------------------------------
    latencies = []
    for _ in range(10000):
        tname = random.choice(tables)
        where_clause = get_random_predicate()
        
        start = time.perf_counter()
        cursor.execute(f"DELETE FROM {tname} WHERE {where_clause}")
        end = time.perf_counter()
        latencies.append((end - start) * 1000.0)
    print_result("4. 10k Deletes", latencies)

    # -------------------------------------------------------------
    # Workload 5: 10k Insert + Read Alternate (5k each)
    # -------------------------------------------------------------
    latencies = []
    current_id = 100001
    for _ in range(5000):
        # Insert
        tname_ins = random.choice(tables)
        age = random.randint(1, 50000)
        start_ins = time.perf_counter()
        cursor.execute(f"INSERT OR IGNORE INTO {tname_ins} (id, age, name) VALUES (?, ?, ?)", (current_id, age, "AltUser"))
        end_ins = time.perf_counter()
        latencies.append((end_ins - start_ins) * 1000.0)
        current_id += 1

        # Read
        tname_read = random.choice(tables)
        where_clause = get_random_predicate()
        start_read = time.perf_counter()
        cursor.execute(f"SELECT id, age, name FROM {tname_read} WHERE {where_clause}")
        cursor.fetchall()
        end_read = time.perf_counter()
        latencies.append((end_read - start_read) * 1000.0)

    print_result("5. 10k Alternating Insert & Read", latencies)

    # -------------------------------------------------------------
    # Workload 6: 50k Random Workload
    # -------------------------------------------------------------
    latencies = []
    current_id = 200001
    for _ in range(50000):
        tname = random.choice(tables)
        op = random.randint(0, 3)

        start = time.perf_counter()
        if op == 0: # Insert
            age = random.randint(1, 50000)
            cursor.execute(f"INSERT OR IGNORE INTO {tname} (id, age, name) VALUES (?, ?, ?)", (current_id, age, "RandUser"))
            current_id += 1
        elif op == 1: # Read
            where_clause = get_random_predicate()
            cursor.execute(f"SELECT id, age, name FROM {tname} WHERE {where_clause}")
            cursor.fetchall()
        elif op == 2: # Update
            where_clause = get_random_predicate()
            new_age = random.randint(1, 50000)
            cursor.execute(f"UPDATE {tname} SET age = ? WHERE {where_clause}", (new_age,))
        else: # Delete
            where_clause = get_random_predicate()
            cursor.execute(f"DELETE FROM {tname} WHERE {where_clause}")
        end = time.perf_counter()
        
        latencies.append((end - start) * 1000.0)

    print_result("6. 50k Random Workload (Insert, Read, Update, Delete)", latencies)

    conn.close()
    remove_db()

if __name__ == "__main__":
    run_sqlite_benchmark()
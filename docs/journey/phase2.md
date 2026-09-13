# Phase 2 — The Volcano Model

<span class="phase-badge">8 days</span>

> *"The upper layers have the responsibility to perform the necessary complex operations on any data passed from the base, according to a set of rules that are designed to always be expandable."*

---

## What this phase built

An **abstract executor pipeline** — the architecture used by real production databases (PostgreSQL, MySQL).
Every operation (scan, filter, project) is a node in a tree that calls a single `Next()` to pull the next row.

---

## Day 1 — Schemas

No more hardcoded structs.
A **Schema** is a list of **Columns**, each with a name, type, byte offset, and length.
Two types were introduced: `INT32` and `VARCHAR`.

```cpp
enum class TypeId { INT32, VARCHAR };

struct Column {
    std::string name;
    TypeId type;
    uint32_t length;
    uint32_t offset; // byte offset within a tuple
};

class Schema {
    std::vector<Column> columns;
    uint32_t tuple_size;
};
```

---

## Day 2 — Tuples & RIDs

A **Tuple** is a zero-copy view into a page's raw bytes — no allocation, just a pointer and a size.
An **RID** (Record ID) uniquely identifies a record by `(page_id, slot_num)`.

```cpp
struct RID { uint32_t page_id, slot_num; };

class Tuple {
    const uint8_t* data_ptr_;
    uint32_t size_;
    RID rid_;
public:
    int32_t     GetInt32(const Schema&, uint32_t col_idx);
    std::string GetVarchar(const Schema&, uint32_t col_idx);
};
```

---

## Day 3 — The Heart of the Volcano

Three virtual base classes define every executor and expression:

```cpp
class AbstractExecutor {
    virtual void Init() = 0;
    virtual bool Next(Tuple*, RID*) = 0;        // pull next row
    virtual const Schema& GetOutputSchema() = 0;
};

class AbstractExpression {
    virtual Value Evaluate(const Tuple*, const Schema&) = 0;
};
```

A `Value` wraps `int32_t` or `std::string` behind a uniform interface, powered by a union + `TypeId` tag.

---

## Day 4 — Sequential Scanner

`SeqScanExecutor` pages through the disk in order, yielding one tuple per `Next()` call — the base source node for any query.

---

## Day 5 — Abstract Expressions

Three expression types power the `WHERE` clause:

| Expression | Role |
|---|---|
| `ConstantValueExpression` | A literal value, e.g. `21` |
| `ColumnValueExpression` | A column reference, e.g. `table.age` |
| `ComparisonExpression` | Binary comparison between two child expressions |

All expressions compose via `unique_ptr` — arbitrarily deep nesting for free.

---

## Day 6 — Filter Executor

`FilterExecutor` wraps any child executor and drops rows that don't satisfy a predicate.
This is the `WHERE` clause materialised.

---

## Day 7 — Projection Executor

`ProjectionExecutor` takes a list of column expressions and a target schema, remaps columns from the child row into a fresh tuple.
This is the `SELECT col1, col2` materialised.

---

## Day 8 — End-to-End Pipeline

Full `SELECT age, id FROM ... WHERE age > 26` pipeline assembled:

```
ProjectionExecutor
    └── FilterExecutor  (age > 26)
            └── SeqScanExecutor
```

```
Init → Next → Next → ... → Next (EOF)
```

<!-- SCREENSHOT: Phase 2 end-to-end pipeline test output in terminal -->
<div class="screenshot-placeholder">
  📸 <strong>Screenshot coming soon</strong> — Phase 2 end-to-end pipeline passing
</div>

---

!!! success "Phase 2 complete"
    A fully generic, composable executor pipeline. Any query can be expressed as a tree of nodes — exactly how PostgreSQL does it.

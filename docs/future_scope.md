# Future Scope

`bsql` is an educational v1.0 — intentionally kept within scope to stay learnable.
But there's a clear roadmap of what a v2.0 could look like.

---

## Query Engine Extensions

### Joins
Currently `bsql` only supports single-table queries.
A natural extension would be adding `JOIN` support — starting with a simple **nested loop join** and eventually a **hash join** for larger datasets.

```sql
-- not yet supported
SELECT u.name, o.amount
FROM users u
JOIN orders o ON u.id = o.user_id;
```

### Aggregation
`GROUP BY`, `HAVING`, and aggregate functions (`COUNT`, `SUM`, `AVG`, `MIN`, `MAX`) would make the query engine significantly more useful.

### Subqueries
Allowing queries inside WHERE clauses or as table sources.

### Foreign Keys
Referential integrity — enforcing that a value in one table's column must exist in another table's primary key.

---

## Storage & Reliability

### Write-Ahead Logging (WAL)
Currently if the process crashes mid-write, data may be corrupted.
WAL is the standard solution — all changes are written to a log first, then applied.
This makes the database **crash-safe**.

### Transactions (ACID)
`BEGIN`, `COMMIT`, `ROLLBACK` — atomic multi-statement operations.
Requires WAL + a transaction manager.

### Better Page Management
The current tombstone-based deletion leaves fragmented space.
A **vacuum / compaction** pass could reclaim deleted slot space within pages.

### Larger VARCHAR Support
Currently strings have a fixed max length per column.
Supporting truly variable-length values across page boundaries would require an overflow page mechanism.

---

## Parser Improvements

### `SELECT *` Instead of `SELECT ALL`
The `*` shorthand for all columns was avoided during development to keep the lexer simple.
It's a straightforward addition.

### Expression Depth
Currently WHERE clauses are flat conjunctions / disjunctions.
Parenthesised sub-expressions (e.g., `WHERE (a > 1 AND b < 5) OR c = 3`) would require a proper recursive descent parser.

### `DISTINCT`
Deduplication of result rows.

### `LIMIT` + `OFFSET`
`OFFSET` (skip first N rows) alongside the existing `LIMIT`.

---

## Application

### A TUI Dashboard
Replace the current single-buffer editor with a split-pane terminal UI:
a query panel on top, results table below, using a library like `ftxui`.

### Browser-Based UI
A small local HTTP server wrapping the Compiler + a React frontend — turning `bsql` into a browser-accessible SQL workbench.

### Index Support (B-Tree)
A B-Tree index on a column would make point queries and range queries dramatically faster than full sequential scans.

---

!!! note "Contributions welcome"
    This project was built as a solo learning exercise, but the architecture is modular enough that any of the above features can be added as a standalone phase. PRs are open.

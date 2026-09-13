# Features

A reference for everything `bsql` supports in its current `v1.0.0` release.

---

## The 13 SQL Commands

### Database Commands

```sql
-- 1. List all databases (.db files) in the current directory
SHOW DATABASES;

-- 2. Create a new database file
CREATE DATABASE "db_name";

-- 3. Connect to a database (must be done before any table commands)
LINK DATABASE "db_name";

-- 4. Disconnect from the current database
UNLINK DATABASE;

-- 5. Delete a database file
DELETE DATABASE "db_name";
```

### Table Commands

```sql
-- 6. List all tables in the linked database
SHOW TABLES;

-- 7. Show the column schema of a table
SHOW TABLE SCHEMA "table_name";

-- 8. Delete an entire table and its data
DELETE TABLE "table_name";

-- 9. Create a table with a schema and a primary key
CREATE TABLE "table_name" (
    "col1_name" TYPE SIZE,
    "col2_name" TYPE SIZE,
    ...,
    PRIMARY KEY ("col1_name", ...)
);
```

!!! note "Types"
    - `INT` — 32-bit signed integer
    - `VARCHAR SIZE` — variable-length string, e.g. `VARCHAR 64`

### Data Commands

```sql
-- 10. Insert one or more rows (atomic — all or nothing)
INSERT INTO "table_name" VALUES (val1, val2, ...), (val1, val2, ...), ...;

-- 11. Select data with optional filtering, ordering, and limit
SELECT "col1", "col2", ...
FROM "table_name"
WHERE <clause1> <AND|OR> <clause2> ...
ORDERBY "output_col" <ASC|DESC>, ...
LIMIT <n>;

-- Or select all columns:
SELECT ALL FROM "table_name";

-- 12. Delete rows matching a condition
DELETE FROM "table_name"
WHERE <clause1> <AND|OR> <clause2> ...;

-- 13. Update specific columns on matching rows
UPDATE "table_name"
SET "col1" = val1, "col2" = val2, ...
WHERE <clause1> <AND|OR> <clause2> ...;
```

---

## WHERE Clause Operators

| Operator | Meaning |
|---|---|
| `=` | Equal |
| `!=` | Not equal |
| `>` | Greater than |
| `<` | Less than |
| `>=` | Greater than or equal |
| `<=` | Less than or equal |
| `STARTSWITH "prefix"` | String prefix match |
| `ENDSWITH "suffix"` | String suffix match |
| `AND` | Logical AND |
| `OR` | Logical OR |
| `!` | Logical NOT |

---

## Editor Keybindings

| Key | Mode | Action |
|---|---|---|
| `i` | Normal | Enter Insert mode |
| `Esc` | Insert | Return to Normal mode |
| `Enter` | Insert | New line (multi-line query support) |
| `:e` + `Enter` | Normal | Execute current query buffer |
| `:q` + `Enter` | Normal | Quit |

---

## Architecture Highlights

| Component | Technique |
|---|---|
| Storage | 4 KB pages, offset-based serialisation, `[[gnu::packed]]` |
| Caching | LRU buffer pool with dirty-bit write-back |
| Execution | Volcano pull-model (Init / Next / Next …) |
| Metadata | Single-file forest of linked page lists |
| Atomicity | All-or-nothing batch insert, primary key enforcement |
| Keyword parsing | Trie for O(k) keyword recognition |
| Expression parsing | Reverse Polish Notation for WHERE clauses |
| Deletion | Tombstone pattern for pages and slots |

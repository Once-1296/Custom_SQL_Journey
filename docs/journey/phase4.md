# Phase 4 — The Parser

<span class="phase-badge">4 days · Automata PTSD</span>

> *"Input String → BLACKBOX → Output result/error."*

---

## What this phase built

The **language layer** — turning raw text typed by a user into Catalog operations.
Three components form the pipeline: **Lexer → Interpreter → Compiler**.

---

## The Pipeline

```
User types:  SELECT name FROM users WHERE age > 21;

  Lexer          →  [SELECT][name][FROM][users][WHERE][age][>][21][;]
  Interpreter    →  "This is command #10 — selectQry"
  Compiler       →  validateQuery() → catalog.Query(...)
                                            ↓
                                     prints result table
```

Every step passes a `std::string& Message` reference. If any step fails, the error message propagates back and is shown to the user — no crash, just a helpful error and a new prompt.

---

## Day 1 — The Lexer

The lexer converts a raw query string into a `vector<Token>`.

### Token Types

| Type | Example |
|---|---|
| `KEYWORD` | `SELECT`, `FROM`, `WHERE`, `CREATE`, … |
| `STR` | column names, identifiers |
| `FORCE_STR` | `"user string"` — enclosed in double quotes |
| `INT` | `-42`, `100` |
| `OPERATOR` | `>`, `<=`, `!=`, `AND`, `OR`, `STARTSWITH`, `ENDSWITH` |
| `BRACKET_OPEN/CLOSE` | `(` `)` |
| `COMMA` | `,` |

### Keyword Recognition — Trie

Keywords are stored in a **Trie** (prefix tree) for O(k) lookup (k = keyword length), avoiding a linear scan through a keyword list on every token.

```cpp
class tokenTrie {
    tokenTrie* children[26];
    std::optional<Token> value;
public:
    void insert(const char* keyword, Token& token);
    const std::optional<Token> getValue() const;
};
```

!!! note
    Instead of `SELECT *`, this engine uses `SELECT ALL`. The parser uses a stack-like approach to decide token type based on the previous character, with one-character lookahead for negative integers.

---

## Day 2 — Commands & The Compiler

The **Compiler** class is the single entry point: it holds the active catalog pointer, the working directory, and the lexer instance.

```cpp
class Compiler {
    catalog* cata = nullptr;
    lexer Lexer;
    std::string root_path;
public:
    bool query(std::string& query); // master function
};
```

For each query string, `query()` calls Lexer → Interpreter → Caller in sequence, stopping and reporting the error message if any step returns false.

---

## Day 3 — Table Commands (commands 5–8)

```
SHOW TABLES;
SHOW TABLE SCHEMA "tablename";
CREATE TABLE "tablename" ("col" TYPE SIZE, ..., PRIMARY KEY ("col"));
DELETE TABLE "tablename";
```

Each command verifies that a catalog is currently linked, validates column names (alphanumeric + underscore, within length limits), then delegates to the Catalog.

---

## Day 4 — Row Commands (commands 9–12)

The four data manipulation commands, each parsed in its own header file:

**`INSERT INTO "t" VALUES (...), (...);`**  
Schema match check → atomically insert all rows or none.

**`SELECT col1, col2 FROM "t" WHERE ... ORDERBY col ASC LIMIT n;`**  
Five clauses — SELECT and FROM mandatory, the rest optional.  
WHERE is parsed using **Reverse Polish Notation** to build the `AbstractExpression` tree the Catalog understands.

**`DELETE FROM "t" WHERE ...;`**  
FROM + optional WHERE, same expression parsing as SELECT.

**`UPDATE "t" SET col = val, ... WHERE ...;`**  
SET clause validated (no repeated columns, no primary key columns touched), then optional WHERE.

### Clause Helpers

```cpp
// Build AbstractExpression from RPN token list
std::unique_ptr<AbstractExpression> whereHelper(...);

// Sort result rows
std::vector<std::pair<uint32_t, uint32_t>> orderByHelper(...);

// Truncate result rows
bool limitHelper(...);
```

---

<!-- SCREENSHOT: A full SELECT query with WHERE and ORDERBY in the bsql editor -->
<div class="screenshot-placeholder">
  📸 <strong>Screenshot coming soon</strong> — SELECT query with WHERE / ORDERBY in bsql
</div>

---

!!! success "Phase 4 complete"
    A full SQL front-end: Trie-based lexer, command interpreter, and callers for all 13 commands — with error reporting at every step.

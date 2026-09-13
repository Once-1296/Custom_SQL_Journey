# Project Journey — Overview

This project was built in **5 phases**, each one adding a new layer on top of the last.
Every phase has its own branch in the repository, so you can check out any individual stage.

---

## The 5 Phases

| Phase | Name | What it built |
|---|---|---|
| [1](phase1.md) | **The OS Background** | Serialisation, Page class, Disk Manager, LRU Buffer Pool |
| [2](phase2.md) | **The Volcano Model** | Schemas, Tuples, Abstract Executors, Expressions |
| [3](phase3.md) | **The System Catalog** | Full CRUD — tables, rows, schemas, metadata forest |
| [4](phase4.md) | **The Parser** | Lexer (Trie), Interpreter, Compiler, all 13 commands |
| [5](phase5.md) | **The Application** | CMake build, `bsql` CLI, vim-style modal editor |

---

## Architecture Overview

```
┌─────────────────────────────────────────────┐
│               bsql  (Phase 5)               │
│         CMake · CLI · Modal Editor          │
├─────────────────────────────────────────────┤
│            Compiler  (Phase 4)              │
│      Lexer → Interpreter → 13 Commands      │
├─────────────────────────────────────────────┤
│           System Catalog  (Phase 3)         │
│   CreateTable · Insert · Query · Update     │
│              Delete · Metadata              │
├─────────────────────────────────────────────┤
│         Volcano Executor Pipeline  (Phase 2)│
│   SeqScan → Filter → Projection             │
│   Schema · Tuple · AbstractExpression       │
├─────────────────────────────────────────────┤
│           Storage Layer  (Phase 1)          │
│   DiskManager · Page · BufferPoolManager    │
└─────────────────────────────────────────────┘
```

Each phase is described in detail in the following sections.

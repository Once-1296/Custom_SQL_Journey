# Custom SQL Journey

> A hand-built, educational SQL engine written in C++ — from raw bytes on disk all the way to an interactive terminal editor.

`bsql` is a custom, file-backed relational database engine built from scratch in C++20.
It implements the core internal machinery of a real SQL system:
a page-based disk manager, LRU buffer pool, tuple/schema abstraction, a Volcano-model executor pipeline, a system catalog, a hand-written lexer + interpreter, and a modal command-line editor — all without any external database library.

This project was a personal deep-dive to understand how SQL actually works under the hood.
It was neither easy nor fast, but every phase forced real learning: OS concepts, automata theory, DBMS internals, memory management, and large-scale C++ project structure all came together in a single codebase.

---

## Branch Map

| Branch | Contents |
|---|---|
| `main` | Latest source — complete 5-phase implementation |
| `phase-1` | OS background: serialisation, pages, disk manager, LRU buffer pool |
| `phase-2` | Volcano model: schemas, tuples, abstract executors, expressions |
| `phase-3` | System catalog: CRUD, metadata forest structure |
| `phase-4` | Parser: lexer (Trie), interpreter, compiler, all 13 SQL commands |
| `phase-5` | Application: CMake build, `bsql` CLI + modal editor |

*Note: Phases 3 and 4 have 2 branches, and the second one is the final implementation.*

Each branch contains an `src/` folder for the phase code and a `tests/` folder for unit tests.

For the full story of the journey — design decisions, code walkthroughs and architecture diagrams — refer to [`documentation/project.pdf`](documentation/project.pdf) or the [docs site](https://once-1296.github.io/Custom_SQL_Journey/).

---

## Installation & Setup

### Prerequisites

Ensure you have the following installed before proceeding:
- **Git**
- **CMake** (v3.16 or higher)
- A **C++20 compatible compiler**:
  - **Linux:** GCC 10+ or Clang 10+
  - **macOS:** Xcode Command Line Tools (Clang)
  - **Windows:** MSVC (Visual Studio 2019+) or MinGW-w64

---

### Step 1: Clone the Repository

```bash
git clone https://github.com/Once-1296/Custom_SQL_Journey.git
cd Custom_SQL_Journey
```

---

### Step 2: Build & Install

#### 🐧 Linux

```bash
# Create and navigate to build directory
mkdir build && cd build

# Configure and compile the project
cmake ..
cmake --build .

# (Optional) Install system-wide to run 'bsql' from anywhere
sudo cmake --install .
```

---

#### 🍎 macOS

```bash
# Ensure Xcode Command Line Tools are installed
xcode-select --install

# Create and navigate to build directory
mkdir build && cd build

# Configure and compile the project
cmake ..
cmake --build .

# (Optional) Install system-wide to run 'bsql' from anywhere
sudo cmake --install .
```

---

#### 🪟 Windows

##### Option A: Visual Studio (Developer Command Prompt / PowerShell)

```cmd
mkdir build
cd build

cmake ..
cmake --build . --config Release

# (Optional) Install to default path (requires Administrator prompt)
cmake --install . --config Release
```

*The `bsql.exe` binary will be available inside `build/Release/bsql.exe`.*

##### Option B: MinGW / MSYS2

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

---

### Step 3: Run

```bash
# Interactive editor — current directory as database root
./bsql               # Linux / macOS
.\Release\bsql.exe   # Windows (MSVC)
.\bsql.exe           # Windows (MinGW)

# If installed system-wide:
bsql

# Start with a specific database directory
bsql /path/to/directory

# List all supported SQL commands
bsql commands

# Explain a specific command (1–13)
bsql command <1-13>

# Version / help
bsql --version
bsql --help
```

#### Editor keybindings

| Key | Action |
|---|---|
| `i` | Enter insert mode |
| `Esc` | Return to normal mode |
| `:e` + Enter | Execute current multi-line query |
| `:q` + Enter | Quit |
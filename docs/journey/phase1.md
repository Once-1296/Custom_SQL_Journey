# Phase 1 — The OS Background

<span class="phase-badge">7 days</span>

> *"Data is often stored in raw bytes. So when we fetch it we can't use something like `struct.id`. Instead we use an offset variable."*

---

## What this phase built

The very bottom of the stack — the storage layer.
Before any SQL can run, someone has to handle reading and writing raw bytes to and from a file on disk efficiently.
Phase 1 built exactly that.

---

## Day 1 — Serialisation & Deserialisation

Structs in C++ use **padding** to align fields to memory boundaries — which means you can't simply dump a struct to disk and read it back correctly later.

The fix: treat everything as raw bytes and track each field's **offset manually** using `memcpy`.

```cpp
// Serialise
std::memcpy(buffer.data() + offset, &obj.name, sizeof(obj.name));
offset += sizeof(obj.name);
std::memcpy(buffer.data() + offset, &obj.id, sizeof(obj.id));
offset += sizeof(obj.id);

// Deserialise — same idea, source and destination swapped
std::memcpy(&obj.name, buffer.data() + offset, sizeof(obj.name));
```

The `[[gnu::packed]]` attribute was used on structs to prevent the compiler from silently inserting padding bytes.

---

## Days 2–4 — Page & Disk Manager

A **Page** is a fixed 4 096-byte block — matching the typical OS page size.
Every record lives inside a page; pages live inside a file that acts as our disk.

```cpp
class Page {
    std::array<uint8_t, 4096> data_buffer;
public:
    bool AppendRecord(const uint8_t* record_data, uint32_t record_size);
    const uint8_t* GetRecordPtr(uint32_t index, uint32_t record_size);
    uint32_t GetRecordCount() const;
};
```

The **DiskManager** treats one file as the entire disk.
Like a physical read/write head, it uses `seekg`/`seekp` to move a pointer across pages:

```cpp
class DiskManager {
    void WritePage(uint32_t page_id, const Page& page);
    void ReadPage(uint32_t page_id, Page& out_page);
    uint32_t AllocatePage();   // grows the file by one blank page
    uint32_t GetTotalPages();
};
```

---

## Day 5 — First Query

With pages and disk in place, a simple sequential scan over every page performed a `SELECT username, age WHERE age >= X` — a precursor to the full executor pipeline coming in Phase 2.

---

## Day 6 — Buffer Pool Manager (LRU Cache)

Going to disk every single time is expensive.
The **Buffer Pool Manager** acts as RAM — caching frequently accessed pages using an **LRU eviction policy**.

```
FetchPage(id)
  ├─ Hit?  → move to MRU front, return page
  └─ Miss? → evict LRU page (write back if dirty), load from disk
```

Key design: a `std::list` (MRU front → LRU back) paired with an `unordered_map` for O(1) lookup. A `dirty` flag ensures modified pages are flushed to disk before eviction.

---

## Day 7 — Integration Test

End-to-end test: write to disk → modify via BPM (mark dirty) → evict → verify data survived to disk.

<!-- SCREENSHOT: Phase 1 tests passing in terminal -->
<div class="screenshot-placeholder">
  📸 <strong>Screenshot coming soon</strong> — Phase 1 unit tests passing
</div>

---

!!! success "Phase 1 complete"
    A working page-based storage engine: serialisation, disk I/O, and an LRU buffer pool — all from scratch.

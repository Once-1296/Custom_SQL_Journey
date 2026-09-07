// catalog.hpp
#ifndef CATALOG_HPP
#define CATALOG_HPP

#include "storage/my_custom_buffer_pool_manager.hpp"
#include "table_heap.hpp"
#include "types/schema.hpp"
#include <unordered_map>
#include <memory>
#include <string>

struct TableMetadata {
    std::string name_;
    Schema schema_;
    std::unique_ptr<TableHeap> table_heap_;
    uint32_t first_page_id_;
};

class Catalog {
private:
    BufferPoolManager *bpm_;
    // Fast lookup directories
    std::unordered_map<std::string, std::unique_ptr<TableMetadata>> tables_;

public:
    Catalog(BufferPoolManager *bpm) : bpm_(bpm) {}

    // Adds a brand new table to the system directory
    bool CreateTable(const std::string &table_name, const Schema &schema) {
        if (tables_.find(table_name) != tables_.end()) {
            return false; // Table already exists!
        }

        // Utilize your hardened TableHeap constructor pattern.
        // Pass 0xFFFFFFFF as the page token to let the TableHeap take responsibility
        // for allocating and formatting its own new root page frame via the BPM.
        auto heap = std::make_unique<TableHeap>(bpm_, 0xFFFFFFFF);
        uint32_t root_pid = heap->GetFirstPageId();

        // Construct a TableMetadata instance tracking the name, schema,
        // unique pointer heap reference, and root page ID.
        auto metadata = std::make_unique<TableMetadata>();
        metadata->name_ = table_name;
        metadata->schema_ = schema;
        metadata->table_heap_ = std::move(heap);
        metadata->first_page_id_ = root_pid;

        // Map the metadata into your internal tables_ directory using the table_name string.
        tables_[table_name] = std::move(metadata);

        return true;
    }

    // Fetches the metadata of an existing table
    TableMetadata* GetTable(const std::string &table_name) {
        // Check if the table exists inside tables_. 
        // If it does, return a raw pointer to its metadata node. If not, return nullptr.
        auto it = tables_.find(table_name);
        if (it != tables_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
};

#endif
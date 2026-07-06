// slotted_page.hpp
#ifndef SLOTTED_PAGE_HPP
#define SLOTTED_PAGE_HPP

#include <iostream>
#include <array>
#include <cstring>
#include <cstdint>
#include <cassert>

class Page
{
private:
    std::array<uint8_t, 4096> data_buffer_;

    // Static layout definitions
    static constexpr size_t PAGE_SIZE = 4096;
    static constexpr size_t SLOT_COUNT_OFFSET = 0;   // 4 bytes
    static constexpr size_t FREE_SPACE_OFFSET = 4;   // 4 bytes
    static constexpr size_t NEXT_PAGE_ID_OFFSET = 8; // 4 bytes (New!)
    static constexpr size_t HEADER_SIZE = 12;        // 12 bytes total
    struct [[gnu::packed]] Slot
    {
        uint32_t offset_;
        uint32_t size_;
        Slot(uint32_t offset = 0, uint32_t size = 0) : offset_(offset), size_(size) {}
    };

    // track if defragmented or not
    bool is_defragmented = true;

public:
    Page()
    {
        Init();
    }
    void Init()
    {
        data_buffer_.fill(0);

        // Initially, there are 0 slots inside this block
        uint32_t initial_slots = 0;
        std::memcpy(data_buffer_.data() + SLOT_COUNT_OFFSET, &initial_slots, sizeof(uint32_t));

        // Initialize the free space pointer.
        // Because data grows BACKWARD from the absolute end of the page,
        // the initial free space pointer should point to byte 4096.
        uint32_t initial_free_space = PAGE_SIZE;
        std::memcpy(data_buffer_.data() + FREE_SPACE_OFFSET, &initial_free_space, sizeof(uint32_t));

        // set next_page_id by default to a sentinel value : 0xFFFFFFFF
        uint32_t sentinel_id = 0xFFFFFFFF;
        std::memcpy(data_buffer_.data() + NEXT_PAGE_ID_OFFSET, &sentinel_id, sizeof(uint32_t));

        is_defragmented = true;
    }

    uint32_t GetSlotCount() const
    {
        uint32_t count = 0;
        std::memcpy(&count, data_buffer_.data() + SLOT_COUNT_OFFSET, sizeof(uint32_t));
        return count;
    }

    uint32_t GetFreeSpacePointer() const
    {
        uint32_t ptr = 0;
        std::memcpy(&ptr, data_buffer_.data() + FREE_SPACE_OFFSET, sizeof(uint32_t));
        return ptr;
    }

    // Tries to insert a raw tuple into the page
    // if dead slot (0,0) claim it for insertion
    bool InsertTuple(const uint8_t *tuple_data, uint32_t size, uint32_t *out_slot_num)
    {
        uint32_t slot_count = GetSlotCount();
        uint32_t free_space_ptr = GetFreeSpacePointer();

        // std::cout<<"DEBUG : slot count "<<slot_count<<" , Free Space PTR "<<free_space_ptr<<std::endl;

        // Calculate space requirements
        // The space required for a new entry is the size of the raw tuple data PLUS
        // the size of one new Slot structure (8 bytes) in the header array.
        // Calculate the current space used by the header + slots array, and ensure
        uint32_t current_header_size = HEADER_SIZE + (sizeof(uint32_t) + sizeof(uint32_t)) * slot_count;
        // that adding this new tuple doesn't cause the slots array to crash into the free_space_ptr.
        // If it exceeds bounds, return false (Page Full).

        // std::cout<<"DEBUG : LHS "<<current_header_size + sizeof(uint32_t) + sizeof(uint32_t)<<" RHS "<<free_space_ptr - size<<std::endl;

        if (free_space_ptr < size || current_header_size + sizeof(uint32_t) + sizeof(uint32_t) > free_space_ptr - size)
        {
            if (!is_defragmented)
            {
                Defragment();
                return InsertTuple(tuple_data, size, out_slot_num);
            }
            return false;
        }
        // Write data backward
        // Calculate the new free space pointer location (free_space_ptr - size).
        // Use std::memcpy to copy 'tuple_data' into this newly claimed memory chunk.
        free_space_ptr = free_space_ptr - size;
        std::memcpy(data_buffer_.data() + free_space_ptr, reinterpret_cast<const char *>(tuple_data), size);
        // Write header slot entries forward
        // Construct a Slot struct capturing the new offset and size parameters.
        Slot new_slot(free_space_ptr, size);
        // std::cout << "Slot Offset: " << new_slot.offset_ << ", Slot Size: " << new_slot.size_ << std::endl; // Debugging line

        // Copy this Slot struct into the slots array position directly after the last slot entry.
        std::memcpy(data_buffer_.data() + HEADER_SIZE + (sizeof(uint32_t) + sizeof(uint32_t)) * slot_count, reinterpret_cast<const char *>(&new_slot), sizeof(new_slot));
        // Update Master Header Properties
        // Increment slot_count, update the free space pointer memory address,
        // assign *out_slot_num = current slot index, and return true.
        slot_count++;
        std::memcpy(data_buffer_.data() + SLOT_COUNT_OFFSET, &slot_count, sizeof(uint32_t));
        std::memcpy(data_buffer_.data() + FREE_SPACE_OFFSET, &free_space_ptr, sizeof(uint32_t));
        *out_slot_num = GetSlotCount() - 1;
        return true;
    }

    // Zero-copy lookup of data using a slot index
    const uint8_t *GetTuplePtr(uint32_t slot_num, uint32_t *out_size) const
    {
        assert(slot_num < GetSlotCount());

        // Extract Slot metadata properties at slot_num index.
        // Read the Slot layout out of the header array, assign its size to *out_size,
        // and return a direct pointer to its starting location inside data_buffer_.
        Slot slot;
        std::memcpy(&slot, data_buffer_.data() + HEADER_SIZE + (sizeof(uint32_t) + sizeof(uint32_t)) * slot_num, sizeof(slot));
        // std::cout << "Slot Offset: " << slot.offset_ << ", Slot Size: " << slot.size_ << std::endl; // Debugging line
        // std::cout << "Header Size " << HEADER_SIZE << std::endl;
        // std::cout << "Slot address: " << HEADER_SIZE + (sizeof(uint32_t)+sizeof(uint32_t))*slot_num << std::endl;
        std::memcpy(out_size, &slot.size_, sizeof(uint32_t));
        return data_buffer_.data() + slot.offset_;
    }

    const uint8_t *ReadRawData() const { return data_buffer_.data(); }
    uint8_t *WriteRawData() { return data_buffer_.data(); }

    void SetNextPageId(uint32_t next_page_id)
    {
        std::memcpy(data_buffer_.data() + NEXT_PAGE_ID_OFFSET, &next_page_id, sizeof(uint32_t));
    }
    uint32_t GetNextPageId() const
    {
        uint32_t next_page_id = 0;
        std::memcpy(&next_page_id, data_buffer_.data() + NEXT_PAGE_ID_OFFSET, sizeof(uint32_t));
        return next_page_id;
    }

    // Mark a slot as logically deleted by clearing its size field
    bool ApplyDelete(uint32_t slot_num)
    {
        uint32_t slot_count = GetSlotCount();
        if (slot_num >= slot_count)
            return false;

        // Extract the slot structure at slot_num index.
        // Change its size_ property to 0 to mark it as deleted.
        // Re-serialize the modified slot structure back into its correct offset in the header.
        uint32_t offset = HEADER_SIZE + slot_num * (sizeof(uint32_t) + sizeof(uint32_t));
        Slot slot;
        std::memcpy(&slot, data_buffer_.data() + offset, sizeof(slot));
        slot.size_ = 0;
        std::memcpy(data_buffer_.data() + offset, reinterpret_cast<const char *>(&slot), sizeof(slot));
        is_defragmented = false;
        return true;
    }

    // Updates a tuple in place if space permits
    bool UpdateTuple(uint32_t slot_num, const uint8_t *new_tuple_data, uint32_t new_size)
    {
        assert(slot_num < GetSlotCount());

        // Extract the slot layout metadata at slot_num
        // Scenario A: If new_size <= old_slot.size_, we can overwrite the existing memory
        // in-place! Update the slot's size_ property to match new_size, copy the raw bytes
        // into the existing offset location via std::memcpy, and return true.
        uint32_t offset = HEADER_SIZE + slot_num * (sizeof(uint32_t) + sizeof(uint32_t));
        Slot old_slot;
        std::memcpy(&old_slot, data_buffer_.data() + offset, sizeof(old_slot));
        if (new_size <= old_slot.size_)
        {
            is_defragmented = is_defragmented && (new_size == old_slot.size_);
            old_slot.size_ = new_size;
            std::memcpy(data_buffer_.data() + offset, reinterpret_cast<const char *>(&old_slot), sizeof(old_slot));
            std::memcpy(data_buffer_.data() + old_slot.offset_, new_tuple_data, new_size);
            return true;
        }

        // Scenario B: If new_size > old_slot.size_, it won't fit in its current location.
        // Treat this as an internal relocation: Check if there is enough space between
        // the current header size and the free_space_ptr to write the larger tuple.
        // If it fits, shift free_space_ptr downward (free_space_ptr - new_size), copy the
        // new data there, update the slot's offset_ and size_ values, update the master
        // FREE_SPACE_OFFSET header on the page, and return true.
        // If it does not fit, return false (the engine will have to turn this into a Delete + Append).
        uint32_t free_space_ptr = GetFreeSpacePointer();
        uint32_t slot_count = GetSlotCount();
        if (free_space_ptr >= new_size && HEADER_SIZE + (sizeof(uint32_t) + sizeof(uint32_t)) * (slot_count + 1) <= free_space_ptr - new_size)
        {
            old_slot.size_ = new_size;
            free_space_ptr -= new_size;
            old_slot.offset_ = free_space_ptr;
            std::memcpy(data_buffer_.data() + free_space_ptr, new_tuple_data, new_size);
            std::memcpy(data_buffer_.data() + offset, reinterpret_cast<const char *>(&old_slot), sizeof(old_slot));
            std::memcpy(data_buffer_.data() + FREE_SPACE_OFFSET, &free_space_ptr, sizeof(uint32_t));
            is_defragmented = false;
            return true;
        }

        return false;
    }

    void Defragment()
    {
        if (is_defragmented)
            return;

        std::array<uint8_t, 4096> tmp_buffer = data_buffer_;
        uint32_t total_slots = GetSlotCount();
        uint32_t new_free_space_ptr = 4096;

        // Loop through every slot, maintaining its original position index
        for (uint32_t i = 0; i < total_slots; i++)
        {
            uint32_t slot_header_offset = HEADER_SIZE + i * sizeof(Slot);

            Slot slot;
            std::memcpy(&slot, data_buffer_.data() + slot_header_offset, sizeof(slot));

            if (slot.size_ == 0)
            {
                // Pinned Tombstone: Keep the slot empty but preserve its index position
                Slot dead_slot(0, 0);
                std::memcpy(tmp_buffer.data() + slot_header_offset, &dead_slot, sizeof(Slot));
            }
            else
            {
                // Relocate data payload safely
                uint32_t new_offset = new_free_space_ptr - slot.size_;
                Slot updated_slot(new_offset, slot.size_);

                // Write updated metadata back to its EXACT original index location
                std::memcpy(tmp_buffer.data() + slot_header_offset, &updated_slot, sizeof(Slot));

                // Copy data payload
                std::memcpy(tmp_buffer.data() + new_offset, data_buffer_.data() + slot.offset_, slot.size_);
                new_free_space_ptr -= slot.size_;
            }
        }

        // Update the free space pointer. Total slot count remains unchanged!
        std::memcpy(tmp_buffer.data() + FREE_SPACE_OFFSET, &new_free_space_ptr, sizeof(uint32_t));
        data_buffer_ = tmp_buffer;
        is_defragmented = true;
    }

    const uint32_t maxPageSpace() const {
        return 4096 - HEADER_SIZE;
    }
};

#endif
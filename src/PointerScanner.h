#pragma once

#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "MemoryReader.h"

// ============================================================================
// PointerScanner: Pointer detection and recursive reading
// Purpose: Detect pointers in structures and read them recursively
// ============================================================================

struct PointerInfo
{
    uintptr_t offset;             // Offset in the original structure
    uintptr_t pointerValue;       // Value stored at this offset
    int depth;                    // How many levels deep this pointer is
    std::vector<uintptr_t> chain; // Chain of pointers from root: [ptr1, ptr2, ...]
    std::vector<uint8_t> data;    // Data read from dereferenced pointer
    bool isValid;                 // Whether this pointer could be dereferenced
    std::wstring errorMessage;    // Error description if not valid
};

struct StructurePointers
{
    std::vector<PointerInfo> pointers;
    std::map<uintptr_t, std::vector<uint8_t>> pointerData; // Maps pointer address to its data
};

class PointerScanner
{
public:
    explicit PointerScanner(MemoryReader *memoryReader);

    // Scan structure for pointers and read them recursively
    StructurePointers ScanStructure(
        uintptr_t baseAddress,
        const std::vector<uint8_t> &buffer,
        int maxDepth);

    // Read data from a dereferenced pointer
    bool ReadPointerData(uintptr_t pointerAddr, std::vector<uint8_t> &outData, size_t readSize = 256);

    // Check if value looks like a valid pointer
    static bool LooksLikePointer(uintptr_t value);

    // Recursively follow and read pointer chain
    bool FollowPointerChain(
        uintptr_t startPointer,
        int currentDepth,
        int maxDepth,
        std::vector<uintptr_t> &chain,
        std::vector<uint8_t> &outData);

private:
    MemoryReader *m_memoryReader;

    // Constants for pointer validation
    static constexpr uintptr_t MIN_VALID_ADDRESS = 0x10000;
    static constexpr uintptr_t MAX_VALID_ADDRESS = 0x7FFFFFFF0000;
    static constexpr size_t POINTER_READ_SIZE = 256; // How many bytes to read from dereferenced pointer
};

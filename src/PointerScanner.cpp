#include "PointerScanner.h"
#include "DebugLog.h"
#include <iostream>

PointerScanner::PointerScanner(MemoryReader *memoryReader)
    : m_memoryReader(memoryReader)
{
    DBG_INFO(L"PointerScanner initialized");
}

bool PointerScanner::LooksLikePointer(uintptr_t value)
{
    // Check if value is within valid user-mode address range
    return value >= MIN_VALID_ADDRESS && value <= MAX_VALID_ADDRESS;
}

StructurePointers PointerScanner::ScanStructure(
    uintptr_t baseAddress,
    const std::vector<uint8_t> &buffer,
    int maxDepth)
{
    StructurePointers result;

    if (maxDepth <= 0)
    {
        DBG_WARN(L"MaxDepth is 0 or negative");
        return result;
    }

    // Scan buffer for potential pointers (8-byte aligned, x64)
    for (size_t offset = 0; offset + 8 <= buffer.size(); offset += 8)
    {
        uintptr_t pointerValue = *reinterpret_cast<const uintptr_t *>(&buffer[offset]);

        if (LooksLikePointer(pointerValue))
        {
            PointerInfo info;
            info.offset = offset;
            info.pointerValue = pointerValue;
            info.depth = 1;
            info.isValid = false;
            info.chain.push_back(pointerValue);

            DBG_INFO(L"Found potential pointer at offset 0x" + std::to_wstring(offset) +
                     L": 0x" + std::to_wstring(pointerValue));

            // Try to read data from this pointer
            if (FollowPointerChain(pointerValue, 1, maxDepth, info.chain, info.data))
            {
                info.isValid = true;
                DBG_INFO(L"Successfully read from pointer at offset 0x" + std::to_wstring(offset));
            }
            else
            {
                info.errorMessage = L"Failed to dereference pointer";
            }

            result.pointers.push_back(info);
        }
    }

    DBG_INFO(L"Structure scan complete. Found " + std::to_wstring(result.pointers.size()) + L" pointers");

    return result;
}

bool PointerScanner::ReadPointerData(uintptr_t pointerAddr, std::vector<uint8_t> &outData, size_t readSize)
{
    if (!LooksLikePointer(pointerAddr))
    {
        DBG_ERR(L"Invalid pointer address: 0x" + std::to_wstring(pointerAddr));
        return false;
    }

    outData.resize(readSize);
    bool success = m_memoryReader->ReadMemory(pointerAddr, outData.data(), readSize);

    if (success)
    {
        DBG_INFO(L"Read " + std::to_wstring(readSize) + L" bytes from 0x" + std::to_wstring(pointerAddr));
    }
    else
    {
        DBG_ERR(L"Failed to read from pointer 0x" + std::to_wstring(pointerAddr));
    }

    return success;
}

bool PointerScanner::FollowPointerChain(
    uintptr_t startPointer,
    int currentDepth,
    int maxDepth,
    std::vector<uintptr_t> &chain,
    std::vector<uint8_t> &outData)
{
    if (currentDepth >= maxDepth)
    {
        // Read final data without following further pointers
        return ReadPointerData(startPointer, outData, POINTER_READ_SIZE);
    }

    // Read data from current pointer
    std::vector<uint8_t> buffer(POINTER_READ_SIZE);
    if (!m_memoryReader->ReadMemory(startPointer, buffer.data(), POINTER_READ_SIZE))
    {
        DBG_ERR(L"Cannot read from pointer at depth " + std::to_wstring(currentDepth));
        return false;
    }

    // Try to find pointer in read data and follow it
    if (buffer.size() >= 8)
    {
        uintptr_t nextPointer = *reinterpret_cast<uintptr_t *>(&buffer[0]);

        if (LooksLikePointer(nextPointer))
        {
            DBG_INFO(L"Found next pointer in chain at depth " + std::to_wstring(currentDepth) +
                     L": 0x" + std::to_wstring(nextPointer));

            chain.push_back(nextPointer);

            // Recursively follow the next pointer
            if (FollowPointerChain(nextPointer, currentDepth + 1, maxDepth, chain, outData))
            {
                return true;
            }
            else
            {
                // Still store the data we read, even if next pointer doesn't lead anywhere
                outData = buffer;
                return true;
            }
        }
    }

    // No valid pointer found, return current data
    outData = buffer;
    return true;
}

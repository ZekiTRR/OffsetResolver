#include "PointerScanner.h"
#include "DebugLog.h"
#include <cstring>
#include <iostream>

namespace
{
size_t GetReadableSize(MemoryReader *memoryReader, uintptr_t address, size_t requestedSize)
{
    if (memoryReader == nullptr)
    {
        return 0;
    }

    size_t candidateSize = requestedSize;
    while (candidateSize >= sizeof(uintptr_t))
    {
        if (memoryReader->IsValidAddress(address, candidateSize))
        {
            return candidateSize;
        }

        candidateSize /= 2;
    }

    return memoryReader->IsValidAddress(address, sizeof(uintptr_t)) ? sizeof(uintptr_t) : 0;
}
}

PointerScanner::PointerScanner(MemoryReader *memoryReader)
    : m_memoryReader(memoryReader)
{
    DBG_INFO(L"PointerScanner initialized");
}

bool PointerScanner::LooksLikePointer(uintptr_t value)
{
    // Check if value is within valid user-mode address range.
    return value >= MIN_VALID_ADDRESS && value <= MAX_VALID_ADDRESS;
}

StructurePointers PointerScanner::ScanStructure(
    uintptr_t baseAddress,
    const std::vector<uint8_t> &buffer,
    int maxDepth)
{
    StructurePointers result;
    (void)baseAddress;

    if (maxDepth <= 0)
    {
        DBG_WARN(L"MaxDepth is 0 or negative");
        return result;
    }

    // Scan buffer for potential pointers (8-byte aligned, x64)
    for (size_t offset = 0; offset + sizeof(uintptr_t) <= buffer.size(); offset += sizeof(uintptr_t))
    {
        uintptr_t pointerValue = 0;
        std::memcpy(&pointerValue, &buffer[offset], sizeof(pointerValue));

        if (LooksLikePointer(pointerValue) &&
            m_memoryReader != nullptr &&
            m_memoryReader->IsValidAddress(pointerValue, sizeof(uintptr_t)))
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
    if (!LooksLikePointer(pointerAddr) || m_memoryReader == nullptr)
    {
        DBG_ERR(L"Invalid pointer address: 0x" + std::to_wstring(pointerAddr));
        return false;
    }

    const size_t readableSize = GetReadableSize(m_memoryReader, pointerAddr, readSize);
    if (readableSize < sizeof(uintptr_t))
    {
        DBG_ERR(L"Pointer address is not readable: 0x" + std::to_wstring(pointerAddr));
        return false;
    }

    outData.resize(readableSize);
    bool success = m_memoryReader->ReadMemory(pointerAddr, outData.data(), readableSize);

    if (success)
    {
        DBG_INFO(L"Read " + std::to_wstring(readableSize) + L" bytes from 0x" + std::to_wstring(pointerAddr));
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
        // Read final data without following further pointers.
        return ReadPointerData(startPointer, outData, POINTER_READ_SIZE);
    }

    const size_t readableSize = GetReadableSize(m_memoryReader, startPointer, POINTER_READ_SIZE);
    if (readableSize < sizeof(uintptr_t))
    {
        DBG_ERR(L"Cannot read from pointer at depth " + std::to_wstring(currentDepth));
        return false;
    }

    // Read data from current pointer.
    std::vector<uint8_t> buffer(readableSize);
    if (!m_memoryReader->ReadMemory(startPointer, buffer.data(), readableSize))
    {
        DBG_ERR(L"Cannot read from pointer at depth " + std::to_wstring(currentDepth));
        return false;
    }

    // Try to find pointer in read data and follow it.
    if (buffer.size() >= sizeof(uintptr_t))
    {
        uintptr_t nextPointer = 0;
        std::memcpy(&nextPointer, buffer.data(), sizeof(nextPointer));

        if (LooksLikePointer(nextPointer) &&
            m_memoryReader != nullptr &&
            m_memoryReader->IsValidAddress(nextPointer, sizeof(uintptr_t)))
        {
            DBG_INFO(L"Found next pointer in chain at depth " + std::to_wstring(currentDepth) +
                     L": 0x" + std::to_wstring(nextPointer));

            chain.push_back(nextPointer);

            // Recursively follow the next pointer.
            if (FollowPointerChain(nextPointer, currentDepth + 1, maxDepth, chain, outData))
            {
                return true;
            }

            // Still store the data we read, even if the next pointer does not lead anywhere.
            outData = buffer;
            return true;
        }
    }

    // No valid nested pointer found, return current data.
    outData = buffer;
    return true;
}
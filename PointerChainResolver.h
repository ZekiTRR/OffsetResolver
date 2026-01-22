#pragma once

#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>
#include "ModuleRegistry.h"
#include "MemoryReader.h"

// Single pointer chain configuration
struct PointerChain
{
    std::wstring moduleName;        // "app.dll"
    uintptr_t baseOffset;           // 0x649910
    std::vector<uintptr_t> offsets; // [0x18, 0x70, 0x370, 0x2D0]
    ValueType valueType;            // INT / FLOAT / DOUBLE
    std::wstring description;       // "Player HP"

    // Runtime (not saved to file)
    uintptr_t resolvedAddress; // Final calculated address
    MemoryValue currentValue;  // Last read value
    bool isResolved;           // Successfully resolved
    std::wstring lastError;    // Error message if resolution failed

    PointerChain()
        : baseOffset(0), valueType(ValueType::INT),
          resolvedAddress(0), isResolved(false)
    {
    }
};

// Resolves pointer chains step-by-step with validation
class PointerChainResolver
{
public:
    PointerChainResolver(
        const ModuleRegistry *moduleRegistry,
        MemoryReader *memoryReader);

    // Resolve single chain and read value
    bool ResolveChain(PointerChain &chain);

    // Resolve all chains in collection
    int ResolveAllChains(std::vector<PointerChain> &chains);

    // Get detailed resolution info for display
    std::wstring GetResolutionInfo(const PointerChain &chain) const;

private:
    const ModuleRegistry *m_moduleRegistry;
    MemoryReader *m_memoryReader;

    // Step-by-step pointer following with validation
    bool ResolveStep(uintptr_t &currentPtr, uintptr_t offset);

    // Read final value based on type
    bool ReadFinalValue(uintptr_t address, PointerChain &chain);

    // Format hex address with leading zeros
    static std::wstring FormatHex(uintptr_t value);
};

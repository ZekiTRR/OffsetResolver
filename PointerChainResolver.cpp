#include "PointerChainResolver.h"
#include <iostream>
#include <iomanip>
#include <sstream>

PointerChainResolver::PointerChainResolver(
    const ModuleRegistry *moduleRegistry,
    MemoryReader *memoryReader)
    : m_moduleRegistry(moduleRegistry), m_memoryReader(memoryReader)
{
}

bool PointerChainResolver::ResolveChain(PointerChain &chain)
{
    // Step 1: Get module base address
    ModuleInfo moduleInfo;
    if (!m_moduleRegistry->FindModule(chain.moduleName, moduleInfo))
    {
        chain.lastError = L"Module not found: " + chain.moduleName;
        chain.isResolved = false;
        return false;
    }

    // Step 2: Calculate base pointer = moduleBase + baseOffset
    uintptr_t currentPtr = moduleInfo.baseAddress + chain.baseOffset;

    // Validate initial pointer
    if (!m_memoryReader->IsValidAddress(currentPtr))
    {
        chain.lastError = L"Invalid base address after module offset";
        chain.isResolved = false;
        return false;
    }

    // Step 3: Walk the pointer chain
    for (size_t i = 0; i < chain.offsets.size(); ++i)
    {
        if (!ResolveStep(currentPtr, chain.offsets[i]))
        {
            chain.lastError = L"Failed at chain step " + std::to_wstring(i + 1) + L"/" + std::to_wstring(chain.offsets.size());
            chain.isResolved = false;
            return false;
        }
    }

    // Step 4: Read final value from resolved address
    chain.resolvedAddress = currentPtr;
    if (!ReadFinalValue(currentPtr, chain))
    {
        chain.lastError = L"Failed to read value at final address";
        chain.isResolved = false;
        return false;
    }

    chain.isResolved = true;
    chain.lastError = L"";
    return true;
}

bool PointerChainResolver::ResolveStep(uintptr_t &currentPtr, uintptr_t offset)
{
    // Add offset to current pointer
    uintptr_t address = currentPtr + offset;

    // Validate address before reading
    if (!m_memoryReader->IsValidAddress(address))
    {
        return false;
    }

    // Read pointer value at this address
    bool success = false;
    uintptr_t nextPtr = m_memoryReader->ReadPointer(address, success);

    if (!success)
    {
        return false;
    }

    // Validate next pointer
    if (!m_memoryReader->IsValidAddress(nextPtr))
    {
        return false;
    }

    currentPtr = nextPtr;
    return true;
}

bool PointerChainResolver::ReadFinalValue(uintptr_t address, PointerChain &chain)
{
    bool success = false;

    switch (chain.valueType)
    {
    case ValueType::INT:
    {
        int32_t value = m_memoryReader->ReadInt(address, success);
        if (success)
        {
            chain.currentValue.type = ValueType::INT;
            chain.currentValue.data.intValue = value;
            chain.currentValue.isValid = true;
        }
        break;
    }
    case ValueType::FLOAT:
    {
        float value = m_memoryReader->ReadFloat(address, success);
        if (success)
        {
            chain.currentValue.type = ValueType::FLOAT;
            chain.currentValue.data.floatValue = value;
            chain.currentValue.isValid = true;
        }
        break;
    }
    case ValueType::DOUBLE:
    {
        double value = m_memoryReader->ReadDouble(address, success);
        if (success)
        {
            chain.currentValue.type = ValueType::DOUBLE;
            chain.currentValue.data.doubleValue = value;
            chain.currentValue.isValid = true;
        }
        break;
    }
    }

    return success;
}

int PointerChainResolver::ResolveAllChains(std::vector<PointerChain> &chains)
{
    int successCount = 0;
    for (auto &chain : chains)
    {
        if (ResolveChain(chain))
        {
            successCount++;
        }
    }
    return successCount;
}

std::wstring PointerChainResolver::GetResolutionInfo(const PointerChain &chain) const
{
    std::wstringstream ss;

    ss << L"  " << chain.moduleName << L" + " << FormatHex(chain.baseOffset) << L"\n";

    for (const auto &offset : chain.offsets)
    {
        ss << L"  -> +" << FormatHex(offset) << L"\n";
    }

    if (chain.isResolved)
    {
        ss << L"  = " << FormatHex(chain.resolvedAddress) << L"\n";
        ss << L"  Value: " << chain.currentValue.ToString();
    }
    else
    {
        ss << L"  [NOT RESOLVED] " << chain.lastError;
    }

    return ss.str();
}

std::wstring PointerChainResolver::FormatHex(uintptr_t value)
{
    wchar_t buffer[32];
    swprintf_s(buffer, L"0x%llX", value);
    return buffer;
}

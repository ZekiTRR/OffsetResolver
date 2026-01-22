#include "PointerChainResolver.h"
#include "DebugLog.h"
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
    DBG_STEP(L"=== Resolving chain: " + chain.description + L" ===");

    // Step 1: Get module base address
    ModuleInfo moduleInfo;
    if (!m_moduleRegistry->FindModule(chain.moduleName, moduleInfo))
    {
        chain.lastError = L"Module not found: " + chain.moduleName;
        chain.isResolved = false;
        DBG_ERR(L"Module not found: " + chain.moduleName);
        return false;
    }

    DBG_MODULE(moduleInfo.name, moduleInfo.baseAddress, moduleInfo.size);

    // Step 2: Calculate base pointer address = moduleBase + baseOffset
    uintptr_t baseAddress = moduleInfo.baseAddress + chain.baseOffset;
    DBG_ADDR(L"Base address (module + offset)", baseAddress);

    // Validate base address
    if (!m_memoryReader->IsValidAddress(baseAddress))
    {
        chain.lastError = L"Invalid base address after module offset";
        chain.isResolved = false;
        DBG_ERR(L"Invalid base address: 0x" + FormatHex(baseAddress).substr(2));
        return false;
    }

    // Step 3: Read first pointer from base address
    bool success = false;
    uintptr_t currentPtr = m_memoryReader->ReadPointer(baseAddress, success);
    DBG_MEM(baseAddress, sizeof(uintptr_t), success);

    if (!success)
    {
        chain.lastError = L"Failed to read pointer at base address";
        chain.isResolved = false;
        DBG_ERR(L"Failed to read pointer at base address");
        return false;
    }

    DBG_PTR(baseAddress, currentPtr);

    // Step 4: Walk the pointer chain (apply offsets and read pointers)
    // For chains with offsets: read pointer, add offset, repeat
    // Last offset leads to final value address
    for (size_t i = 0; i < chain.offsets.size(); ++i)
    {
        // Add offset to current pointer
        uintptr_t nextAddress = currentPtr + chain.offsets[i];
        DBG_CHAIN(i + 1, chain.offsets.size(), currentPtr, chain.offsets[i], nextAddress);

        if (!m_memoryReader->IsValidAddress(nextAddress))
        {
            chain.lastError = L"Invalid address at chain step " + std::to_wstring(i + 1) + L"/" + std::to_wstring(chain.offsets.size());
            chain.isResolved = false;
            DBG_ERR(L"Invalid address at step " + std::to_wstring(i + 1) + L": 0x" + FormatHex(nextAddress).substr(2));
            return false;
        }

        // If this is the last offset, don't dereference - this is the final address
        if (i == chain.offsets.size() - 1)
        {
            currentPtr = nextAddress;
            DBG_OK(L"Final address reached: 0x" + FormatHex(currentPtr).substr(2));
            break;
        }

        // Read next pointer in the chain
        currentPtr = m_memoryReader->ReadPointer(nextAddress, success);
        DBG_MEM(nextAddress, sizeof(uintptr_t), success);

        if (!success)
        {
            chain.lastError = L"Failed to read pointer at chain step " + std::to_wstring(i + 1) + L"/" + std::to_wstring(chain.offsets.size());
            chain.isResolved = false;
            DBG_ERR(L"Failed to read pointer at step " + std::to_wstring(i + 1));
            return false;
        }

        DBG_PTR(nextAddress, currentPtr);

        if (!m_memoryReader->IsValidAddress(currentPtr))
        {
            chain.lastError = L"Invalid pointer value at chain step " + std::to_wstring(i + 1) + L"/" + std::to_wstring(chain.offsets.size());
            chain.isResolved = false;
            DBG_ERR(L"Invalid pointer value at step " + std::to_wstring(i + 1) + L": 0x" + FormatHex(currentPtr).substr(2));
            return false;
        }
    }

    // Step 5: Read final value from resolved address
    chain.resolvedAddress = currentPtr;
    DBG_ADDR(L"Final resolved address", currentPtr);

    if (!ReadFinalValue(currentPtr, chain))
    {
        chain.lastError = L"Failed to read value at final address";
        chain.isResolved = false;
        DBG_ERR(L"Failed to read final value");
        return false;
    }

    chain.isResolved = true;
    chain.lastError = L"";
    DBG_OK(L"Chain resolved successfully: " + chain.currentValue.ToString());
    return true;
}

bool PointerChainResolver::ResolveStep(uintptr_t &currentPtr, uintptr_t offset)
{
    // Add offset to current pointer
    uintptr_t address = currentPtr + offset;

    // Validate address before reading
    if (!m_memoryReader->IsValidAddress(address))
    {
        std::wcerr << L"[PointerChainResolver] Invalid address: 0x" << std::hex << address
                   << L" (ptr=0x" << currentPtr << L" + offset=0x" << offset << L")" << std::dec << std::endl;
        return false;
    }

    // Read pointer value at this address
    bool success = false;
    uintptr_t nextPtr = m_memoryReader->ReadPointer(address, success);

    if (!success)
    {
        std::wcerr << L"[PointerChainResolver] Failed to read pointer at 0x" << std::hex << address << std::dec << std::endl;
        return false;
    }

    // Validate next pointer
    if (!m_memoryReader->IsValidAddress(nextPtr))
    {
        std::wcerr << L"[PointerChainResolver] Read invalid pointer value: 0x" << std::hex << nextPtr
                   << L" from address 0x" << address << std::dec << std::endl;
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

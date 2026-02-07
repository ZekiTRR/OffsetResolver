#include "AddressResolver.h"
#include <iostream>

AddressResolver::AddressResolver()
    : m_moduleRegistry(nullptr)
{
}

void AddressResolver::SetModuleRegistry(const ModuleRegistry *registry)
{
    m_moduleRegistry = registry;
}

bool AddressResolver::ResolveOffset(OffsetEntry &entry)
{
    if (!m_moduleRegistry || !m_moduleRegistry->IsLoaded())
    {
        std::wcerr << L"[-] Module registry is not loaded." << std::endl;
        return false;
    }

    uintptr_t moduleBase = m_moduleRegistry->GetModuleBase(entry.moduleName);

    if (moduleBase == 0)
    {
        std::wcerr << L"[-] Module '" << entry.moduleName << L"' not found." << std::endl;
        entry.isResolved = false;
        entry.resolvedAddress = 0;
        return false;
    }

    entry.resolvedAddress = moduleBase + entry.offset;
    entry.isResolved = true;

    return true;
}

int AddressResolver::ResolveAll(OffsetStorage &storage)
{
    if (!m_moduleRegistry || !m_moduleRegistry->IsLoaded())
    {
        std::wcerr << L"[-] Module registry is not loaded." << std::endl;
        return 0;
    }

    int resolvedCount = 0;
    auto &offsets = storage.GetOffsets();

    for (auto &entry : offsets)
    {
        if (ResolveOffset(entry))
        {
            resolvedCount++;
        }
    }

    if (resolvedCount > 0)
    {
        std::wcout << L"[+] Successfully resolved " << resolvedCount << L"/"
                   << offsets.size() << L" offsets." << std::endl;
    }
    else
    {
        std::wcerr << L"[-] Failed to resolve any offsets." << std::endl;
    }

    return resolvedCount;
}

uintptr_t AddressResolver::CalculateAddress(const std::wstring &moduleName, uintptr_t offset)
{
    if (!m_moduleRegistry || !m_moduleRegistry->IsLoaded())
    {
        return 0;
    }

    uintptr_t moduleBase = m_moduleRegistry->GetModuleBase(moduleName);
    if (moduleBase == 0)
    {
        return 0;
    }

    return moduleBase + offset;
}

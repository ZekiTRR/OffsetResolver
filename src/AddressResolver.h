#pragma once
#include "ModuleRegistry.h"
#include "OffsetStorage.h"

// ============================================================================
// AddressResolver: Address resolution
// Purpose: Recalculate absolute addresses from "module + offset" pairs
// Automatically handles ASLR on each process launch
// ============================================================================

class AddressResolver
{
private:
    const ModuleRegistry *m_moduleRegistry;

public:
    AddressResolver();

    // Set module registry for address resolution
    void SetModuleRegistry(const ModuleRegistry *registry);

    // Resolve single offset
    bool ResolveOffset(OffsetEntry &entry);

    // Resolve all offsets in storage
    int ResolveAll(OffsetStorage &storage);

    // Calculate absolute address manually
    uintptr_t CalculateAddress(const std::wstring &moduleName, uintptr_t offset);
};

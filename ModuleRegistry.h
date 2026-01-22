#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <map>

// ============================================================================
// ModuleRegistry: Process module registry
// Purpose: Retrieve and store loaded module information
// ImageBase, SizeOfImage for each module
// ============================================================================

struct ModuleInfo
{
    std::wstring name;     // Module name (e.g., "app.dll")
    uintptr_t baseAddress; // ImageBase (base address)
    uintptr_t size;        // SizeOfImage (module size)
};

class ModuleRegistry
{
private:
    std::vector<ModuleInfo> m_modules;
    std::map<std::wstring, ModuleInfo> m_moduleMap; // Fast lookup by name
    DWORD m_pid;
    bool m_isLoaded;

public:
    ModuleRegistry();

    // Load module list for specified process
    bool LoadModules(DWORD pid);

    // Get all modules list
    const std::vector<ModuleInfo> &GetModules() const { return m_modules; }

    // Find module by name (case-insensitive)
    bool FindModule(const std::wstring &moduleName, ModuleInfo &outInfo) const;

    // Get module base address
    uintptr_t GetModuleBase(const std::wstring &moduleName) const;

    // Check if modules are loaded
    bool IsLoaded() const { return m_isLoaded; }

    // Clear registry
    void Clear();

    // Print module list to console
    void PrintModules() const;
};

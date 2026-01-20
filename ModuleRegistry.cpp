#include "ModuleRegistry.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

ModuleRegistry::ModuleRegistry()
    : m_pid(0), m_isLoaded(false)
{
}

bool ModuleRegistry::LoadModules(DWORD pid)
{
    Clear();
    m_pid = pid;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"[-] Failed to create module snapshot. Error code: " << GetLastError() << std::endl;
        std::wcerr << L"[-] Try running as administrator." << std::endl;
        return false;
    }

    MODULEENTRY32W me32;
    me32.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(hSnapshot, &me32))
    {
        do
        {
            ModuleInfo info;
            info.name = me32.szModule;
            info.baseAddress = reinterpret_cast<uintptr_t>(me32.modBaseAddr);
            info.size = me32.modBaseSize;

            m_modules.push_back(info);

            // Add to map for fast lookup (convert name to lowercase)
            std::wstring lowerName = info.name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::towlower);
            m_moduleMap[lowerName] = info;

        } while (Module32NextW(hSnapshot, &me32));
    }

    CloseHandle(hSnapshot);
    m_isLoaded = !m_modules.empty();

    if (m_isLoaded)
    {
        std::wcout << L"[+] Loaded " << m_modules.size() << L" modules." << std::endl;
    }
    else
    {
        std::wcerr << L"[-] No modules found." << std::endl;
    }

    return m_isLoaded;
}

bool ModuleRegistry::FindModule(const std::wstring &moduleName, ModuleInfo &outInfo) const
{
    if (!m_isLoaded)
        return false;

    // Case-insensitive search
    std::wstring lowerName = moduleName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::towlower);

    auto it = m_moduleMap.find(lowerName);
    if (it != m_moduleMap.end())
    {
        outInfo = it->second;
        return true;
    }

    return false;
}

uintptr_t ModuleRegistry::GetModuleBase(const std::wstring &moduleName) const
{
    ModuleInfo info;
    if (FindModule(moduleName, info))
    {
        return info.baseAddress;
    }
    return 0;
}

void ModuleRegistry::Clear()
{
    m_modules.clear();
    m_moduleMap.clear();
    m_isLoaded = false;
}

void ModuleRegistry::PrintModules() const
{
    if (!m_isLoaded || m_modules.empty())
    {
        std::wcout << L"[-] No modules loaded." << std::endl;
        return;
    }

    std::wcout << L"\n=== Module List ===\n";
    std::wcout << std::left << std::setw(35) << L"Module Name"
               << L" | " << std::setw(16) << L"Base Address"
               << L" | " << L"Size" << std::endl;
    std::wcout << std::wstring(75, L'-') << std::endl;

    for (const auto &mod : m_modules)
    {
        std::wcout << std::left << std::setw(35) << mod.name
                   << L" | 0x" << std::hex << std::uppercase << std::setw(14) << mod.baseAddress
                   << L" | 0x" << std::setw(8) << mod.size << std::dec << std::endl;
    }
    std::wcout << std::endl;
}

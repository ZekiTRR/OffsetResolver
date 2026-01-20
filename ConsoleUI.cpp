#include "ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

// Helper to convert string to ValueType
static ValueType StringToValueType(const std::string &str)
{
    if (str == "int")
        return ValueType::INT;
    if (str == "float")
        return ValueType::FLOAT;
    if (str == "double")
        return ValueType::DOUBLE;
    return ValueType::INT; // Default
}

ConsoleUI::ConsoleUI(ProcessManager &pm, ModuleRegistry &mr, AddressResolver &ar, OffsetStorage &os,
                     MemoryReader &mr2, PointerChainResolver &pcr, PointerChainStorage &pcs)
    : m_processManager(pm), m_moduleRegistry(mr), m_addressResolver(ar), m_offsetStorage(os),
      m_memoryReader(mr2), m_pointerChainResolver(pcr), m_pointerChainStorage(pcs)
{
    // Set locale for proper character display
    setlocale(LC_ALL, "");
}

void ConsoleUI::ShowMainMenu()
{
    while (true)
    {
        ClearScreen();
        std::wcout << L"====================================================\n";
        std::wcout << L"    Process Module & Offset Management Tool        \n";
        std::wcout << L"====================================================\n\n";

        std::wcout << L"Choose mode:\n";
        std::wcout << L"  1. Offset Manager (ASLR-safe offset storage)\n";
        std::wcout << L"  2. Pointer Chain Manager (multi-level pointers)\n";
        std::wcout << L"  3. Module Dumper (Export module list to file)\n";
        std::wcout << L"  0. Exit\n\n";

        int choice = GetChoice(L"Select option", 0, 3);

        switch (choice)
        {
        case 1:
            ShowOffsetManagerMenu();
            break;
        case 2:
            ShowPointerChainManagerMenu();
            break;
        case 3:
            ShowModuleDumperMenu();
            break;
        case 0:
            return;
        }
    }
}

void ConsoleUI::ShowOffsetManagerMenu()
{
    while (true)
    {
        ClearScreen();
        std::wcout << L"====================================================\n";
        std::wcout << L"              Offset Manager Mode                    \n";
        std::wcout << L"====================================================\n\n";

        // Show status
        if (m_processManager.IsAttached())
        {
            std::wcout << L"[+] Process: " << m_processManager.GetProcessName()
                       << L" (PID: " << m_processManager.GetPID() << L")\n";
        }
        else
        {
            std::wcout << L"[ ] Process: Not attached\n";
        }

        if (m_moduleRegistry.IsLoaded())
        {
            std::wcout << L"[+] Modules: " << m_moduleRegistry.GetModules().size() << L" loaded\n";
        }
        else
        {
            std::wcout << L"[ ] Modules: Not loaded\n";
        }

        std::wcout << L"[+] Offsets: " << m_offsetStorage.Count() << L" in storage";
        if (m_offsetStorage.IsModified())
        {
            std::wcout << L" (modified)";
        }
        std::wcout << L"\n\n";

        std::wcout << L"Options:\n";
        std::wcout << L"  1. Attach to process\n";
        std::wcout << L"  2. Load offsets from file\n";
        std::wcout << L"  3. Add new offset\n";
        std::wcout << L"  4. Resolve all offsets (calculate addresses)\n";
        std::wcout << L"  5. View offsets and resolved addresses\n";
        std::wcout << L"  6. Save offsets to file\n";
        std::wcout << L"  7. View module list\n";
        std::wcout << L"  0. Back to main menu\n\n";

        int choice = GetChoice(L"Select option", 0, 7);

        switch (choice)
        {
        case 1:
            AttachToProcessFlow();
            break;
        case 2:
            LoadOffsetsFlow();
            break;
        case 3:
            AddOffsetFlow();
            break;
        case 4:
            ResolveOffsetsFlow();
            break;
        case 5:
            ViewOffsetsFlow();
            break;
        case 6:
            SaveOffsetsFlow();
            break;
        case 7:
            m_moduleRegistry.PrintModules();
            Pause();
            break;
        case 0:
            return;
        }
    }
}

void ConsoleUI::ShowPointerChainManagerMenu()
{
    while (true)
    {
        ClearScreen();
        std::wcout << L"====================================================\n";
        std::wcout << L"            Pointer Chain Manager Mode              \n";
        std::wcout << L"====================================================\n\n";

        // Show status
        if (m_processManager.IsAttached())
        {
            std::wcout << L"[+] Process: " << m_processManager.GetProcessName()
                       << L" (PID: " << m_processManager.GetPID() << L")\n";
        }
        else
        {
            std::wcout << L"[ ] Process: Not attached\n";
        }

        if (m_moduleRegistry.IsLoaded())
        {
            std::wcout << L"[+] Modules: " << m_moduleRegistry.GetModules().size() << L" loaded\n";
        }
        else
        {
            std::wcout << L"[ ] Modules: Not loaded\n";
        }

        std::wcout << L"[+] Pointer Chains: " << m_pointerChainStorage.GetChainCount()
                   << L" stored";
        if (m_pointerChainStorage.IsModified())
        {
            std::wcout << L" (modified)";
        }
        std::wcout << L"\n\n";

        std::wcout << L"Options:\n";
        std::wcout << L"  1. Attach to process\n";
        std::wcout << L"  2. Load chains from file\n";
        std::wcout << L"  3. Add new pointer chain\n";
        std::wcout << L"  4. Resolve all chains\n";
        std::wcout << L"  5. View resolved chain values\n";
        std::wcout << L"  6. Save chains to file\n";
        std::wcout << L"  7. Print all chains\n";
        std::wcout << L"  0. Back to main menu\n\n";

        int choice = GetChoice(L"Select option", 0, 7);

        switch (choice)
        {
        case 1:
            AttachToProcessFlow();
            break;
        case 2:
            LoadChainsFromFileFlow();
            break;
        case 3:
            AddPointerChainFlow();
            break;
        case 4:
            ResolveAllChainsFlow();
            break;
        case 5:
            ViewChainValuesFlow();
            break;
        case 6:
            SaveChainsToFileFlow();
            break;
        case 7:
            PrintChainList();
            break;
        case 0:
            return;
        }
    }
}

void ConsoleUI::ShowModuleDumperMenu()
{
    ClearScreen();
    std::wcout << L"====================================================\n";
    std::wcout << L"              Module Dumper Mode                     \n";
    std::wcout << L"====================================================\n\n";

    std::wstring processName = GetInput(L"Enter process name (e.g., example.exe)");

    if (!m_processManager.AttachToProcess(processName))
    {
        Pause();
        return;
    }

    if (!m_moduleRegistry.LoadModules(m_processManager.GetPID()))
    {
        Pause();
        return;
    }

    m_moduleRegistry.PrintModules();

    std::wcout << L"\nSave to file? (y/n): ";
    std::wstring answer;
    std::getline(std::wcin, answer);

    if (answer == L"y" || answer == L"Y")
    {
        DumpModulesToFile();
    }

    Pause();
}

// ============================================================================
// Offset Manager Functions
// ============================================================================

void ConsoleUI::AttachToProcessFlow()
{
    std::wstring processName = GetInput(L"Enter process name (e.g., example.exe)");

    if (m_processManager.AttachToProcess(processName))
    {
        m_moduleRegistry.LoadModules(m_processManager.GetPID());
        m_addressResolver.SetModuleRegistry(&m_moduleRegistry);
    }

    Pause();
}

void ConsoleUI::LoadOffsetsFlow()
{
    std::wstring filename = GetInput(L"Enter config filename (e.g., offsets.cfg)");

    if (m_offsetStorage.LoadFromFile(filename))
    {
        m_currentConfigFile = filename;
    }

    Pause();
}

void ConsoleUI::AddOffsetFlow()
{
    if (!m_moduleRegistry.IsLoaded())
    {
        std::wcout << L"\n[-] Please attach to process first!\n";
        Pause();
        return;
    }

    std::wcout << L"\n=== Add New Offset ===\n\n";

    OffsetEntry entry;
    entry.moduleName = GetInput(L"Module name (e.g., client.dll)");

    // Check if module exists
    ModuleInfo modInfo;
    if (!m_moduleRegistry.FindModule(entry.moduleName, modInfo))
    {
        std::wcout << L"[!] Warning: Module '" << entry.moduleName << L"' not found in process.\n";
        std::wcout << L"Continue anyway? (y/n): ";
        std::wstring answer;
        std::getline(std::wcin, answer);
        if (answer != L"y" && answer != L"Y")
        {
            return;
        }
    }
    else
    {
        std::wcout << L"[+] Module found. Base: 0x" << std::hex << std::uppercase
                   << modInfo.baseAddress << std::dec << L"\n";
    }

    entry.offset = GetHexInput(L"Offset (hex, e.g., 0xDEA964)");
    entry.description = GetInput(L"Description (optional, e.g., LocalPlayer)");

    m_offsetStorage.AddOffset(entry);
    std::wcout << L"\n[+] Offset added successfully!\n";

    Pause();
}

void ConsoleUI::ResolveOffsetsFlow()
{
    if (!m_processManager.IsAttached())
    {
        std::wcout << L"\n[-] Please attach to process first!\n";
        Pause();
        return;
    }

    if (m_offsetStorage.Count() == 0)
    {
        std::wcout << L"\n[-] No offsets in storage. Add offsets or load from file.\n";
        Pause();
        return;
    }

    std::wcout << L"\n=== Resolving Offsets ===\n\n";
    int resolved = m_addressResolver.ResolveAll(m_offsetStorage);

    if (resolved > 0)
    {
        std::wcout << L"\n[+] Resolved " << resolved << L" offsets.\n";
        std::wcout << L"\nShow resolved addresses? (y/n): ";
        std::wstring answer;
        std::getline(std::wcin, answer);
        if (answer == L"y" || answer == L"Y")
        {
            m_offsetStorage.PrintOffsets();
        }
    }

    Pause();
}

void ConsoleUI::ViewOffsetsFlow()
{
    m_offsetStorage.PrintOffsets();
    Pause();
}

void ConsoleUI::SaveOffsetsFlow()
{
    if (m_offsetStorage.Count() == 0)
    {
        std::wcout << L"\n[-] No offsets to save.\n";
        Pause();
        return;
    }

    std::wstring filename;

    if (!m_currentConfigFile.empty())
    {
        std::wcout << L"\nCurrent file: " << m_currentConfigFile << L"\n";
        std::wcout << L"Save to this file? (y/n): ";
        std::wstring answer;
        std::getline(std::wcin, answer);

        if (answer == L"y" || answer == L"Y")
        {
            filename = m_currentConfigFile;
        }
    }

    if (filename.empty())
    {
        filename = GetInput(L"Enter filename to save (e.g., offsets.cfg)");
    }

    m_offsetStorage.SaveToFile(filename);
    m_currentConfigFile = filename;

    Pause();
}

// ============================================================================
// Module Dumper Functions (from original base_address.cpp)
// ============================================================================

void ConsoleUI::DumpModulesToFile()
{
    std::wstring processName = m_processManager.GetProcessName();
    std::string narrowName(processName.begin(), processName.end());
    std::string filename = narrowName + "_modules_dump.txt";

    std::wofstream file(filename);
    if (!file.is_open())
    {
        std::wcout << L"[-] Failed to create file.\n";
        return;
    }

    file << L"Process: " << processName << L" (PID: " << m_processManager.GetPID() << L")\n\n";
    file << std::left << std::setw(35) << L"Module Name"
         << L" | " << std::setw(16) << L"Base Address"
         << L" | " << L"Size\n";
    file << std::wstring(75, L'-') << L"\n";

    for (const auto &mod : m_moduleRegistry.GetModules())
    {
        file << std::left << std::setw(35) << mod.name
             << L" | 0x" << std::hex << std::uppercase << std::setw(14) << mod.baseAddress
             << L" | 0x" << std::setw(8) << mod.size << std::dec << L"\n";
    }

    file.close();
    std::wcout << L"[+] Module list saved to " << std::wstring(filename.begin(), filename.end()) << L"\n";
}

// ============================================================================
// Pointer Chain Manager Flow Functions
// ============================================================================

void ConsoleUI::AddPointerChainFlow()
{
    ClearScreen();
    std::wcout << L"====================================================\n";
    std::wcout << L"              Add Pointer Chain                      \n";
    std::wcout << L"====================================================\n\n";

    if (!m_processManager.IsAttached())
    {
        std::wcout << L"[-] No process attached. Attach first.\n";
        Pause();
        return;
    }

    if (!m_moduleRegistry.IsLoaded())
    {
        std::wcout << L"[-] Modules not loaded. Load modules first.\n";
        Pause();
        return;
    }

    // Get module name
    std::wstring moduleName = GetInput(L"Enter module name (e.g., app.dll)");

    // Check if module exists
    const auto &modules = m_moduleRegistry.GetModules();
    bool found = false;
    for (const auto &mod : modules)
    {
        if (mod.name == moduleName)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::wcout << L"[-] Module not found.\n";
        Pause();
        return;
    }

    // Get base offset
    std::wcout << L"\nEnter base offset (hex format, e.g., 0x1000): ";
    uintptr_t baseOffset = GetHexInput(L"Base offset");

    // Get offsets chain
    std::wcout << L"\nEnter offsets chain (one per line, in hex format).\n";
    std::wcout << L"Enter 'done' when finished.\n";
    std::vector<uintptr_t> offsets;

    while (true)
    {
        std::wcout << L"Offset [" << offsets.size() + 1 << L"]: ";
        std::wstring input;
        std::getline(std::wcin, input);

        if (input == L"done" || input == L"Done")
        {
            break;
        }

        // Remove 0x prefix if present
        if (input.length() >= 2 && (input.substr(0, 2) == L"0x" || input.substr(0, 2) == L"0X"))
        {
            input = input.substr(2);
        }

        try
        {
            uintptr_t offset = std::stoull(input, nullptr, 16);
            offsets.push_back(offset);
        }
        catch (...)
        {
            std::wcout << L"[-] Invalid hex value. Try again.\n";
        }
    }

    if (offsets.empty())
    {
        std::wcout << L"[-] No offsets provided.\n";
        Pause();
        return;
    }

    // Get value type
    std::wcout << L"\nValue type to read at final address:\n";
    std::wcout << L"  1. int (32-bit)\n";
    std::wcout << L"  2. float (32-bit)\n";
    std::wcout << L"  3. double (64-bit)\n";
    int typeChoice = GetChoice(L"Select type", 1, 3);

    std::string valueType;
    switch (typeChoice)
    {
    case 1:
        valueType = "int";
        break;
    case 2:
        valueType = "float";
        break;
    case 3:
        valueType = "double";
        break;
    }

    // Get description
    std::wstring description = GetInput(L"\nEnter description (e.g., Health)");

    // Create and store chain
    PointerChain chain;
    chain.moduleName = moduleName;
    chain.baseOffset = baseOffset;
    chain.offsets = offsets;
    chain.valueType = StringToValueType(valueType);
    chain.description = description;

    m_pointerChainStorage.AddChain(chain);

    std::wcout << L"\n[+] Pointer chain added successfully!\n";
    Pause();
}

void ConsoleUI::ResolveAllChainsFlow()
{
    ClearScreen();
    std::wcout << L"====================================================\n";
    std::wcout << L"              Resolve All Chains                     \n";
    std::wcout << L"====================================================\n\n";

    if (!m_processManager.IsAttached())
    {
        std::wcout << L"[-] No process attached.\n";
        Pause();
        return;
    }

    if (!m_moduleRegistry.IsLoaded())
    {
        std::wcout << L"[-] Modules not loaded.\n";
        Pause();
        return;
    }

    if (m_pointerChainStorage.GetChainCount() == 0)
    {
        std::wcout << L"[-] No chains stored.\n";
        Pause();
        return;
    }

    auto &chains = m_pointerChainStorage.GetAllChainsMutable();
    m_pointerChainResolver.ResolveAllChains(chains);

    m_pointerChainStorage.MarkModified();
    std::wcout << L"[+] All chains resolved!\n";
    Pause();
}

void ConsoleUI::ViewChainValuesFlow()
{
    ClearScreen();
    std::wcout << L"====================================================\n";
    std::wcout << L"            View Resolved Chain Values               \n";
    std::wcout << L"====================================================\n\n";

    if (m_pointerChainStorage.GetChainCount() == 0)
    {
        std::wcout << L"[-] No chains stored.\n";
        Pause();
        return;
    }

    const auto &chains = m_pointerChainStorage.GetAllChains();

    std::wcout << L"Resolved chains:\n\n";
    for (size_t i = 0; i < chains.size(); ++i)
    {
        const auto &chain = chains[i];
        std::wcout << L"[" << i + 1 << L"] " << chain.description.c_str() << L"\n";

        if (chain.isResolved)
        {
            std::wcout << L"    Module: " << chain.moduleName.c_str()
                       << L"\n    Base + Offset: 0x" << std::hex << chain.baseOffset << std::dec << L"\n";
            std::wcout << L"    Resolved Address: 0x" << std::hex << chain.resolvedAddress << std::dec << L"\n";
            std::wcout << L"    Value: " << chain.currentValue.ToString().c_str() << L"\n";
        }
        else
        {
            std::wcout << L"    [-] Failed to resolve: " << chain.lastError.c_str() << L"\n";
        }

        std::wcout << L"\n";
    }

    Pause();
}

void ConsoleUI::LoadChainsFromFileFlow()
{
    ClearScreen();
    std::wcout << L"====================================================\n";
    std::wcout << L"            Load Chains from File                    \n";
    std::wcout << L"====================================================\n\n";

    std::wstring filename = GetInput(L"Enter filename (e.g., chains.json)");

    if (m_pointerChainStorage.LoadFromFile(filename))
    {
        m_pointerChainStorage.ClearModified();
        std::wcout << L"[+] Chains loaded successfully!\n";
        std::wcout << L"[+] Total chains: " << m_pointerChainStorage.GetChainCount() << L"\n";
    }
    else
    {
        std::wcout << L"[-] Failed to load chains from file.\n";
    }

    Pause();
}

void ConsoleUI::SaveChainsToFileFlow()
{
    ClearScreen();
    std::wcout << L"====================================================\n";
    std::wcout << L"             Save Chains to File                     \n";
    std::wcout << L"====================================================\n\n";

    if (m_pointerChainStorage.GetChainCount() == 0)
    {
        std::wcout << L"[-] No chains to save.\n";
        Pause();
        return;
    }

    std::wstring filename = GetInput(L"Enter filename (e.g., chains.json)");

    if (m_pointerChainStorage.SaveToFile(filename))
    {
        m_pointerChainStorage.ClearModified();
        std::wcout << L"[+] Chains saved successfully!\n";
    }
    else
    {
        std::wcout << L"[-] Failed to save chains to file.\n";
    }

    Pause();
}

void ConsoleUI::PrintChainList()
{
    ClearScreen();
    std::wcout << L"====================================================\n";
    std::wcout << L"              Pointer Chains List                    \n";
    std::wcout << L"====================================================\n\n";

    m_pointerChainStorage.PrintAllChains();

    Pause();
}

// ============================================================================
// Utility Functions
// ============================================================================

void ConsoleUI::ClearScreen()
{
    system("cls");
}

void ConsoleUI::Pause()
{
    std::wcout << L"\nPress Enter to continue...";
    std::wcin.ignore((std::numeric_limits<std::streamsize>::max)(), L'\n');
    std::wcin.get();
}

std::wstring ConsoleUI::GetInput(const std::wstring &prompt)
{
    std::wcout << prompt << L": ";
    std::wstring input;
    std::getline(std::wcin, input);
    return input;
}

int ConsoleUI::GetChoice(const std::wstring &prompt, int min, int max)
{
    while (true)
    {
        std::wcout << prompt << L" [" << min << L"-" << max << L"]: ";
        std::wstring input;
        std::getline(std::wcin, input);

        try
        {
            int choice = std::stoi(input);
            if (choice >= min && choice <= max)
            {
                return choice;
            }
        }
        catch (...)
        {
        }

        std::wcout << L"Invalid input. Try again.\n";
    }
}

uintptr_t ConsoleUI::GetHexInput(const std::wstring &prompt)
{
    while (true)
    {
        std::wcout << prompt << L": ";
        std::wstring input;
        std::getline(std::wcin, input);

        // Remove 0x prefix if present
        if (input.length() >= 2 && (input.substr(0, 2) == L"0x" || input.substr(0, 2) == L"0X"))
        {
            input = input.substr(2);
        }

        try
        {
            uintptr_t value = std::stoull(input, nullptr, 16);
            return value;
        }
        catch (...)
        {
            std::wcout << L"Invalid hex value. Try again.\n";
        }
    }
}

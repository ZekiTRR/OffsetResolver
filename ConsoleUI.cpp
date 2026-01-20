#include "ConsoleUI.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

ConsoleUI::ConsoleUI(ProcessManager &pm, ModuleRegistry &mr, AddressResolver &ar, OffsetStorage &os)
    : m_processManager(pm), m_moduleRegistry(mr), m_addressResolver(ar), m_offsetStorage(os)
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
        std::wcout << L"  2. Module Dumper (Export module list to file)\n";
        std::wcout << L"  0. Exit\n\n";

        int choice = GetChoice(L"Select option", 0, 2);

        switch (choice)
        {
        case 1:
            ShowOffsetManagerMenu();
            break;
        case 2:
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

void ConsoleUI::ShowModuleDumperMenu()
{
    ClearScreen();
    std::wcout << L"====================================================\n";
    std::wcout << L"              Module Dumper Mode                     \n";
    std::wcout << L"====================================================\n\n";

    std::wstring processName = GetInput(L"Enter process name (e.g., hl2.exe)");

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
    std::wstring processName = GetInput(L"Enter process name (e.g., csgo.exe)");

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

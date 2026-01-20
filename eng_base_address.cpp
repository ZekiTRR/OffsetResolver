#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

// Structure to store module information
struct ModuleInfo
{
    std::wstring name;
    uintptr_t baseAddress;
    uintptr_t size;
};

// Function to get Process ID by name
DWORD GetProcessIdByName(const std::wstring &processName)
{
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnapshot, &pe32))
        {
            do
            {
                if (processName == pe32.szExeFile)
                {
                    pid = pe32.th32ProcessID;
                    break;
                }
            } while (Process32NextW(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    return pid;
}

// Function to get the list of process modules
std::vector<ModuleInfo> GetProcessModules(DWORD pid)
{
    std::vector<ModuleInfo> modules;
    // TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32 allows getting modules for both 32-bit and 64-bit processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32W me32;
        me32.dwSize = sizeof(MODULEENTRY32W);

        if (Module32FirstW(hSnapshot, &me32))
        {
            do
            {
                ModuleInfo info;
                info.name = me32.szModule;
                info.baseAddress = (uintptr_t)me32.modBaseAddr;
                info.size = me32.modBaseSize;
                modules.push_back(info);
            } while (Module32NextW(hSnapshot, &me32));
        }
        CloseHandle(hSnapshot);
    }
    return modules;
}

int main()
{
    // Set locale for correct display
    setlocale(LC_ALL, "");

    std::wcout << L"Enter process name (e.g., hl2.exe): ";
    std::wstring processName;
    std::wcin >> processName;

    DWORD pid = GetProcessIdByName(processName);

    if (pid == 0)
    {
        std::wcout << L"[-] Process '" << processName << L"' not found." << std::endl;
        system("pause");
        return 1;
    }

    std::wcout << L"[+] Process found. PID: " << pid << std::endl;

    std::wcout << L"Save module list and addresses to file? (y/n): ";
    wchar_t saveChoice;
    std::wcin >> saveChoice;
    bool saveToFile = (saveChoice == L'y' || saveChoice == L'Y');

    std::vector<ModuleInfo> modules = GetProcessModules(pid);

    if (modules.empty())
    {
        std::wcout << L"[-] Failed to get module list. Try running the program as administrator." << std::endl;
        system("pause");
        return 1;
    }

    std::wofstream file;
    std::string total_name = std::string(processName.begin(), processName.end()) + "_modules_dump.txt";

    if (saveToFile)
    {
        file.open(total_name);
        if (!file.is_open())
        {
            std::wcout << L"[-] Error creating file " << std::wstring(total_name.begin(), total_name.end()) << std::endl;
            saveToFile = false;
        }
    }

    std::wcout << L"\nModule List:\n";
    std::wcout << std::left << std::setw(35) << L"Module Name" << L" | " << L"Base Address" << std::endl;
    std::wcout << std::wstring(60, L'-') << std::endl;

    if (saveToFile)
    {
        file << std::left << std::setw(35) << L"Module Name" << L" | " << L"Base Address" << std::endl;
        file << std::wstring(60, L'-') << std::endl;
    }

    for (const auto &mod : modules)
    {
        // Output to console
        std::wcout << std::left << std::setw(35) << mod.name << L" | 0x"
                   << std::hex << std::uppercase << mod.baseAddress << std::dec << std::endl;

        // Write to file
        if (saveToFile)
        {
            file << std::left << std::setw(35) << mod.name << L" | 0x"
                 << std::hex << std::uppercase << mod.baseAddress << std::dec << std::endl;
        }
    }

    if (saveToFile)
    {
        std::wcout << L"\n[+] Data successfully saved to " << std::wstring(total_name.begin(), total_name.end()) << std::endl;
        file.close();
    }

    std::wcout << L"\nPress Enter to exit...";
    std::wcin.ignore();
    std::wcin.get();

    return 0;
}
#include "ProcessManager.h"
#include <iostream>

ProcessManager::ProcessManager()
    : m_pid(0), m_hProcess(NULL), m_isAttached(false)
{
}

ProcessManager::~ProcessManager()
{
    Detach();
}

bool ProcessManager::AttachToProcess(const std::wstring &processName)
{
    // If already attached, detach first
    if (m_isAttached)
    {
        Detach();
    }

    m_processName = processName;
    m_pid = FindProcessID(processName);

    if (m_pid == 0)
    {
        std::wcerr << L"[-] Process '" << processName << L"' not found." << std::endl;
        return false;
    }

    // Open process handle with read permissions
    m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_pid);

    if (m_hProcess == NULL)
    {
        std::wcerr << L"[-] Failed to open process. Error code: " << GetLastError() << std::endl;
        std::wcerr << L"[-] Try running as administrator." << std::endl;
        m_pid = 0;
        return false;
    }

    m_isAttached = true;
    std::wcout << L"[+] Successfully attached to process '" << processName
               << L"' (PID: " << m_pid << L")" << std::endl;
    return true;
}

void ProcessManager::Detach()
{
    if (m_hProcess != NULL)
    {
        CloseHandle(m_hProcess);
        m_hProcess = NULL;
    }
    m_pid = 0;
    m_isAttached = false;
}

DWORD ProcessManager::FindProcessID(const std::wstring &processName)
{
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"[-] Failed to create process snapshot." << std::endl;
        return 0;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32))
    {
        do
        {
            if (_wcsicmp(processName.c_str(), pe32.szExeFile) == 0)
            {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return pid;
}

#include "ProcessManager.h"
#include "DebugLog.h"
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
    DBG_STEP(L"Attaching to process: " + processName);

    // If already attached, detach first
    if (m_isAttached)
    {
        DBG_INFO(L"Already attached, detaching first...");
        Detach();
    }

    m_processName = processName;
    m_pid = FindProcessID(processName);

    if (m_pid == 0)
    {
        DBG_ERR(L"Process not found: " + processName);
        std::wcerr << L"[-] Process '" << processName << L"' not found." << std::endl;
        return false;
    }

    DBG_INFO(L"Found process PID: " + std::to_wstring(m_pid));

    // Open process handle with read permissions
    m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_pid);

    if (m_hProcess == NULL)
    {
        DWORD error = GetLastError();
        DBG_ERR(L"OpenProcess failed with error: " + std::to_wstring(error));
        std::wcerr << L"[-] Failed to open process. Error code: " << error << std::endl;
        std::wcerr << L"[-] Try running as administrator." << std::endl;
        m_pid = 0;
        return false;
    }

    m_isAttached = true;
    DBG_OK(L"Successfully attached to " + processName + L" (PID: " + std::to_wstring(m_pid) + L")");
    std::wcout << L"[+] Successfully attached to process '" << processName
               << L"' (PID: " << m_pid << L")" << std::endl;
    return true;
}

void ProcessManager::Detach()
{
    if (m_hProcess != NULL)
    {
        DBG_INFO(L"Detaching from process...");
        CloseHandle(m_hProcess);
        m_hProcess = NULL;
    }
    m_pid = 0;
    m_isAttached = false;
}

DWORD ProcessManager::FindProcessID(const std::wstring &processName)
{
    DBG_STEP(L"Searching for process: " + processName);
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        DBG_ERR(L"CreateToolhelp32Snapshot failed");
        std::wcerr << L"[-] Failed to create process snapshot." << std::endl;
        return 0;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    int processCount = 0;

    if (Process32FirstW(hSnapshot, &pe32))
    {
        do
        {
            processCount++;
            if (_wcsicmp(processName.c_str(), pe32.szExeFile) == 0)
            {
                pid = pe32.th32ProcessID;
                DBG_OK(L"Found matching process: " + std::wstring(pe32.szExeFile) + L" (PID: " + std::to_wstring(pid) + L")");
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    DBG_INFO(L"Scanned " + std::to_wstring(processCount) + L" processes");
    CloseHandle(hSnapshot);
    return pid;
}

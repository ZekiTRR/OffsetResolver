#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>

// ============================================================================
// ProcessManager: Process management
// Purpose: Find process by name, get PID, open handle
// ============================================================================

class ProcessManager
{
private:
    DWORD m_pid;
    HANDLE m_hProcess;
    std::wstring m_processName;
    bool m_isAttached;

public:
    ProcessManager();
    ~ProcessManager();

    // Find process by name
    bool AttachToProcess(const std::wstring &processName);

    // Detach from process
    void Detach();

    // Check if attached to process
    bool IsAttached() const { return m_isAttached; }

    // Get PID
    DWORD GetPID() const { return m_pid; }

    // Get handle
    HANDLE GetHandle() const { return m_hProcess; }

    // Get process name
    std::wstring GetProcessName() const { return m_processName; }

private:
    // Find PID by process name
    DWORD FindProcessID(const std::wstring &processName);
};

#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>

// ============================================================================
// ProcessManager: Управление процессами
// Назначение: поиск процесса по имени, получение PID, открытие дескриптора
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

    // Поиск процесса по имени
    bool AttachToProcess(const std::wstring &processName);

    // Отключение от процесса
    void Detach();

    // Проверка, открыт ли процесс
    bool IsAttached() const { return m_isAttached; }

    // Получение PID
    DWORD GetPID() const { return m_pid; }

    // Получение дескриптора
    HANDLE GetHandle() const { return m_hProcess; }

    // Получение имени процесса
    std::wstring GetProcessName() const { return m_processName; }

private:
    // Поиск PID по имени процесса
    DWORD FindProcessID(const std::wstring &processName);
};

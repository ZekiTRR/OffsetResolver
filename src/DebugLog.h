#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <Windows.h>

// ============================================================================
// DebugLog: Global debugging system
// Enable with 'debug' command in main menu
// File logging enabled with 'debugfile' command
// ============================================================================

class DebugLog
{
private:
    static bool s_enabled;
    static bool s_fileLogging;
    static std::wofstream s_logFile;
    static HANDLE s_consoleHandle;

public:
    static void Enable()
    {
        s_enabled = true;
        s_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    static void Disable() { s_enabled = false; }
    static bool IsEnabled() { return s_enabled; }
    static void Toggle()
    {
        s_enabled = !s_enabled;
        if (s_enabled)
            s_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    // File logging
    static bool IsFileLoggingEnabled() { return s_fileLogging; }

    static void EnableFileLogging(const std::wstring &filename = L"debug_log.txt")
    {
        if (s_logFile.is_open())
            s_logFile.close();

        s_logFile.open(filename, std::ios::out | std::ios::app);
        if (s_logFile.is_open())
        {
            s_fileLogging = true;
            // Write session header
            SYSTEMTIME st;
            GetLocalTime(&st);
            s_logFile << L"\n========== DEBUG SESSION START: "
                      << st.wYear << L"-" << st.wMonth << L"-" << st.wDay << L" "
                      << st.wHour << L":" << st.wMinute << L":" << st.wSecond
                      << L" ==========\n\n";
            s_logFile.flush();
        }
    }

    static void DisableFileLogging()
    {
        if (s_logFile.is_open())
        {
            s_logFile << L"\n========== DEBUG SESSION END ==========\n";
            s_logFile.close();
        }
        s_fileLogging = false;
    }

    static void ToggleFileLogging()
    {
        if (s_fileLogging)
            DisableFileLogging();
        else
            EnableFileLogging();
    }

    // Colors for different message types
    enum class Color : WORD
    {
        Default = 7,  // White
        Info = 11,    // Cyan
        Success = 10, // Green
        Warning = 14, // Yellow
        Error = 12,   // Red
        Debug = 13,   // Magenta
        Address = 9,  // Blue
    };

    static void SetColor(Color color)
    {
        if (s_consoleHandle)
            SetConsoleTextAttribute(s_consoleHandle, static_cast<WORD>(color));
    }

    static void ResetColor()
    {
        SetColor(Color::Default);
    }

private:
    // Internal method for writing to file
    static void WriteToFile(const std::wstring &msg)
    {
        if (s_fileLogging && s_logFile.is_open())
        {
            s_logFile << msg << std::endl;
            s_logFile.flush();
        }
    }

public:
    // === Colored logging methods ===

    static void Info(const std::wstring &msg)
    {
        if (!s_enabled)
            return;
        std::wstring fullMsg = L"[DEBUG INFO] " + msg;
        SetColor(Color::Info);
        std::wcout << fullMsg << std::endl;
        ResetColor();
        WriteToFile(fullMsg);
    }

    static void Success(const std::wstring &msg)
    {
        if (!s_enabled)
            return;
        std::wstring fullMsg = L"[DEBUG OK] " + msg;
        SetColor(Color::Success);
        std::wcout << fullMsg << std::endl;
        ResetColor();
        WriteToFile(fullMsg);
    }

    static void Warning(const std::wstring &msg)
    {
        if (!s_enabled)
            return;
        std::wstring fullMsg = L"[DEBUG WARN] " + msg;
        SetColor(Color::Warning);
        std::wcout << fullMsg << std::endl;
        ResetColor();
        WriteToFile(fullMsg);
    }

    static void Error(const std::wstring &msg)
    {
        if (!s_enabled)
            return;
        std::wstring fullMsg = L"[DEBUG ERROR] " + msg;
        SetColor(Color::Error);
        std::wcout << fullMsg << std::endl;
        ResetColor();
        WriteToFile(fullMsg);
    }

    static void Step(const std::wstring &msg)
    {
        if (!s_enabled)
            return;
        std::wstring fullMsg = L"[DEBUG STEP] " + msg;
        SetColor(Color::Debug);
        std::wcout << fullMsg << std::endl;
        ResetColor();
        WriteToFile(fullMsg);
    }

    // === Specialized methods for address debugging ===

    static void Address(const std::wstring &label, uintptr_t addr)
    {
        if (!s_enabled)
            return;
        SetColor(Color::Address);
        std::wcout << L"[DEBUG ADDR] " << label << L": 0x"
                   << std::hex << std::uppercase << addr << std::dec << std::endl;
        ResetColor();

        // Format string separately for file output
        if (s_fileLogging && s_logFile.is_open())
        {
            s_logFile << L"[DEBUG ADDR] " << label << L": 0x"
                      << std::hex << std::uppercase << addr << std::dec << std::endl;
            s_logFile.flush();
        }
    }

    static void PointerRead(uintptr_t fromAddr, uintptr_t value)
    {
        if (!s_enabled)
            return;
        SetColor(Color::Address);
        std::wcout << L"[DEBUG PTR] Read pointer at 0x" << std::hex << fromAddr
                   << L" -> 0x" << value << std::dec << std::endl;
        ResetColor();

        if (s_fileLogging && s_logFile.is_open())
        {
            s_logFile << L"[DEBUG PTR] Read pointer at 0x" << std::hex << fromAddr
                      << L" -> 0x" << value << std::dec << std::endl;
            s_logFile.flush();
        }
    }

    static void ChainStep(size_t step, size_t total, uintptr_t addr, uintptr_t offset, uintptr_t result)
    {
        if (!s_enabled)
            return;
        SetColor(Color::Debug);
        std::wcout << L"[DEBUG CHAIN] Step " << step << L"/" << total
                   << L": 0x" << std::hex << addr << L" + 0x" << offset
                   << L" = 0x" << result << std::dec << std::endl;
        ResetColor();

        if (s_fileLogging && s_logFile.is_open())
        {
            s_logFile << L"[DEBUG CHAIN] Step " << step << L"/" << total
                      << L": 0x" << std::hex << addr << L" + 0x" << offset
                      << L" = 0x" << result << std::dec << std::endl;
            s_logFile.flush();
        }
    }

    static void ModuleInfo(const std::wstring &name, uintptr_t baseAddr, size_t size)
    {
        if (!s_enabled)
            return;
        SetColor(Color::Info);
        std::wcout << L"[DEBUG MODULE] " << name << L" @ 0x"
                   << std::hex << baseAddr << L" (size: 0x" << size << L")"
                   << std::dec << std::endl;
        ResetColor();

        if (s_fileLogging && s_logFile.is_open())
        {
            s_logFile << L"[DEBUG MODULE] " << name << L" @ 0x"
                      << std::hex << baseAddr << L" (size: 0x" << size << L")"
                      << std::dec << std::endl;
            s_logFile.flush();
        }
    }

    static void MemoryRead(uintptr_t addr, size_t size, bool success)
    {
        if (!s_enabled)
            return;
        if (success)
        {
            SetColor(Color::Success);
            std::wcout << L"[DEBUG MEM] Read " << size << L" bytes from 0x"
                       << std::hex << addr << std::dec << L" - OK" << std::endl;
        }
        else
        {
            SetColor(Color::Error);
            std::wcout << L"[DEBUG MEM] Read " << size << L" bytes from 0x"
                       << std::hex << addr << std::dec << L" - FAILED" << std::endl;
        }
        ResetColor();

        if (s_fileLogging && s_logFile.is_open())
        {
            s_logFile << L"[DEBUG MEM] Read " << size << L" bytes from 0x"
                      << std::hex << addr << std::dec
                      << (success ? L" - OK" : L" - FAILED") << std::endl;
            s_logFile.flush();
        }
    }

    static void Value(const std::wstring &label, int32_t val)
    {
        if (!s_enabled)
            return;
        SetColor(Color::Info);
        std::wcout << L"[DEBUG VALUE] " << label << L": " << val << std::endl;
        ResetColor();
        WriteToFile(L"[DEBUG VALUE] " + label + L": " + std::to_wstring(val));
    }

    static void Value(const std::wstring &label, float val)
    {
        if (!s_enabled)
            return;
        SetColor(Color::Info);
        std::wcout << L"[DEBUG VALUE] " << label << L": " << val << std::endl;
        ResetColor();
        WriteToFile(L"[DEBUG VALUE] " + label + L": " + std::to_wstring(val));
    }

    static void Value(const std::wstring &label, double val)
    {
        if (!s_enabled)
            return;
        SetColor(Color::Info);
        std::wcout << L"[DEBUG VALUE] " << label << L": " << val << std::endl;
        ResetColor();
        WriteToFile(L"[DEBUG VALUE] " + label + L": " + std::to_wstring(val));
    }

    // Вывод информации о handle процесса
    static void HandleInfo(HANDLE handle)
    {
        if (!s_enabled)
            return;
        SetColor(Color::Info);
        std::wcout << L"[DEBUG HANDLE] Process handle: 0x" << std::hex << (uintptr_t)handle << std::dec;
        if (handle == NULL || handle == INVALID_HANDLE_VALUE)
            std::wcout << L" (INVALID!)";
        std::wcout << std::endl;
        ResetColor();

        if (s_fileLogging && s_logFile.is_open())
        {
            s_logFile << L"[DEBUG HANDLE] Process handle: 0x" << std::hex << (uintptr_t)handle << std::dec;
            if (handle == NULL || handle == INVALID_HANDLE_VALUE)
                s_logFile << L" (INVALID!)";
            s_logFile << std::endl;
            s_logFile.flush();
        }
    }
};

// Макросы для удобства
#define DBG_INFO(msg) DebugLog::Info(msg)
#define DBG_OK(msg) DebugLog::Success(msg)
#define DBG_WARN(msg) DebugLog::Warning(msg)
#define DBG_ERR(msg) DebugLog::Error(msg)
#define DBG_STEP(msg) DebugLog::Step(msg)
#define DBG_ADDR(label, addr) DebugLog::Address(label, addr)
#define DBG_PTR(from, val) DebugLog::PointerRead(from, val)
#define DBG_CHAIN(step, total, addr, off, res) DebugLog::ChainStep(step, total, addr, off, res)
#define DBG_MODULE(name, base, size) DebugLog::ModuleInfo(name, base, size)
#define DBG_MEM(addr, size, ok) DebugLog::MemoryRead(addr, size, ok)

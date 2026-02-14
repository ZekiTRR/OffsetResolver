#include "MemoryReader.h"
#include "DebugLog.h"
#include <iostream>
#include <iomanip>
#include <sstream>

MemoryReader::MemoryReader(HANDLE processHandle)
    : m_processHandle(processHandle), m_logErrors(true)
{
    DBG_INFO(L"MemoryReader initialized with process handle");
}

int32_t MemoryReader::ReadInt(uintptr_t address, bool &success)
{
    int32_t value = 0;
    success = ReadMemory(address, &value, sizeof(int32_t));
    if (success)
    {
        DebugLog::Value(L"ReadInt", value);
    }
    return value;
}

float MemoryReader::ReadFloat(uintptr_t address, bool &success)
{
    float value = 0.0f;
    success = ReadMemory(address, &value, sizeof(float));
    if (success)
    {
        DebugLog::Value(L"ReadFloat", value);
    }
    return value;
}

double MemoryReader::ReadDouble(uintptr_t address, bool &success)
{
    double value = 0.0;
    success = ReadMemory(address, &value, sizeof(double));
    if (success)
    {
        DebugLog::Value(L"ReadDouble", value);
    }
    return value;
}

uintptr_t MemoryReader::ReadPointer(uintptr_t address, bool &success)
{
    uintptr_t value = 0;
    success = ReadMemory(address, &value, sizeof(uintptr_t));
    return value;
}

bool MemoryReader::ReadMemory(uintptr_t address, void *buffer, size_t size)
{
    // Проверяем handle перед чтением
    if (m_processHandle == NULL || m_processHandle == INVALID_HANDLE_VALUE)
    {
        DBG_ERR(L"Process handle is invalid!");
        DebugLog::HandleInfo(m_processHandle);
        if (m_logErrors)
        {
            std::wcerr << L"[MemoryReader] Process handle is NULL or INVALID!" << std::endl;
        }
        return false;
    }

    SIZE_T bytesRead = 0;
    BOOL result = ReadProcessMemory(
        m_processHandle,
        (LPCVOID)address,
        buffer,
        size,
        &bytesRead);

    if (!result || bytesRead != size)
    {
        DWORD lastError = GetLastError();
        std::wostringstream hexStream;
        hexStream << L"0x" << std::hex << address;
        DBG_ERR(L"ReadProcessMemory failed at address: " + hexStream.str() +
                L", error code: " + std::to_wstring(lastError));
        if (m_logErrors)
        {
            std::wcerr << L"[MemoryReader] ReadProcessMemory failed:\n"
                       << L"  Address: 0x" << std::hex << address << std::dec << L"\n"
                       << L"  Expected bytes: " << size << L"\n"
                       << L"  Actual bytes read: " << bytesRead << L"\n"
                       << L"  Windows error: " << lastError;

            if (lastError == ERROR_PARTIAL_COPY)
            {
                std::wcerr << L" (ERROR_PARTIAL_COPY: Address may be invalid/inaccessible)";
            }
            else if (lastError == ERROR_INVALID_PARAMETER)
            {
                std::wcerr << L" (ERROR_INVALID_PARAMETER: Invalid handle or parameters)";
            }
            std::wcerr << L"\n";
        }
        return false;
    }

    return true;
}

bool MemoryReader::IsValidAddress(uintptr_t address) const
{
    return address >= MIN_VALID_ADDRESS && address <= MAX_VALID_ADDRESS;
}

std::wstring MemoryValue::ToString() const
{
    if (!isValid)
        return L"<invalid>";

    wchar_t buffer[256];
    switch (type)
    {
    case ValueType::INT:
        swprintf_s(buffer, L"%d", data.intValue);
        break;
    case ValueType::FLOAT:
        swprintf_s(buffer, L"%.6f", data.floatValue);
        break;
    case ValueType::DOUBLE:
        swprintf_s(buffer, L"%.15f", data.doubleValue);
        break;
    default:
        swprintf_s(buffer, L"<unknown type>");
        break;
    }
    return buffer;
}

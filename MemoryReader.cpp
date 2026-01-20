#include "MemoryReader.h"
#include <iostream>
#include <iomanip>

MemoryReader::MemoryReader(HANDLE processHandle)
    : m_processHandle(processHandle), m_logErrors(true)
{
}

int32_t MemoryReader::ReadInt(uintptr_t address, bool &success)
{
    int32_t value = 0;
    success = ReadMemory(address, &value, sizeof(int32_t));
    return value;
}

float MemoryReader::ReadFloat(uintptr_t address, bool &success)
{
    float value = 0.0f;
    success = ReadMemory(address, &value, sizeof(float));
    return value;
}

double MemoryReader::ReadDouble(uintptr_t address, bool &success)
{
    double value = 0.0;
    success = ReadMemory(address, &value, sizeof(double));
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
    // Validate address before reading
    if (!IsValidAddress(address))
    {
        if (m_logErrors)
        {
            std::wcerr << L"[MemoryReader] Invalid address: 0x"
                       << std::hex << address << std::dec << std::endl;
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
        if (m_logErrors)
        {
            std::wcerr << L"[MemoryReader] Failed to read from 0x"
                       << std::hex << address << std::dec << L" (bytes: "
                       << bytesRead << L"/" << size << L")" << std::endl;
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

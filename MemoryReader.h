#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>

// Value types for pointer chain reads
enum class ValueType
{
    INT,
    FLOAT,
    DOUBLE
};

// Represents a typed value read from memory
struct MemoryValue
{
    ValueType type;
    union
    {
        int32_t intValue;
        float floatValue;
        double doubleValue;
    } data;
    bool isValid;

    std::wstring ToString() const;
};

// Safe memory reader with validation and error handling
class MemoryReader
{
public:
    explicit MemoryReader(HANDLE processHandle);

    // Type-safe read operations with validation
    int32_t ReadInt(uintptr_t address, bool &success);
    float ReadFloat(uintptr_t address, bool &success);
    double ReadDouble(uintptr_t address, bool &success);
    uintptr_t ReadPointer(uintptr_t address, bool &success);

    // Generic memory read
    bool ReadMemory(uintptr_t address, void *buffer, size_t size);

    // Address validation
    bool IsValidAddress(uintptr_t address) const;
    void SetLogErrors(bool enabled) { m_logErrors = enabled; }

private:
    HANDLE m_processHandle;
    bool m_logErrors;

    // x64 user-space memory limits
    static constexpr uintptr_t MIN_VALID_ADDRESS = 0x10000;
    static constexpr uintptr_t MAX_VALID_ADDRESS = 0x7FFFFFFF0000;
};

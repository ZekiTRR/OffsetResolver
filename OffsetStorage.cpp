#include "OffsetStorage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

OffsetStorage::OffsetStorage()
    : m_isModified(false)
{
}

bool OffsetStorage::LoadFromFile(const std::wstring &filename)
{
    m_filename = filename;
    Clear();

    std::wifstream file(filename);
    if (!file.is_open())
    {
        std::wcerr << L"[-] Failed to open file: " << filename << std::endl;
        return false;
    }

    std::wstring line;
    int lineNumber = 0;

    while (std::getline(file, line))
    {
        lineNumber++;
        line = Trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == L'#' || line[0] == L';')
            continue;

        // Format: ModuleName+0xOffset=Description
        // Example: client.dll+0xDEA964=LocalPlayer

        size_t plusPos = line.find(L'+');
        size_t equalPos = line.find(L'=');

        if (plusPos == std::wstring::npos)
        {
            std::wcerr << L"[!] Invalid format at line " << lineNumber << L": " << line << std::endl;
            continue;
        }

        OffsetEntry entry;
        entry.moduleName = Trim(line.substr(0, plusPos));

        std::wstring offsetStr;
        if (equalPos != std::wstring::npos)
        {
            offsetStr = Trim(line.substr(plusPos + 1, equalPos - plusPos - 1));
            entry.description = Trim(line.substr(equalPos + 1));
        }
        else
        {
            offsetStr = Trim(line.substr(plusPos + 1));
        }

        if (!ParseHexValue(offsetStr, entry.offset))
        {
            std::wcerr << L"[!] Failed to parse offset at line " << lineNumber << L": " << offsetStr << std::endl;
            continue;
        }

        m_offsets.push_back(entry);
    }

    file.close();
    m_isModified = false;

    std::wcout << L"[+] Loaded " << m_offsets.size() << L" offsets from " << filename << std::endl;
    return true;
}

bool OffsetStorage::SaveToFile(const std::wstring &filename)
{
    m_filename = filename;

    std::wofstream file(filename);
    if (!file.is_open())
    {
        std::wcerr << L"[-] Failed to create file: " << filename << std::endl;
        return false;
    }

    // File header
    file << L"# Offset Configuration File\n";
    file << L"# Format: ModuleName+0xOffset=Description\n";
    file << L"# Example: client.dll+0xDEA964=LocalPlayer\n";
    file << L"#\n";
    file << L"# Note: Absolute addresses are NOT saved, only module+offset pairs\n\n";

    for (const auto &entry : m_offsets)
    {
        file << entry.moduleName << L"+0x" << std::hex << std::uppercase << entry.offset;

        if (!entry.description.empty())
        {
            file << L"=" << entry.description;
        }

        file << L"\n";
    }

    file.close();
    m_isModified = false;

    std::wcout << L"[+] Saved " << m_offsets.size() << L" offsets to " << filename << std::endl;
    return true;
}

bool OffsetStorage::Save()
{
    if (m_filename.empty())
    {
        std::wcerr << L"[-] No filename specified." << std::endl;
        return false;
    }
    return SaveToFile(m_filename);
}

void OffsetStorage::AddOffset(const OffsetEntry &entry)
{
    m_offsets.push_back(entry);
    m_isModified = true;
}

void OffsetStorage::Clear()
{
    m_offsets.clear();
    m_isModified = false;
}

void OffsetStorage::PrintOffsets() const
{
    if (m_offsets.empty())
    {
        std::wcout << L"[-] No offsets in storage." << std::endl;
        return;
    }

    std::wcout << L"\n=== Offset List ===\n";
    std::wcout << std::left << std::setw(20) << L"Module"
               << L" | " << std::setw(12) << L"Offset"
               << L" | " << std::setw(16) << L"Resolved Addr"
               << L" | " << L"Description" << std::endl;
    std::wcout << std::wstring(85, L'-') << std::endl;

    for (const auto &entry : m_offsets)
    {
        std::wcout << std::left << std::setw(20) << entry.moduleName
                   << L" | 0x" << std::hex << std::uppercase << std::setw(10) << entry.offset;

        if (entry.isResolved)
        {
            std::wcout << L" | 0x" << std::setw(14) << entry.resolvedAddress;
        }
        else
        {
            std::wcout << L" | " << std::setw(16) << L"[Not Resolved]";
        }

        std::wcout << L" | " << entry.description << std::dec << std::endl;
    }
    std::wcout << std::endl;
}

bool OffsetStorage::ParseHexValue(const std::wstring &str, uintptr_t &outValue)
{
    std::wstring cleanStr = str;

    // Удаляем префикс 0x или 0X если есть
    if (cleanStr.length() >= 2)
    {
        if (cleanStr.substr(0, 2) == L"0x" || cleanStr.substr(0, 2) == L"0X")
        {
            cleanStr = cleanStr.substr(2);
        }
    }

    // Проверяем, что строка содержит только hex символы
    for (wchar_t c : cleanStr)
    {
        if (!iswxdigit(c))
        {
            return false;
        }
    }

    std::wstringstream ss;
    ss << std::hex << cleanStr;
    ss >> outValue;

    return !ss.fail();
}

std::wstring OffsetStorage::Trim(const std::wstring &str)
{
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos)
        return L"";

    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, last - first + 1);
}

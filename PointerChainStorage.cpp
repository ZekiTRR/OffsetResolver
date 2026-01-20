#include "PointerChainStorage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Simple JSON builder for pointer chains without external dependencies
class SimpleJSON
{
public:
    static std::string ValueTypeToString(ValueType type)
    {
        switch (type)
        {
        case ValueType::INT:
            return "int";
        case ValueType::FLOAT:
            return "float";
        case ValueType::DOUBLE:
            return "double";
        default:
            return "int";
        }
    }

    static ValueType StringToValueType(const std::string &str)
    {
        if (str == "int")
            return ValueType::INT;
        if (str == "float")
            return ValueType::FLOAT;
        if (str == "double")
            return ValueType::DOUBLE;
        return ValueType::INT;
    }

    static std::string UintPtrToHexString(uintptr_t value)
    {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::uppercase << value;
        return oss.str();
    }

    static uintptr_t HexStringToUintPtr(const std::string &str)
    {
        try
        {
            return std::stoull(str, nullptr, 16);
        }
        catch (...)
        {
            return 0;
        }
    }
};

void PointerChainStorage::AddChain(const PointerChain &chain)
{
    m_chains.push_back(chain);
    m_modified = true;
}

void PointerChainStorage::RemoveChain(size_t index)
{
    if (index < m_chains.size())
    {
        m_chains.erase(m_chains.begin() + index);
        m_modified = true;
    }
}

bool PointerChainStorage::LoadFromFile(const std::wstring &filename)
{
    try
    {
        std::wifstream file(filename);
        if (!file.is_open())
        {
            std::wcerr << L"[-] Failed to open file: " << filename << std::endl;
            return false;
        }

        m_chains.clear();
        std::wstring line;

        while (std::getline(file, line))
        {
            // Skip empty lines and comments
            if (line.empty() || line[0] == L'#')
                continue;

            // Parse line: moduleName|baseOffset|offsets|valueType|description
            size_t pos1 = line.find(L'|');
            if (pos1 == std::wstring::npos)
                continue;

            size_t pos2 = line.find(L'|', pos1 + 1);
            if (pos2 == std::wstring::npos)
                continue;

            size_t pos3 = line.find(L'|', pos2 + 1);
            if (pos3 == std::wstring::npos)
                continue;

            size_t pos4 = line.find(L'|', pos3 + 1);
            if (pos4 == std::wstring::npos)
                continue;

            PointerChain chain;

            // Extract fields
            chain.moduleName = line.substr(0, pos1);

            std::string baseOffsetStr(line.substr(pos1 + 1, pos2 - pos1 - 1).begin(),
                                      line.substr(pos1 + 1, pos2 - pos1 - 1).end());
            chain.baseOffset = SimpleJSON::HexStringToUintPtr(baseOffsetStr);

            std::wstring offsetsStr = line.substr(pos2 + 1, pos3 - pos2 - 1);
            std::wistringstream offsetStream(offsetsStr);
            std::wstring offsetHex;
            while (std::getline(offsetStream, offsetHex, L','))
            {
                std::string offsetStr(offsetHex.begin(), offsetHex.end());
                chain.offsets.push_back(SimpleJSON::HexStringToUintPtr(offsetStr));
            }

            std::string valueTypeStr(line.substr(pos3 + 1, pos4 - pos3 - 1).begin(),
                                     line.substr(pos3 + 1, pos4 - pos3 - 1).end());
            chain.valueType = SimpleJSON::StringToValueType(valueTypeStr);

            chain.description = line.substr(pos4 + 1);

            m_chains.push_back(chain);
        }

        file.close();
        std::wcout << L"[+] Loaded " << m_chains.size() << L" pointer chains from file" << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[-] Error loading chains: " << e.what() << std::endl;
        return false;
    }
}

bool PointerChainStorage::SaveToFile(const std::wstring &filename) const
{
    try
    {
        std::wofstream file(filename);
        if (!file.is_open())
        {
            std::wcerr << L"[-] Failed to open file for writing: " << filename << std::endl;
            return false;
        }

        file << L"# Pointer Chains Configuration\n";
        file << L"# Format: moduleName|baseOffset|offsets|valueType|description\n\n";

        for (const auto &chain : m_chains)
        {
            file << chain.moduleName << L"|";
            file << std::wstring(SimpleJSON::UintPtrToHexString(chain.baseOffset).begin(),
                                 SimpleJSON::UintPtrToHexString(chain.baseOffset).end())
                 << L"|";

            for (size_t i = 0; i < chain.offsets.size(); ++i)
            {
                if (i > 0)
                    file << L",";
                file << std::wstring(SimpleJSON::UintPtrToHexString(chain.offsets[i]).begin(),
                                     SimpleJSON::UintPtrToHexString(chain.offsets[i]).end());
            }

            file << L"|";
            file << std::wstring(SimpleJSON::ValueTypeToString(chain.valueType).begin(),
                                 SimpleJSON::ValueTypeToString(chain.valueType).end())
                 << L"|";
            file << chain.description << L"\n";
        }

        file.close();
        std::wcout << L"[+] Saved " << m_chains.size() << L" pointer chains to file" << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[-] Error saving chains: " << e.what() << std::endl;
        return false;
    }
}

void PointerChainStorage::PrintAllChains() const
{
    if (m_chains.empty())
    {
        std::wcout << L"[-] No chains stored" << std::endl;
        return;
    }

    std::wcout << L"\n=== Pointer Chains ===\n\n";

    for (size_t i = 0; i < m_chains.size(); ++i)
    {
        const auto &chain = m_chains[i];
        std::wcout << L"[" << (i + 1) << L"] " << chain.description << L"\n";
        std::wcout << L"    Module: " << chain.moduleName << L"\n";
        std::wcout << L"    Base Offset: 0x" << std::hex << chain.baseOffset << std::dec << L"\n";
        std::wcout << L"    Chain Steps: " << chain.offsets.size() << L"\n";
        std::wcout << L"    Value Type: " << std::wstring(SimpleJSON::ValueTypeToString(chain.valueType).begin(), SimpleJSON::ValueTypeToString(chain.valueType).end()) << L"\n\n";
    }
}

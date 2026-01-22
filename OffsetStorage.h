#pragma once
#include <string>
#include <vector>
#include <map>

// ============================================================================
// OffsetStorage: Offset storage system
// Purpose: Load and save offsets in "module + offset" format
// File format: Simple text-based (INI-like)
// Absolute addresses are NOT saved, only module + offset
// ============================================================================

struct OffsetEntry
{
    std::wstring moduleName;  // Module name (e.g., "app.dll")
    uintptr_t offset;         // Offset relative to module base
    std::wstring description; // Description (e.g., "Pointer1")

    // Runtime data (not saved to file)
    uintptr_t resolvedAddress; // Absolute address (moduleBase + offset)
    bool isResolved;           // Flag indicating if address was resolved

    OffsetEntry()
        : offset(0), resolvedAddress(0), isResolved(false)
    {
    }
};

class OffsetStorage
{
private:
    std::vector<OffsetEntry> m_offsets;
    std::wstring m_filename;
    bool m_isModified;

public:
    OffsetStorage();

    // Load offsets from file
    bool LoadFromFile(const std::wstring &filename);

    // Save offsets to file
    bool SaveToFile(const std::wstring &filename);

    // Save using current filename
    bool Save();

    // Add new offset
    void AddOffset(const OffsetEntry &entry);

    // Get offset list
    const std::vector<OffsetEntry> &GetOffsets() const { return m_offsets; }
    std::vector<OffsetEntry> &GetOffsets() { return m_offsets; }

    // Clear offset list
    void Clear();

    // Check if modified
    bool IsModified() const { return m_isModified; }

    // Get offset count
    size_t Count() const { return m_offsets.size(); }

    // Print all offsets to console
    void PrintOffsets() const;

private:
    // Parse hex value string
    bool ParseHexValue(const std::wstring &str, uintptr_t &outValue);

    // Trim whitespace
    std::wstring Trim(const std::wstring &str);
};

#pragma once

#include <vector>
#include <string>
#include "PointerChainResolver.h"

// Storage and persistence for pointer chains
class PointerChainStorage
{
public:
    PointerChainStorage() = default;

    // Chain management
    void AddChain(const PointerChain &chain);
    void RemoveChain(size_t index);
    void ClearAllChains() { m_chains.clear(); }

    size_t GetChainCount() const { return m_chains.size(); }
    const PointerChain &GetChain(size_t index) const { return m_chains[index]; }
    PointerChain &GetChainMutable(size_t index) { return m_chains[index]; }
    const std::vector<PointerChain> &GetAllChains() const { return m_chains; }
    std::vector<PointerChain> &GetAllChainsMutable() { return m_chains; }

    // File I/O (uses JSON format)
    bool LoadFromFile(const std::wstring &filename);
    bool SaveToFile(const std::wstring &filename) const;

    // Display all chains
    void PrintAllChains() const;

    // Track modification state
    bool IsModified() const { return m_modified; }
    void MarkModified() { m_modified = true; }
    void ClearModified() { m_modified = false; }

private:
    std::vector<PointerChain> m_chains;
    bool m_modified = false;
};

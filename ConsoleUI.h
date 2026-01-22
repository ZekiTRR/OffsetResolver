#pragma once
#include "ProcessManager.h"
#include "ModuleRegistry.h"
#include "AddressResolver.h"
#include "OffsetStorage.h"
#include "MemoryReader.h"
#include "PointerChainResolver.h"
#include "PointerChainStorage.h"
#include <string>

// ============================================================================
// ConsoleUI: User interface
// Purpose: Console-based user interaction
// Menus, data input, result output
// ============================================================================

class ConsoleUI
{
private:
    ProcessManager &m_processManager;
    ModuleRegistry &m_moduleRegistry;
    AddressResolver &m_addressResolver;
    OffsetStorage &m_offsetStorage;
    MemoryReader &m_memoryReader;
    PointerChainResolver &m_pointerChainResolver;
    PointerChainStorage &m_pointerChainStorage;

    std::wstring m_currentConfigFile;

public:
    ConsoleUI(ProcessManager &pm, ModuleRegistry &mr, AddressResolver &ar, OffsetStorage &os,
              MemoryReader &mr2, PointerChainResolver &pcr, PointerChainStorage &pcs);

    // Main menu
    void ShowMainMenu();

    // === Mode menus ===
    void ShowOffsetManagerMenu();       // Offset management
    void ShowPointerChainManagerMenu(); // Pointer chain management
    void ShowModuleDumperMenu();        // Module dumper

private:
    // === Offset Manager Functions ===
    void AttachToProcessFlow();
    void LoadOffsetsFlow();
    void AddOffsetFlow();
    void ResolveOffsetsFlow();
    void ViewOffsetsFlow();
    void SaveOffsetsFlow();

    // === Pointer Chain Manager Functions ===
    void AddPointerChainFlow();
    void ResolveAllChainsFlow();
    void ViewChainValuesFlow();
    void LoadChainsFromFileFlow();
    void SaveChainsToFileFlow();
    void PrintChainList();

    // === Module Dumper Functions ===
    void DumpModulesToFile();

    // === Utilities ===
    void ClearScreen();
    void Pause();
    std::wstring GetInput(const std::wstring &prompt);
    int GetChoice(const std::wstring &prompt, int min, int max);
    uintptr_t GetHexInput(const std::wstring &prompt);
};

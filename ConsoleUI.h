#pragma once
#include "ProcessManager.h"
#include "ModuleRegistry.h"
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
    MemoryReader &m_memoryReader;
    PointerChainResolver &m_pointerChainResolver;
    PointerChainStorage &m_pointerChainStorage;

public:
    ConsoleUI(ProcessManager &pm, ModuleRegistry &mr,
              MemoryReader &mr2, PointerChainResolver &pcr, PointerChainStorage &pcs);

    // Main menu
    void ShowMainMenu();

    // === Mode menus ===
    void ShowPointerChainManagerMenu(); // Pointer chain management
    void ShowModuleDumperMenu();        // Module dumper
    void AddressToModuleOffsetFlow();   // Resolve runtime address -> module + offset

private:
    // === Common Functions ===
    void AttachToProcessFlow();

    // === Pointer Chain Manager Functions ===
    void AddPointerChainFlow();
    void ResolveAllChainsFlow();
    void ViewChainValuesFlow();
    void LoadChainsFromFileFlow();
    void SaveChainsToFileFlow();
    void PrintChainList();

    // === Structure Export Functions ===
    void ExportStructureFlow();

    // === Module Dumper Functions ===
    void DumpModulesToFile();

    // === Utilities ===
    void ClearScreen();
    void Pause();
    std::wstring GetInput(const std::wstring &prompt);
    int GetChoice(const std::wstring &prompt, int min, int max);
    uintptr_t GetHexInput(const std::wstring &prompt);
};

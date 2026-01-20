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
// ConsoleUI: Пользовательский интерфейс
// Назначение: взаимодействие с пользователем через консоль
// Меню, ввод данных, вывод результатов
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

    // Главное меню
    void ShowMainMenu();

    // === Меню режимов работы ===
    void ShowOffsetManagerMenu();       // Управление оффсетами
    void ShowPointerChainManagerMenu(); // Управление цепочками указателей
    void ShowModuleDumperMenu();        // Дамп модулей (из base_address.cpp)

private:
    // === Offset Manager Функции ===
    void AttachToProcessFlow();
    void LoadOffsetsFlow();
    void AddOffsetFlow();
    void ResolveOffsetsFlow();
    void ViewOffsetsFlow();
    void SaveOffsetsFlow();

    // === Pointer Chain Manager Функции ===
    void AddPointerChainFlow();
    void ResolveAllChainsFlow();
    void ViewChainValuesFlow();
    void LoadChainsFromFileFlow();
    void SaveChainsToFileFlow();
    void PrintChainList();

    // === Module Dumper Функции (из оригинального base_address.cpp) ===
    void DumpModulesToFile();

    // === Утилиты ===
    void ClearScreen();
    void Pause();
    std::wstring GetInput(const std::wstring &prompt);
    int GetChoice(const std::wstring &prompt, int min, int max);
    uintptr_t GetHexInput(const std::wstring &prompt);
};

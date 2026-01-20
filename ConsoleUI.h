#pragma once
#include "ProcessManager.h"
#include "ModuleRegistry.h"
#include "AddressResolver.h"
#include "OffsetStorage.h"
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

    std::wstring m_currentConfigFile;

public:
    ConsoleUI(ProcessManager &pm, ModuleRegistry &mr, AddressResolver &ar, OffsetStorage &os);

    // Главное меню
    void ShowMainMenu();

    // === Меню режимов работы ===
    void ShowOffsetManagerMenu(); // Управление оффсетами
    void ShowModuleDumperMenu();  // Дамп модулей (из base_address.cpp)

private:
    // === Offset Manager Функции ===
    void AttachToProcessFlow();
    void LoadOffsetsFlow();
    void AddOffsetFlow();
    void ResolveOffsetsFlow();
    void ViewOffsetsFlow();
    void SaveOffsetsFlow();

    // === Module Dumper Функции (из оригинального base_address.cpp) ===
    void DumpModulesToFile();

    // === Утилиты ===
    void ClearScreen();
    void Pause();
    std::wstring GetInput(const std::wstring &prompt);
    int GetChoice(const std::wstring &prompt, int min, int max);
    uintptr_t GetHexInput(const std::wstring &prompt);
};

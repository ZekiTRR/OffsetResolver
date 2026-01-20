#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <map>

// ============================================================================
// ModuleRegistry: Реестр модулей процесса
// Назначение: получение и хранение информации о загруженных модулях
// ImageBase, SizeOfImage для каждого модуля
// ============================================================================

struct ModuleInfo
{
    std::wstring name;     // Имя модуля (например, "client.dll")
    uintptr_t baseAddress; // ImageBase (базовый адрес)
    uintptr_t size;        // SizeOfImage (размер модуля)
};

class ModuleRegistry
{
private:
    std::vector<ModuleInfo> m_modules;
    std::map<std::wstring, ModuleInfo> m_moduleMap; // Для быстрого поиска по имени
    DWORD m_pid;
    bool m_isLoaded;

public:
    ModuleRegistry();

    // Загрузка списка модулей для указанного процесса
    bool LoadModules(DWORD pid);

    // Получение списка всех модулей
    const std::vector<ModuleInfo> &GetModules() const { return m_modules; }

    // Поиск модуля по имени (регистронезависимый)
    bool FindModule(const std::wstring &moduleName, ModuleInfo &outInfo) const;

    // Получение базового адреса модуля
    uintptr_t GetModuleBase(const std::wstring &moduleName) const;

    // Проверка, загружены ли модули
    bool IsLoaded() const { return m_isLoaded; }

    // Очистка реестра
    void Clear();

    // Вывод списка модулей в консоль
    void PrintModules() const;
};

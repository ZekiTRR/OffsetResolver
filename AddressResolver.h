#pragma once
#include "ModuleRegistry.h"
#include "OffsetStorage.h"

// ============================================================================
// AddressResolver: Разрешение адресов
// Назначение: пересчёт абсолютных адресов из пар "module + offset"
// Автоматически учитывает ASLR при каждом запуске процесса
// ============================================================================

class AddressResolver
{
private:
    const ModuleRegistry *m_moduleRegistry;

public:
    AddressResolver();

    // Установка реестра модулей для разрешения адресов
    void SetModuleRegistry(const ModuleRegistry *registry);

    // Разрешение одного оффсета
    bool ResolveOffset(OffsetEntry &entry);

    // Разрешение всех оффсетов в хранилище
    int ResolveAll(OffsetStorage &storage);

    // Вычисление абсолютного адреса вручную
    uintptr_t CalculateAddress(const std::wstring &moduleName, uintptr_t offset);
};

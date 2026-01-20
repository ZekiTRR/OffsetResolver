#pragma once
#include <string>
#include <vector>
#include <map>

// ============================================================================
// OffsetStorage: Хранилище оффсетов
// Назначение: загрузка и сохранение оффсетов в формате "module + offset"
// Формат файла: простой текстовый (INI-like)
// Абсолютные адреса НЕ сохраняются, только module + offset
// ============================================================================

struct OffsetEntry
{
    std::wstring moduleName;  // Имя модуля (например, "client.dll")
    uintptr_t offset;         // Оффсет относительно модуля
    std::wstring description; // Описание (например, "LocalPlayer")

    // Runtime данные (не сохраняются в файл)
    uintptr_t resolvedAddress; // Абсолютный адрес (moduleBase + offset)
    bool isResolved;           // Флаг, пересчитан ли адрес

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

    // Загрузка оффсетов из файла
    bool LoadFromFile(const std::wstring &filename);

    // Сохранение оффсетов в файл
    bool SaveToFile(const std::wstring &filename);

    // Сохранение с использованием текущего имени файла
    bool Save();

    // Добавление нового оффсета
    void AddOffset(const OffsetEntry &entry);

    // Получение списка оффсетов
    const std::vector<OffsetEntry> &GetOffsets() const { return m_offsets; }
    std::vector<OffsetEntry> &GetOffsets() { return m_offsets; }

    // Очистка списка
    void Clear();

    // Проверка, были ли изменения
    bool IsModified() const { return m_isModified; }

    // Количество оффсетов
    size_t Count() const { return m_offsets.size(); }

    // Вывод всех оффсетов в консоль
    void PrintOffsets() const;

private:
    // Парсинг строки с hex значением
    bool ParseHexValue(const std::wstring &str, uintptr_t &outValue);

    // Trim whitespace
    std::wstring Trim(const std::wstring &str);
};

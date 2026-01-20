# API Reference

Документация для расширения функционала приложения.

---

## ProcessManager

### Конструктор
```cpp
ProcessManager();
```

### Методы

#### AttachToProcess
```cpp
bool AttachToProcess(const std::wstring& processName);
```
Подключается к процессу по имени.

**Параметры**:
- `processName` — имя процесса (например, `L"example.exe"`)

**Возвращает**: `true` если успешно, `false` если процесс не найден или нет доступа

**Пример**:
```cpp
ProcessManager pm;
if (pm.AttachToProcess(L"notepad.exe")) {
    std::wcout << L"Attached to PID: " << pm.GetPID() << std::endl;
}
```

---

#### Detach
```cpp
void Detach();
```
Отключается от процесса, закрывает дескриптор.

---

#### IsAttached
```cpp
bool IsAttached() const;
```
Проверяет, подключён ли ProcessManager к процессу.

---

#### GetPID
```cpp
DWORD GetPID() const;
```
Возвращает PID процесса.

---

#### GetHandle
```cpp
HANDLE GetHandle() const;
```
Возвращает дескриптор процесса.

**Внимание**: Не закрывайте дескриптор вручную!

---

## ModuleRegistry

### Конструктор
```cpp
ModuleRegistry();
```

### Методы

#### LoadModules
```cpp
bool LoadModules(DWORD pid);
```
Загружает список модулей для указанного процесса.

**Параметры**:
- `pid` — ID процесса

**Возвращает**: `true` если модули загружены

**Пример**:
```cpp
ModuleRegistry registry;
if (registry.LoadModules(processManager.GetPID())) {
    registry.PrintModules();
}
```

---

#### FindModule
```cpp
bool FindModule(const std::wstring& moduleName, ModuleInfo& outInfo) const;
```
Ищет модуль по имени (регистронезависимо).

**Параметры**:
- `moduleName` — имя модуля (например, `L"client.dll"`)
- `outInfo` — выходная структура с информацией о модуле

**Возвращает**: `true` если модуль найден

**Пример**:
```cpp
ModuleInfo info;
if (registry.FindModule(L"client.dll", info)) {
    std::wcout << L"Base: 0x" << std::hex << info.baseAddress << std::endl;
}
```

---

#### GetModuleBase
```cpp
uintptr_t GetModuleBase(const std::wstring& moduleName) const;
```
Возвращает базовый адрес модуля или `0` если не найден.

**Пример**:
```cpp
uintptr_t clientBase = registry.GetModuleBase(L"client.dll");
if (clientBase != 0) {
    std::wcout << L"client.dll base: 0x" << std::hex << clientBase << std::endl;
}
```

---

#### GetModules
```cpp
const std::vector<ModuleInfo>& GetModules() const;
```
Возвращает вектор всех модулей.

**Пример**:
```cpp
for (const auto& mod : registry.GetModules()) {
    std::wcout << mod.name << L": 0x" << std::hex << mod.baseAddress << std::endl;
}
```

---

#### PrintModules
```cpp
void PrintModules() const;
```
Выводит отформатированный список модулей в консоль.

---

## OffsetStorage

### Конструктор
```cpp
OffsetStorage();
```

### Методы

#### LoadFromFile
```cpp
bool LoadFromFile(const std::wstring& filename);
```
Загружает оффсеты из файла.

**Формат файла**:
```ini
client.dll+0xDEA964=LocalPlayer
engine.dll+0x58EFC4=ViewAngles
```

**Возвращает**: `true` если файл успешно загружен

**Пример**:
```cpp
OffsetStorage storage;
if (storage.LoadFromFile(L"offsets.cfg")) {
    std::wcout << L"Loaded " << storage.Count() << L" offsets" << std::endl;
}
```

---

#### SaveToFile
```cpp
bool SaveToFile(const std::wstring& filename);
```
Сохраняет оффсеты в файл.

**Внимание**: Абсолютные адреса НЕ сохраняются!

**Возвращает**: `true` если успешно

---

#### AddOffset
```cpp
void AddOffset(const OffsetEntry& entry);
```
Добавляет новый оффсет в хранилище.

**Пример**:
```cpp
OffsetEntry entry;
entry.moduleName = L"client.dll";
entry.offset = 0xDEA964;
entry.description = L"LocalPlayer";

storage.AddOffset(entry);
```

---

#### GetOffsets
```cpp
const std::vector<OffsetEntry>& GetOffsets() const;
std::vector<OffsetEntry>& GetOffsets();
```
Возвращает вектор оффсетов.

**Пример**:
```cpp
for (const auto& offset : storage.GetOffsets()) {
    if (offset.isResolved) {
        std::wcout << offset.description << L": 0x" 
                   << std::hex << offset.resolvedAddress << std::endl;
    }
}
```

---

#### Clear
```cpp
void Clear();
```
Очищает все оффсеты из хранилища.

---

#### Count
```cpp
size_t Count() const;
```
Возвращает количество оффсетов в хранилище.

---

## AddressResolver

### Конструктор
```cpp
AddressResolver();
```

### Методы

#### SetModuleRegistry
```cpp
void SetModuleRegistry(const ModuleRegistry* registry);
```
Устанавливает реестр модулей для разрешения адресов.

**Важно**: Вызовите этот метод перед использованием `ResolveOffset()` или `ResolveAll()`!

**Пример**:
```cpp
AddressResolver resolver;
resolver.SetModuleRegistry(&moduleRegistry);
```

---

#### ResolveOffset
```cpp
bool ResolveOffset(OffsetEntry& entry);
```
Разрешает один оффсет, вычисляя абсолютный адрес.

**Параметры**:
- `entry` — структура оффсета (модифицируется)

**Возвращает**: `true` если успешно разрешён

**Пример**:
```cpp
OffsetEntry entry;
entry.moduleName = L"client.dll";
entry.offset = 0xDEA964;

if (resolver.ResolveOffset(entry)) {
    std::wcout << L"Resolved to: 0x" << std::hex << entry.resolvedAddress << std::endl;
}
```

---

#### ResolveAll
```cpp
int ResolveAll(OffsetStorage& storage);
```
Разрешает все оффсеты в хранилище.

**Возвращает**: Количество успешно разрешённых оффсетов

**Пример**:
```cpp
int resolved = resolver.ResolveAll(offsetStorage);
std::wcout << L"Resolved " << resolved << L"/" << offsetStorage.Count() << std::endl;
```

---

#### CalculateAddress
```cpp
uintptr_t CalculateAddress(const std::wstring& moduleName, uintptr_t offset);
```
Вычисляет абсолютный адрес вручную без изменения структуры.

**Возвращает**: Абсолютный адрес или `0` если модуль не найден

**Пример**:
```cpp
uintptr_t localPlayer = resolver.CalculateAddress(L"client.dll", 0xDEA964);
```

---

## Структуры данных

### ModuleInfo
```cpp
struct ModuleInfo {
    std::wstring name;          // Имя модуля
    uintptr_t baseAddress;      // Базовый адрес (ImageBase)
    uintptr_t size;             // Размер модуля (SizeOfImage)
};
```

---

### OffsetEntry
```cpp
struct OffsetEntry {
    std::wstring moduleName;    // Имя модуля
    uintptr_t offset;           // Оффсет от базы модуля
    std::wstring description;   // Описание
    
    // Runtime данные (не сохраняются):
    uintptr_t resolvedAddress;  // Абсолютный адрес
    bool isResolved;            // Флаг разрешения
    
    OffsetEntry();              // Конструктор по умолчанию
};
```

---

## Примеры расширения

### 1. Добавление Memory Reader

Добавьте в `ProcessManager.h`:

```cpp
class ProcessManager {
public:
    // ... существующие методы ...
    
    // Чтение памяти процесса
    template<typename T>
    bool ReadMemory(uintptr_t address, T& outValue) {
        if (!m_isAttached) return false;
        
        SIZE_T bytesRead;
        return ReadProcessMemory(
            m_hProcess,
            reinterpret_cast<LPCVOID>(address),
            &outValue,
            sizeof(T),
            &bytesRead
        ) && bytesRead == sizeof(T);
    }
    
    // Чтение массива байт
    bool ReadMemoryRaw(uintptr_t address, void* buffer, size_t size) {
        if (!m_isAttached) return false;
        
        SIZE_T bytesRead;
        return ReadProcessMemory(
            m_hProcess,
            reinterpret_cast<LPCVOID>(address),
            buffer,
            size,
            &bytesRead
        ) && bytesRead == size;
    }
};
```

**Использование**:
```cpp
ProcessManager pm;
pm.AttachToProcess(L"game.exe");

// Читаем int по адресу
int health;
if (pm.ReadMemory(0x7FF6A2DEA964, health)) {
    std::cout << "Health: " << health << std::endl;
}

// Читаем float массив
float position[3];
if (pm.ReadMemoryRaw(0x7FF6A2DEA970, position, sizeof(position))) {
    std::cout << "Position: " << position[0] << ", " 
              << position[1] << ", " << position[2] << std::endl;
}
```

---

### 2. Добавление Pattern Scanner

Добавьте в `ModuleRegistry.h`:

```cpp
class ModuleRegistry {
public:
    // ... существующие методы ...
    
    // Поиск паттерна в модуле
    bool FindPattern(
        const std::wstring& moduleName,
        const std::vector<byte>& pattern,
        const std::string& mask,
        uintptr_t& outAddress,
        HANDLE hProcess
    );
};
```

В `ModuleRegistry.cpp`:

```cpp
bool ModuleRegistry::FindPattern(
    const std::wstring& moduleName,
    const std::vector<byte>& pattern,
    const std::string& mask,
    uintptr_t& outAddress,
    HANDLE hProcess
) {
    ModuleInfo modInfo;
    if (!FindModule(moduleName, modInfo)) {
        return false;
    }
    
    // Читаем память модуля
    std::vector<byte> buffer(modInfo.size);
    SIZE_T bytesRead;
    if (!ReadProcessMemory(hProcess, 
                           reinterpret_cast<LPCVOID>(modInfo.baseAddress),
                           buffer.data(), 
                           modInfo.size, 
                           &bytesRead)) {
        return false;
    }
    
    // Поиск паттерна
    for (size_t i = 0; i < buffer.size() - pattern.size(); i++) {
        bool found = true;
        for (size_t j = 0; j < pattern.size(); j++) {
            if (mask[j] != '?' && buffer[i + j] != pattern[j]) {
                found = false;
                break;
            }
        }
        
        if (found) {
            outAddress = modInfo.baseAddress + i;
            return true;
        }
    }
    
    return false;
}
```

**Использование**:
```cpp
// Паттерн: 55 8B EC ?? ?? ?? E8
std::vector<byte> pattern = {0x55, 0x8B, 0xEC, 0x00, 0x00, 0x00, 0xE8};
std::string mask = "xxx????x";

uintptr_t address;
if (registry.FindPattern(L"client.dll", pattern, mask, address, pm.GetHandle())) {
    std::wcout << L"Pattern found at: 0x" << std::hex << address << std::endl;
}
```

---

### 3. Добавление Pointer Chain Resolver

Добавьте в `AddressResolver.h`:

```cpp
class AddressResolver {
public:
    // ... существующие методы ...
    
    // Разрешение цепочки указателей
    uintptr_t ResolvePointerChain(
        HANDLE hProcess,
        uintptr_t baseAddress,
        const std::vector<uintptr_t>& offsets
    );
};
```

В `AddressResolver.cpp`:

```cpp
uintptr_t AddressResolver::ResolvePointerChain(
    HANDLE hProcess,
    uintptr_t baseAddress,
    const std::vector<uintptr_t>& offsets
) {
    uintptr_t address = baseAddress;
    
    for (size_t i = 0; i < offsets.size(); i++) {
        // Читаем указатель
        uintptr_t ptr;
        SIZE_T bytesRead;
        if (!ReadProcessMemory(hProcess, 
                               reinterpret_cast<LPCVOID>(address),
                               &ptr, 
                               sizeof(ptr), 
                               &bytesRead)) {
            return 0;
        }
        
        // Добавляем оффсет
        address = ptr + offsets[i];
    }
    
    return address;
}
```

**Использование**:
```cpp
// Цепочка: [[client.dll + 0xDEA964] + 0x10] + 0x20
uintptr_t base = resolver.CalculateAddress(L"client.dll", 0xDEA964);
std::vector<uintptr_t> chain = {0x10, 0x20};

uintptr_t finalAddress = resolver.ResolvePointerChain(
    pm.GetHandle(), 
    base, 
    chain
);

if (finalAddress != 0) {
    std::wcout << L"Final address: 0x" << std::hex << finalAddress << std::endl;
}
```

---

### 4. JSON Support (с nlohmann/json)

Добавьте в `OffsetStorage.h`:

```cpp
#include <nlohmann/json.hpp>

class OffsetStorage {
public:
    // ... существующие методы ...
    
    bool LoadFromJSON(const std::wstring& filename);
    bool SaveToJSON(const std::wstring& filename);
};
```

В `OffsetStorage.cpp`:

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

bool OffsetStorage::SaveToJSON(const std::wstring& filename) {
    json j;
    j["version"] = "1.0";
    j["offsets"] = json::array();
    
    for (const auto& entry : m_offsets) {
        json offset;
        // Конвертируем wstring в string
        std::string modName(entry.moduleName.begin(), entry.moduleName.end());
        std::string desc(entry.description.begin(), entry.description.end());
        
        offset["module"] = modName;
        offset["offset"] = entry.offset;
        offset["description"] = desc;
        
        j["offsets"].push_back(offset);
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << j.dump(4); // Pretty print with 4 spaces
    file.close();
    
    return true;
}
```

**Формат JSON**:
```json
{
    "version": "1.0",
    "offsets": [
        {
            "module": "client.dll",
            "offset": 15443300,
            "description": "LocalPlayer"
        },
        {
            "module": "engine.dll",
            "offset": 5828548,
            "description": "ViewAngles"
        }
    ]
}
```

---

## Best Practices

### 1. Обработка ошибок
```cpp
if (!pm.AttachToProcess(L"game.exe")) {
    std::wcerr << L"Failed to attach to process" << std::endl;
    return 1;
}
```

### 2. RAII Pattern
```cpp
{
    ProcessManager pm;
    pm.AttachToProcess(L"game.exe");
    // ... работа с процессом ...
} // Деструктор автоматически вызовет Detach()
```

### 3. Проверка состояния
```cpp
if (!pm.IsAttached()) {
    std::wcerr << L"Not attached to any process" << std::endl;
    return;
}
```

### 4. Unicode Strings
```cpp
// Всегда используйте wstring для имён процессов/модулей
std::wstring processName = L"game.exe";
pm.AttachToProcess(processName);
```

---

## Thread Safety

**⚠️ Важно**: Текущая реализация **НЕ потокобезопасна**!

Если планируете многопоточность:

```cpp
#include <mutex>

class ProcessManager {
private:
    std::mutex m_mutex;
    
public:
    bool AttachToProcess(const std::wstring& processName) {
        std::lock_guard<std::mutex> lock(m_mutex);
        // ... оригинальный код ...
    }
};
```

---

## Заключение

Это API предоставляет базу для:
- Чтения памяти процессов
- Pattern scanning
- Pointer chain resolution
- Расширенного хранения данных

Для вопросов и предложений — см. исходный код и комментарии в `.h` файлах.

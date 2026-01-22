# Справочник API

**[English](../API.md) | [Русский](API.md)**

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

#### GetPID / GetHandle
```cpp
DWORD GetPID() const;
HANDLE GetHandle() const;
```
Возвращает PID и дескриптор процесса.

**Внимание**: Не закрывайте дескриптор вручную!

---

## ModuleRegistry

### Методы

#### LoadModules
```cpp
bool LoadModules(DWORD pid);
```
Загружает список модулей для указанного процесса.

---

#### FindModule
```cpp
bool FindModule(const std::wstring& moduleName, ModuleInfo& outInfo) const;
```
Ищет модуль по имени (регистронезависимо).

---

#### GetModuleBase
```cpp
uintptr_t GetModuleBase(const std::wstring& moduleName) const;
```
Возвращает базовый адрес модуля или `0` если не найден.

---

## OffsetStorage

### Методы

#### LoadFromFile / SaveToFile
```cpp
bool LoadFromFile(const std::wstring& filename);
bool SaveToFile(const std::wstring& filename);
```

**Формат файла**:
```ini
# Комментарий
app.dll+0xDEA964=PlayerBase
module2.dll+0x58EFC4=ViewAngles
```

**Внимание**: Абсолютные адреса НЕ сохраняются!

---

#### AddOffset
```cpp
void AddOffset(const OffsetEntry& entry);
```

**Пример**:
```cpp
OffsetEntry entry;
entry.moduleName = L"app.dll";
entry.offset = 0xDEA964;
entry.description = L"PlayerBase";
storage.AddOffset(entry);
```

---

## AddressResolver

### Методы

#### SetModuleRegistry
```cpp
void SetModuleRegistry(const ModuleRegistry* registry);
```
Устанавливает реестр модулей для разрешения адресов.

**Важно**: Вызовите этот метод перед использованием `ResolveOffset()` или `ResolveAll()`!

---

#### ResolveOffset / ResolveAll
```cpp
bool ResolveOffset(OffsetEntry& entry);
int ResolveAll(OffsetStorage& storage);
```
Разрешает оффсеты, вычисляя абсолютные адреса.

---

#### CalculateAddress
```cpp
uintptr_t CalculateAddress(const std::wstring& moduleName, uintptr_t offset);
```
Вычисляет абсолютный адрес вручную.

---

## PointerChainResolver

### Конструктор
```cpp
PointerChainResolver(MemoryReader& reader, ModuleRegistry& registry);
```

### Методы

#### ResolveChain
```cpp
bool ResolveChain(const PointerChainEntry& chain, 
                  uintptr_t& finalAddress, 
                  std::string& valueStr);
```
Разрешает многоуровневую цепочку указателей.

**Пример**:
```cpp
PointerChainEntry chain;
chain.moduleName = L"app.dll";
chain.baseOffset = 0x17E0A8;
chain.offsets = {0x18, 0x70, 0x2D0};
chain.valueType = "float";
chain.description = "Position";

uintptr_t finalAddr;
std::string value;
if (resolver.ResolveChain(chain, finalAddr, value)) {
    std::cout << "Value: " << value << std::endl;
}
```

---

## MemoryReader

### Методы

#### SetProcessHandle
```cpp
void SetProcessHandle(HANDLE processHandle);
```
Устанавливает дескриптор процесса для чтения памяти.

**Важно**: Вызывайте после подключения к процессу!

---

#### ReadMemory / Read
```cpp
bool ReadMemory(uintptr_t address, void* buffer, size_t size);

template<typename T>
bool Read(uintptr_t address, T& value);
```

**Пример**:
```cpp
int health;
if (reader.Read(0x7FF6A2DEA964, health)) {
    std::cout << "Health: " << health << std::endl;
}
```

---

## DebugLog (Статический класс)

### Управление состоянием

```cpp
static void Enable();
static void Disable();
static void Toggle();
static bool IsEnabled();
```

### Логирование в файл

```cpp
static void EnableFileLogging(const std::string& filename = "debug_log.txt");
static void DisableFileLogging();
```

### Методы логирования

```cpp
static void Info(const std::string& message);      // Информация (голубой)
static void Success(const std::string& message);   // Успех (зелёный)
static void Warning(const std::string& message);   // Предупреждение (жёлтый)
static void Error(const std::string& message);     // Ошибка (красный)
static void Step(const std::string& message);      // Шаг (пурпурный)
static void Address(const std::string& label, uintptr_t address);
static void PointerRead(uintptr_t address, uintptr_t value);
static void ChainStep(int step, int total, uintptr_t address, uintptr_t value);
```

---

## Структуры данных

### ModuleInfo
```cpp
struct ModuleInfo {
    std::wstring name;          // Имя модуля
    uintptr_t baseAddress;      // Базовый адрес (ImageBase)
    uintptr_t size;             // Размер модуля
};
```

### OffsetEntry
```cpp
struct OffsetEntry {
    std::wstring moduleName;    // Имя модуля
    uintptr_t offset;           // Смещение от базы модуля
    std::wstring description;   // Описание
    
    // Runtime (не сохраняются):
    uintptr_t resolvedAddress;
    bool isResolved;
};
```

### PointerChainEntry
```cpp
struct PointerChainEntry {
    std::wstring moduleName;        // Имя модуля
    uintptr_t baseOffset;           // Базовое смещение
    std::vector<uintptr_t> offsets; // Смещения цепочки
    std::string valueType;          // "int", "float", "double"
    std::string description;        // Описание
    
    // Runtime:
    uintptr_t resolvedAddress;
    std::string resolvedValue;
    bool isResolved;
};
```

---

## Макросы отладки

```cpp
#define DBG_INFO(msg)           DebugLog::Info(msg)
#define DBG_SUCCESS(msg)        DebugLog::Success(msg)
#define DBG_WARN(msg)           DebugLog::Warning(msg)
#define DBG_ERROR(msg)          DebugLog::Error(msg)
#define DBG_STEP(msg)           DebugLog::Step(msg)
#define DBG_ADDR(label, addr)   DebugLog::Address(label, addr)
#define DBG_PTR(addr, val)      DebugLog::PointerRead(addr, val)
#define DBG_CHAIN(s, t, a, v)   DebugLog::ChainStep(s, t, a, v)
#define DBG_MEM(type, val)      DebugLog::MemoryValue(type, val)
```

---

## Best Practices

### 1. Обработка ошибок
```cpp
if (!pm.AttachToProcess(L"example.exe")) {
    std::wcerr << L"Failed to attach" << std::endl;
    return 1;
}
```

### 2. Обновление дескриптора
```cpp
// После подключения обновите MemoryReader
if (pm.AttachToProcess(processName)) {
    memoryReader.SetProcessHandle(pm.GetHandle());
}
```

### 3. Режим отладки
```cpp
// Включите для диагностики
DebugLog::Enable();
DebugLog::EnableFileLogging();
// ... операции ...
DebugLog::Disable();
```

---

## Thread Safety

**⚠️ Важно**: Текущая реализация **НЕ потокобезопасна**!

Для многопоточности добавьте mutex:

```cpp
#include <mutex>

class ProcessManager {
private:
    std::mutex m_mutex;
    
public:
    bool AttachToProcess(const std::wstring& processName) {
        std::lock_guard<std::mutex> lock(m_mutex);
        // ... код ...
    }
};
```

---

## Заключение

API предоставляет базу для:
- Чтения памяти процессов
- ASLR-safe хранения оффсетов
- Разрешения цепочек указателей
- Отладки с цветным выводом и файловым логированием

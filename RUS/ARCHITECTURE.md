# Архитектура приложения

## 📐 Общая схема

```
┌─────────────────────────────────────────────────────────────┐
│                        ConsoleUI                            │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  Главное меню:                                       │   │
│  │  1. Offset Manager                                   │   │
│  │  2. Module Dumper                                    │   │
│  └──────────────────────────────────────────────────────┘   │
└──────────────┬──────────────────────┬────────────────────────┘
               │                      │
               ▼                      ▼
   ┌───────────────────────┐  ┌──────────────────────┐
   │  Offset Manager       │  │  Module Dumper       │
   │  (ASLR-safe storage)  │  │  (Export to file)    │
   └───────────────────────┘  └──────────────────────┘
               │
               ├──────────────────────────────────────────┐
               │                                          │
               ▼                                          ▼
   ┌───────────────────────┐                ┌──────────────────────┐
   │   ProcessManager      │◄───────────────│   ModuleRegistry     │
   │  - Find process       │                │  - Load modules      │
   │  - Get PID            │                │  - Store ImageBase   │
   │  - Open handle        │                │  - Fast lookup       │
   └───────────────────────┘                └──────────────────────┘
               │                                          │
               │                                          │
               ▼                                          ▼
   ┌───────────────────────┐                ┌──────────────────────┐
   │   OffsetStorage       │◄───────────────│   AddressResolver    │
   │  - Load from file     │                │  - Resolve offsets   │
   │  - Save to file       │                │  - Calculate address │
   │  - Store module+offset│                │  - Handle errors     │
   └───────────────────────┘                └──────────────────────┘
```

---

## 🔧 Модули и их обязанности

### 1. ConsoleUI (Пользовательский интерфейс)

**Ответственность**:
- Отображение меню
- Обработка ввода пользователя
- Оркестрация работы других модулей
- Форматированный вывод данных

**Ключевые методы**:
- `ShowMainMenu()` — главное меню выбора режима
- `ShowOffsetManagerMenu()` — меню управления оффсетами
- `ShowModuleDumperMenu()` — меню экспорта модулей

**Зависимости**: Все модули (ProcessManager, ModuleRegistry, AddressResolver, OffsetStorage)

---

### 2. ProcessManager (Управление процессами)

**Ответственность**:
- Поиск процесса по имени
- Получение PID
- Открытие дескриптора с необходимыми правами
- Проверка состояния подключения

**Ключевые методы**:
```cpp
bool AttachToProcess(const std::wstring& processName);
void Detach();
bool IsAttached() const;
DWORD GetPID() const;
HANDLE GetHandle() const;
```

**WinAPI функции**:
- `CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)` — снимок процессов
- `Process32FirstW()` / `Process32NextW()` — перечисление процессов
- `OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, ...)` — открытие

**Обработка ошибок**:
- Процесс не найден → возврат `false`, сообщение пользователю
- Нет прав доступа → предложение запустить от администратора

---

### 3. ModuleRegistry (Реестр модулей)

**Ответственность**:
- Получение списка всех модулей процесса
- Хранение информации: имя, ImageBase, SizeOfImage
- Быстрый поиск модуля по имени (регистронезависимый)

**Структуры данных**:
```cpp
struct ModuleInfo {
    std::wstring name;          // "client.dll"
    uintptr_t baseAddress;      // 0x7FF6A2000000
    uintptr_t size;             // 0x1A3C000
};
```

**Ключевые методы**:
```cpp
bool LoadModules(DWORD pid);
bool FindModule(const std::wstring& moduleName, ModuleInfo& outInfo);
uintptr_t GetModuleBase(const std::wstring& moduleName);
void PrintModules() const;
```

**WinAPI функции**:
- `CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid)`
- `Module32FirstW()` / `Module32NextW()` — перечисление модулей

**Оптимизация**:
- `std::map<std::wstring, ModuleInfo>` — для быстрого поиска O(log n)
- Имена модулей приводятся к нижнему регистру для регистронезависимого поиска

---

### 4. OffsetStorage (Хранилище оффсетов)

**Ответственность**:
- Загрузка оффсетов из текстового файла
- Сохранение оффсетов (только module+offset, БЕЗ абсолютных адресов)
- Хранение runtime данных (resolved addresses)

**Структуры данных**:
```cpp
struct OffsetEntry {
    std::wstring moduleName;     // "client.dll"
    uintptr_t offset;            // 0xDEA964
    std::wstring description;    // "LocalPlayer"
    
    // Runtime (не сохраняются в файл):
    uintptr_t resolvedAddress;   // 0x7FF6A2DEA964
    bool isResolved;             // true
};
```

**Формат файла**:
```ini
# Комментарий
client.dll+0xDEA964=LocalPlayer
engine.dll+0x58EFC4=ViewAngles
```

**Ключевые методы**:
```cpp
bool LoadFromFile(const std::wstring& filename);
bool SaveToFile(const std::wstring& filename);
void AddOffset(const OffsetEntry& entry);
void PrintOffsets() const;
```

**Парсинг**:
- Игнорирование комментариев (`#` и `;`)
- Разбор формата: `ModuleName+0xOffset=Description`
- Trim пробелов
- Поддержка hex значений с/без префикса `0x`

---

### 5. AddressResolver (Разрешение адресов)

**Ответственность**:
- Пересчёт абсолютных адресов из пар "module + offset"
- Формула: `resolved_address = module_base + offset`
- Обработка случаев, когда модуль не найден

**Ключевые методы**:
```cpp
void SetModuleRegistry(const ModuleRegistry* registry);
bool ResolveOffset(OffsetEntry& entry);
int ResolveAll(OffsetStorage& storage);
uintptr_t CalculateAddress(const std::wstring& moduleName, uintptr_t offset);
```

**Алгоритм разрешения**:
```cpp
1. Получить baseAddress модуля из ModuleRegistry
2. Если модуль не найден → isResolved = false, return false
3. resolved_address = baseAddress + offset
4. isResolved = true
5. return true
```

**Обработка ошибок**:
- Модуль не найден → помечаем оффсет как неразрешённый, выводим предупреждение
- ModuleRegistry не загружен → возврат ошибки

---

## 🔄 Типичные сценарии использования

### Сценарий 1: Первый запуск (создание конфигурации)

```
User Action                      System Response
───────────────────────────────  ─────────────────────────────────────
1. Start application             → ConsoleUI: ShowMainMenu()
2. Select "Offset Manager"       → ConsoleUI: ShowOffsetManagerMenu()
3. Select "Attach to process"    → ConsoleUI: AttachToProcessFlow()
4. Enter "example.exe"              → ProcessManager: AttachToProcess("example.exe")
                                 → Find PID via CreateToolhelp32Snapshot
                                 → OpenProcess(PROCESS_VM_READ, pid)
                                 → Success: "Attached to example.exe (PID: 12345)"
                                 
5. Auto-load modules             → ModuleRegistry: LoadModules(12345)
                                 → CreateToolhelp32Snapshot(TH32CS_SNAPMODULE)
                                 → Module32FirstW/NextW loop
                                 → Store all modules with base addresses
                                 → "Loaded 156 modules"
                                 
6. Select "Add new offset"       → ConsoleUI: AddOffsetFlow()
7. Enter "client.dll"            → Validate module exists in registry
8. Enter "0xDEA964"              → Parse hex value
9. Enter "LocalPlayer"           → Set description
                                 → OffsetStorage: AddOffset(entry)
                                 → "Offset added successfully"
                                 
10. Repeat steps 6-9 for more offsets

11. Select "Save offsets"        → ConsoleUI: SaveOffsetsFlow()
12. Enter "offsets.cfg"          → OffsetStorage: SaveToFile("offsets.cfg")
                                 → Write format: module+offset=description
                                 → "Saved 5 offsets to offsets.cfg"
```

---

### Сценарий 2: Повторный запуск (загрузка конфигурации)

```
User Action                      System Response
───────────────────────────────  ─────────────────────────────────────
1. Start application             → ConsoleUI: ShowMainMenu()
2. Select "Offset Manager"       → ConsoleUI: ShowOffsetManagerMenu()
3. Select "Attach to process"    → ProcessManager: AttachToProcess("csgo.exe")
                                 → NEW PID: 23456 (process restarted)
                                 → ModuleRegistry: LoadModules(23456)
                                 → NEW module bases due to ASLR!
                                 
4. Select "Load offsets"         → ConsoleUI: LoadOffsetsFlow()
5. Enter "offsets.cfg"           → OffsetStorage: LoadFromFile("offsets.cfg")
                                 → Parse file, load module+offset pairs
                                 → "Loaded 5 offsets from offsets.cfg"
                                 → Note: resolved addresses NOT loaded
                                 
6. Select "Resolve all offsets"  → ConsoleUI: ResolveOffsetsFlow()
                                 → AddressResolver: SetModuleRegistry(&registry)
                                 → AddressResolver: ResolveAll(storage)
                                 
                                 For each offset:
                                 - Get module base from ModuleRegistry
                                 - Calculate: resolved = base + offset
                                 - Mark as resolved
                                 
                                 → "Successfully resolved 5/5 offsets"
                                 
7. Select "View offsets"         → OffsetStorage: PrintOffsets()
                                 → Display table with NEW addresses:
                                 
                                   client.dll | 0xDEA964 | 0x7FF7A3DEA964 | LocalPlayer
                                   (previous: 0x7FF6A2DEA964 - ASLR changed!)
```

---

### Сценарий 3: Экспорт модулей

```
User Action                      System Response
───────────────────────────────  ─────────────────────────────────────
1. Start application             → ConsoleUI: ShowMainMenu()
2. Select "Module Dumper"        → ConsoleUI: ShowModuleDumperMenu()
3. Enter "notepad.exe"           → ProcessManager: AttachToProcess("notepad.exe")
                                 → ModuleRegistry: LoadModules(PID)
                                 → ModuleRegistry: PrintModules()
                                 → Display all modules with addresses
                                 
4. Choose to save                → ConsoleUI: DumpModulesToFile()
                                 → Create "notepad.exe_modules_dump.txt"
                                 → Write module list with bases
                                 → "Module list saved to notepad.exe_modules_dump.txt"
```

---

## 🧩 Взаимодействие модулей

### Зависимости:

```
ConsoleUI
  ├─► ProcessManager
  ├─► ModuleRegistry ─────► ProcessManager (needs PID)
  ├─► AddressResolver ────► ModuleRegistry (needs module bases)
  └─► OffsetStorage ──────► AddressResolver (for resolving)
```

### Потоки данных:

```
1. Process Name (User Input)
   └─► ProcessManager: Find PID
       └─► PID
           └─► ModuleRegistry: Enumerate modules
               └─► Module List (name, base, size)
                   └─► AddressResolver: Resolve offsets
                       └─► Resolved Addresses

2. Offset File (offsets.cfg)
   └─► OffsetStorage: Load module+offset pairs
       └─► AddressResolver: Calculate addresses
           └─► Resolved Offsets
               └─► ConsoleUI: Display
```

---

## 🔐 ASLR Protection Mechanism

### Проблема:
В Windows каждый раз при запуске процесса модули загружаются по случайным адресам (ASLR).

**Пример**:
```
Запуск 1: client.dll base = 0x7FF6A2000000
Запуск 2: client.dll base = 0x7FF7A3000000  ← ИЗМЕНИЛСЯ!
```

Если сохранить абсолютный адрес `0x7FF6A2DEA964`, он станет недействительным при следующем запуске.

### Решение:

Хранить **относительный оффсет** от базы модуля:

```
LocalPlayer = client.dll + 0xDEA964
```

При каждом запуске:
```cpp
uintptr_t newBase = GetModuleBase("client.dll");  // Новый base из-за ASLR
uintptr_t actualAddress = newBase + 0xDEA964;     // Пересчитываем
```

**Результат**:
```
Запуск 1: 0x7FF6A2000000 + 0xDEA964 = 0x7FF6A2DEA964 ✓
Запуск 2: 0x7FF7A3000000 + 0xDEA964 = 0x7FF7A3DEA964 ✓
```

Оффсет `0xDEA964` остаётся постоянным, меняется только base!

---

## 🚀 Точки расширения

### 1. Pattern Scanner

Добавить в `ModuleRegistry`:

```cpp
bool ModuleRegistry::FindPattern(
    const std::wstring& moduleName,
    const std::vector<byte>& pattern,
    const std::string& mask,
    uintptr_t& outAddress
);
```

Использование:
```cpp
// Поиск сигнатуры "55 8B EC ? ? ? E8"
std::vector<byte> pattern = {0x55, 0x8B, 0xEC, 0x00, 0x00, 0x00, 0xE8};
std::string mask = "xxx????x";
uintptr_t address;
if (registry.FindPattern(L"client.dll", pattern, mask, address)) {
    std::wcout << L"Pattern found at: 0x" << std::hex << address << std::endl;
}
```

### 2. Memory Reader

Добавить в `ProcessManager`:

```cpp
template<typename T>
bool ProcessManager::ReadMemory(uintptr_t address, T& outValue);

bool ProcessManager::ReadMemoryRaw(
    uintptr_t address,
    void* buffer,
    size_t size
);
```

### 3. Signature-based Offset Updater

Вместо хранения статических оффсетов, хранить сигнатуры:

```ini
# offsets_signatures.cfg
[LocalPlayer]
module=client.dll
signature=55 8B EC ? ? ? E8 ? ? ? ? 8B 0D
mask=xxx????x????xx
offset_from_found=+13
```

---

## 📊 Диаграмма последовательности (полный цикл)

```
User       ConsoleUI    ProcessMgr   ModuleReg   OffsetStorage   AddressResolver
 │              │            │            │             │                │
 ├─Start────────►│            │            │             │                │
 │              ├─Menu───────►│            │             │                │
 │              │            │            │             │                │
 ├─Attach─────► ├────────────►Find PID    │             │                │
 │              │            ├────────────►Load Modules │                │
 │              │            │            │             │                │
 ├─Load cfg──► ├────────────────────────────────────────►Parse file     │
 │              │            │            │             │                │
 ├─Resolve────► ├────────────────────────────────────────────────────────►
 │              │            │            │             │    For each:   │
 │              │            │            ◄─────────────────GetModuleBase│
 │              │            │            │             ◄────Calculate───┤
 │              │            │            │             │                │
 ├─View──────► ├────────────────────────────────────────►PrintOffsets   │
 │              │            │            │             │                │
```

---

## 📝 Примечания по дизайну

### ✅ Хорошие практики:

1. **Разделение ответственности**: Каждый модуль имеет одну чёткую задачу
2. **Минимальные зависимости**: Модули не знают друг о друге напрямую
3. **Обработка ошибок**: Каждая операция проверяется на успех
4. **Unicode support**: Полная поддержка Unicode для имён процессов/модулей
5. **RAII**: Автоматическое закрытие дескрипторов в деструкторах

### ⚠️ Ограничения:

1. Только Windows x64
2. Требуются права администратора для некоторых процессов
3. Не поддерживает protected/anti-cheat процессы
4. Простой формат хранения (INI), не JSON

### 🔮 Будущие улучшения:

1. JSON формат для конфигурации (с использованием nlohmann/json)
2. Pattern scanning для автоматического обновления оффсетов
3. Поддержка pointer chains (multi-level pointers)
4. GUI версия (Qt или ImGui)
5. Логирование в файл
6. History/Undo для изменений

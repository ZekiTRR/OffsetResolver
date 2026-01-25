# Application Architecture


## 📐 Overview Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                        ConsoleUI                            │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  Main Menu:                                          │   │
│  │  1. Offset Manager                                   │   │
│  │  2. Pointer Chain Manager                            │   │
│  │  3. Module Dumper                                    │   │
│  │  Commands: debug | debugfile                         │   │
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
               │
               │
               ▼
   ┌───────────────────────────────────────────────────────────────┐
   │                    Pointer Chain System                       │
   │  ┌─────────────────────┐  ┌─────────────────────────────────┐ │
   │  │PointerChainStorage  │  │   PointerChainResolver          │ │
   │  │- Store chains       │  │   - Multi-level pointer reading │ │
   │  │- Load/Save          │  │   - Step-by-step resolution     │ │
   │  └─────────────────────┘  └─────────────────────────────────┘ │
   │                                    │                          │
   │                                    ▼                          │
   │                         ┌───────────────────┐                 │
   │                         │   MemoryReader    │                 │
   │                         │- Safe memory read │                 │
   │                         │- Type conversion  │                 │
   │                         └───────────────────┘                 │
   └───────────────────────────────────────────────────────────────┘
               │
               ▼
   ┌───────────────────────┐
   │      DebugLog         │
   │  - Colored output     │
   │  - File logging       │
   │  - Step tracking      │
   └───────────────────────┘
```

---

## 🔧 Modules and Responsibilities

### 1. ConsoleUI (User Interface)

**Responsibility**:
- Display menus
- Handle user input
- Orchestrate other modules
- Formatted output

**Key Methods**:
- `ShowMainMenu()` — main mode selection menu
- `ShowOffsetManagerMenu()` — offset management menu
- `ShowPointerChainMenu()` — pointer chain management
- `ShowModuleDumperMenu()` — module export menu

**Debug Commands**:
- `debug` — enable debug mode with colored output
- `debugfile` — enable file logging to debug_log.txt

**Dependencies**: All modules

---

### 2. ProcessManager (Process Management)

**Responsibility**:
- Find process by name
- Get PID
- Open handle with necessary permissions
- Check attachment status

**Key Methods**:
```cpp
bool AttachToProcess(const std::wstring& processName);
void Detach();
bool IsAttached() const;
DWORD GetPID() const;
HANDLE GetHandle() const;
```

**WinAPI Functions**:
- `CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)` — process snapshot
- `Process32FirstW()` / `Process32NextW()` — enumerate processes
- `OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, ...)` — open handle

**Error Handling**:
- Process not found → return `false`, user message
- Access denied → suggest running as administrator

---

### 3. ModuleRegistry (Module Registry)

**Responsibility**:
- Get list of all process modules
- Store information: name, ImageBase, SizeOfImage
- Fast module lookup by name (case-insensitive)

**Data Structures**:
```cpp
struct ModuleInfo {
    std::wstring name;          // "app.dll"
    uintptr_t baseAddress;      // 0x7FF6A2000000
    uintptr_t size;             // 0x1A3C000
};
```

**Key Methods**:
```cpp
bool LoadModules(DWORD pid);
bool FindModule(const std::wstring& moduleName, ModuleInfo& outInfo);
uintptr_t GetModuleBase(const std::wstring& moduleName);
void PrintModules() const;
```

**WinAPI Functions**:
- `CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid)`
- `Module32FirstW()` / `Module32NextW()` — enumerate modules

**Optimization**:
- `std::map<std::wstring, ModuleInfo>` — for fast O(log n) lookup
- Module names converted to lowercase for case-insensitive search

---

### 4. OffsetStorage (Offset Storage)

**Responsibility**:
- Load offsets from text file
- Save offsets (only module+offset, WITHOUT absolute addresses)
- Store runtime data (resolved addresses)

**Data Structures**:
```cpp
struct OffsetEntry {
    std::wstring moduleName;     // "app.dll"
    uintptr_t offset;            // 0xDEA964
    std::wstring description;    // "PlayerBase"
    
    // Runtime (not saved to file):
    uintptr_t resolvedAddress;   // 0x7FF6A2DEA964
    bool isResolved;             // true
};
```

**File Format**:
```ini
# Comment
app.dll+0xDEA964=PlayerBase
module2.dll+0x58EFC4=ViewAngles
```

**Key Methods**:
```cpp
bool LoadFromFile(const std::wstring& filename);
bool SaveToFile(const std::wstring& filename);
void AddOffset(const OffsetEntry& entry);
void PrintOffsets() const;
```

**Parsing**:
- Ignore comments (`#` and `;`)
- Parse format: `ModuleName+0xOffset=Description`
- Trim whitespace
- Support hex values with/without `0x` prefix

---

### 5. AddressResolver (Address Resolution)

**Responsibility**:
- Recalculate absolute addresses from "module + offset" pairs
- Formula: `resolved_address = module_base + offset`
- Handle cases when module not found

**Key Methods**:
```cpp
void SetModuleRegistry(const ModuleRegistry* registry);
bool ResolveOffset(OffsetEntry& entry);
int ResolveAll(OffsetStorage& storage);
uintptr_t CalculateAddress(const std::wstring& moduleName, uintptr_t offset);
```

**Resolution Algorithm**:
```cpp
1. Get module baseAddress from ModuleRegistry
2. If module not found → isResolved = false, return false
3. resolved_address = baseAddress + offset
4. isResolved = true
5. return true
```

---

### 6. PointerChainResolver (Multi-Level Pointer Resolution)

**Responsibility**:
- Read multi-level pointer chains
- Step-by-step validation
- Final value reading with type support

**Key Methods**:
```cpp
bool ResolveChain(const PointerChainEntry& chain, 
                  uintptr_t& finalAddress, 
                  std::string& valueStr);
```

**Resolution Algorithm**:
```cpp
1. Get module base address
2. Calculate base: moduleBase + baseOffset
3. Read first pointer from base address
4. For each offset in chain:
   - Add offset to current address
   - Read pointer at that address
5. Read final value based on type (int, float, double, etc.)
```

**Debug Output** (when debug mode enabled):
```
[CHAIN] Resolving 'Health': app.dll+0x17E0A8 -> [0xEC]
[PTR]   Module base: 0x7FF6A2000000
[PTR]   Base address: 0x7FF6A217E0A8
[STEP]  Read pointer at 0x7FF6A217E0A8 = 0x22A14567890
[STEP]  After offset 0xEC: 0x22A1456797C
[MEM]   Final value (int): 100
```

---

### 7. MemoryReader (Safe Memory Reading)

**Responsibility**:
- Safe memory reading with validation
- Type-safe templates
- Handle validation

**Key Methods**:
```cpp
void SetProcessHandle(HANDLE processHandle);
bool ReadMemory(uintptr_t address, void* buffer, size_t size);

template<typename T>
bool Read(uintptr_t address, T& value);
```

**Safety Features**:
- NULL/INVALID_HANDLE validation
- ReadProcessMemory error checking
- Partial read detection

---

### 8. DebugLog (Debug System)

**Responsibility**:
- Global debug state management
- Colored console output
- File logging

**Key Methods**:
```cpp
static void Enable();
static void Disable();
static void Toggle();
static bool IsEnabled();
static void EnableFileLogging(const std::string& filename);
static void DisableFileLogging();
```

**Log Types**:
- `Info()` — general information (cyan)
- `Success()` — success messages (green)
- `Warning()` — warnings (yellow)
- `Error()` — errors (red)
- `Step()` — step indicators (magenta)
- `Address()` — address output (cyan)
- `PointerRead()` — pointer read results (yellow)
- `ChainStep()` — chain resolution steps (white)

---

## 🔄 Typical Usage Scenarios

### Scenario 1: First Run (Create Configuration)

```
User Action                      System Response
───────────────────────────────  ─────────────────────────────────────
1. Start application             → ConsoleUI: ShowMainMenu()
2. Select "Offset Manager"       → ConsoleUI: ShowOffsetManagerMenu()
3. Select "Attach to process"    → ConsoleUI: AttachToProcessFlow()
4. Enter "example.exe"           → ProcessManager: AttachToProcess()
                                 → Find PID via CreateToolhelp32Snapshot
                                 → OpenProcess(PROCESS_VM_READ, pid)
                                 → Success: "Attached to example.exe"
                                 
5. Auto-load modules             → ModuleRegistry: LoadModules()
                                 → CreateToolhelp32Snapshot(TH32CS_SNAPMODULE)
                                 → Store all modules with base addresses
                                 
6. Select "Add new offset"       → ConsoleUI: AddOffsetFlow()
7. Enter "app.dll"               → Validate module exists in registry
8. Enter "0xDEA964"              → Parse hex value
9. Enter "Pointer1"              → Set description
                                 → OffsetStorage: AddOffset(entry)
                                 
10. Select "Save offsets"        → OffsetStorage: SaveToFile()
```

---

### Scenario 2: Pointer Chain Resolution

```
User Action                      System Response
───────────────────────────────  ─────────────────────────────────────
1. Start with 'debug' command    → DebugLog::Enable()
2. Select "Pointer Chain Manager"→ ConsoleUI: ShowPointerChainMenu()
3. Attach to process             → ProcessManager + ModuleRegistry
4. Add chain:                    
   - Module: app.dll
   - Base offset: 0x17E0A8
   - Offsets: 0x18,0x70,0x2D0
   - Type: float
   - Name: Position
   
5. Resolve chains                → PointerChainResolver::ResolveChain()
                                 
   Debug output:
   [CHAIN] Resolving 'Position': app.dll+0x17E0A8 -> [0x18,0x70,0x2D0]
   [PTR]   Module base: 0x7FF6A2000000
   [PTR]   Base address: 0x7FF6A217E0A8
   [STEP]  Step 1/3: Read at 0x7FF6A217E0A8 = 0x22A14567890
   [STEP]  Step 2/3: 0x22A14567890 + 0x70 = 0x22A14567900
   [STEP]  Step 3/3: Read at 0x22A14567900 = 0x22A14568000
   [MEM]   Final address: 0x22A145682D0
   [MEM]   Value (float): 123.456
```

---

## 🔐 ASLR Protection Mechanism

### Problem:
In Windows, every time a process starts, modules load at random addresses (ASLR).

**Example**:
```
Run 1: app.dll base = 0x7FF6A2000000
Run 2: app.dll base = 0x7FF7A3000000  ← CHANGED!
```

If you save absolute address `0x7FF6A2DEA964`, it becomes invalid on next run.

### Solution:

Store **relative offset** from module base:

```
PlayerBase = app.dll + 0xDEA964
```

On each run:
```cpp
uintptr_t newBase = GetModuleBase("app.dll");  // New base due to ASLR
uintptr_t actualAddress = newBase + 0xDEA964;  // Recalculate
```

**Result**:
```
Run 1: 0x7FF6A2000000 + 0xDEA964 = 0x7FF6A2DEA964 ✓
Run 2: 0x7FF7A3000000 + 0xDEA964 = 0x7FF7A3DEA964 ✓
```

Offset `0xDEA964` stays constant, only base changes!

---

## 🧩 Module Interactions

### Dependencies:

```
ConsoleUI
  ├─► ProcessManager
  ├─► ModuleRegistry ─────► ProcessManager (needs PID)
  ├─► AddressResolver ────► ModuleRegistry (needs module bases)
  ├─► OffsetStorage ──────► AddressResolver (for resolving)
  ├─► PointerChainStorage
  ├─► PointerChainResolver ► MemoryReader (for reading memory)
  └─► DebugLog (global, static)
```

### Data Flows:

```
1. Process Name (User Input)
   └─► ProcessManager: Find PID
       └─► PID
           └─► ModuleRegistry: Enumerate modules
               └─► Module List (name, base, size)
                   └─► AddressResolver: Resolve offsets
                       └─► Resolved Addresses

2. Pointer Chain
   └─► PointerChainResolver: Step-by-step resolution
       └─► MemoryReader: Read at each level
           └─► Final Value
```

---

## 📊 Sequence Diagram (Full Cycle)

```
User       ConsoleUI    ProcessMgr   ModuleReg   ChainResolver  MemoryReader
 │              │            │            │             │              │
 ├─'debug'─────►│            │            │             │              │
 │              ├─Enable─────►DebugLog    │             │              │
 │              │            │            │             │              │
 ├─Attach─────► ├────────────►Find PID    │             │              │
 │              │            ├────────────►Load Modules │              │
 │              │            │            │             │              │
 ├─Add chain──► ├───────────────────────────────────────►Store chain   │
 │              │            │            │             │              │
 ├─Resolve────► ├───────────────────────────────────────►ResolveChain  │
 │              │            │            ◄─────────────┤GetModuleBase │
 │              │            │            │             ├──────────────►
 │              │            │            │             │   ReadMemory │
 │              │            │            │             ◄──────────────┤
 │              │            │            │             │(repeat steps)│
 │              │            │            │             ├──────────────►
 │              │            │            │             │   ReadValue  │
 │              │            │            │             ◄──────────────┤
 │              ◄─────────────────────────────────────────Result       │
 │              │            │            │             │              │
```

---

## 📝 Design Notes

### ✅ Good Practices:

1. **Separation of Concerns**: Each module has one clear responsibility
2. **Minimal Dependencies**: Modules don't know each other directly
3. **Error Handling**: Each operation is validated
4. **Unicode Support**: Full Unicode support for process/module names
5. **RAII**: Automatic handle cleanup in destructors
6. **Debug System**: Built-in debug mode with colored output and file logging

### ⚠️ Limitations:

1. Windows x64 only
2. Administrator rights required for some processes
3. Does not support protected/anti-cheat processes
4. Simple storage format (INI), not JSON

### 🔮 Future Improvements:

1. JSON configuration format (using nlohmann/json)
2. Pattern scanning for automatic offset updates
3. GUI version (Qt or ImGui)
4. Hot-reload configuration
5. Network export/import of offsets

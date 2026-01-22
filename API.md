# API Reference

**[English](API.md) | [Русский](RUS/API.md)**

Documentation for extending application functionality.

---

## ProcessManager

### Constructor
```cpp
ProcessManager();
```

### Methods

#### AttachToProcess
```cpp
bool AttachToProcess(const std::wstring& processName);
```
Attaches to a process by name.

**Parameters**:
- `processName` — process name (e.g., `L"example.exe"`)

**Returns**: `true` if successful, `false` if process not found or access denied

**Example**:
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
Disconnects from process, closes handle.

---

#### IsAttached
```cpp
bool IsAttached() const;
```
Checks if ProcessManager is attached to a process.

---

#### GetPID
```cpp
DWORD GetPID() const;
```
Returns process PID.

---

#### GetHandle
```cpp
HANDLE GetHandle() const;
```
Returns process handle.

**Warning**: Do not close the handle manually!

---

## ModuleRegistry

### Constructor
```cpp
ModuleRegistry();
```

### Methods

#### LoadModules
```cpp
bool LoadModules(DWORD pid);
```
Loads module list for specified process.

**Parameters**:
- `pid` — process ID

**Returns**: `true` if modules loaded

**Example**:
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
Finds module by name (case-insensitive).

**Parameters**:
- `moduleName` — module name (e.g., `L"app.dll"`)
- `outInfo` — output structure with module information

**Returns**: `true` if module found

**Example**:
```cpp
ModuleInfo info;
if (registry.FindModule(L"app.dll", info)) {
    std::wcout << L"Base: 0x" << std::hex << info.baseAddress << std::endl;
}
```

---

#### GetModuleBase
```cpp
uintptr_t GetModuleBase(const std::wstring& moduleName) const;
```
Returns module base address or `0` if not found.

**Example**:
```cpp
uintptr_t appBase = registry.GetModuleBase(L"app.dll");
if (appBase != 0) {
    std::wcout << L"app.dll base: 0x" << std::hex << appBase << std::endl;
}
```

---

#### GetModules
```cpp
const std::vector<ModuleInfo>& GetModules() const;
```
Returns vector of all modules.

---

#### PrintModules
```cpp
void PrintModules() const;
```
Prints formatted module list to console.

---

## OffsetStorage

### Constructor
```cpp
OffsetStorage();
```

### Methods

#### LoadFromFile
```cpp
bool LoadFromFile(const std::wstring& filename);
```
Loads offsets from file.

**File Format**:
```ini
# Comment
app.dll+0xDEA964=PlayerBase
module2.dll+0x58EFC4=ViewAngles
```

**Returns**: `true` if file successfully loaded

**Example**:
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
Saves offsets to file.

**Note**: Absolute addresses are NOT saved!

**Returns**: `true` if successful

---

#### AddOffset
```cpp
void AddOffset(const OffsetEntry& entry);
```
Adds new offset to storage.

**Example**:
```cpp
OffsetEntry entry;
entry.moduleName = L"app.dll";
entry.offset = 0xDEA964;
entry.description = L"PlayerBase";

storage.AddOffset(entry);
```

---

#### GetOffsets
```cpp
const std::vector<OffsetEntry>& GetOffsets() const;
std::vector<OffsetEntry>& GetOffsets();
```
Returns offsets vector.

---

#### Clear / Count
```cpp
void Clear();           // Clears all offsets
size_t Count() const;   // Returns offset count
```

---

## AddressResolver

### Constructor
```cpp
AddressResolver();
```

### Methods

#### SetModuleRegistry
```cpp
void SetModuleRegistry(const ModuleRegistry* registry);
```
Sets module registry for address resolution.

**Important**: Call this method before using `ResolveOffset()` or `ResolveAll()`!

---

#### ResolveOffset
```cpp
bool ResolveOffset(OffsetEntry& entry);
```
Resolves single offset, calculating absolute address.

**Parameters**:
- `entry` — offset structure (modified)

**Returns**: `true` if successfully resolved

---

#### ResolveAll
```cpp
int ResolveAll(OffsetStorage& storage);
```
Resolves all offsets in storage.

**Returns**: Number of successfully resolved offsets

---

#### CalculateAddress
```cpp
uintptr_t CalculateAddress(const std::wstring& moduleName, uintptr_t offset);
```
Calculates absolute address manually without modifying structure.

**Returns**: Absolute address or `0` if module not found

---

## PointerChainResolver

### Constructor
```cpp
PointerChainResolver(MemoryReader& reader, ModuleRegistry& registry);
```

### Methods

#### ResolveChain
```cpp
bool ResolveChain(const PointerChainEntry& chain, 
                  uintptr_t& finalAddress, 
                  std::string& valueStr);
```
Resolves multi-level pointer chain.

**Parameters**:
- `chain` — pointer chain definition
- `finalAddress` — output final address
- `valueStr` — output value as string

**Returns**: `true` if chain successfully resolved

**Example**:
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

### Constructor
```cpp
MemoryReader(HANDLE processHandle = nullptr);
```

### Methods

#### SetProcessHandle
```cpp
void SetProcessHandle(HANDLE processHandle);
```
Sets process handle for memory reading.

**Important**: Call this after attaching to process!

---

#### ReadMemory
```cpp
bool ReadMemory(uintptr_t address, void* buffer, size_t size);
```
Reads raw memory.

**Parameters**:
- `address` — memory address to read
- `buffer` — output buffer
- `size` — bytes to read

**Returns**: `true` if successful

---

#### Read (Template)
```cpp
template<typename T>
bool Read(uintptr_t address, T& value);
```
Type-safe memory read.

**Example**:
```cpp
int health;
if (reader.Read(0x7FF6A2DEA964, health)) {
    std::cout << "Health: " << health << std::endl;
}
```

---

## DebugLog (Static Class)

### Methods

#### Enable / Disable / Toggle
```cpp
static void Enable();
static void Disable();
static void Toggle();
static bool IsEnabled();
```
Controls debug output state.

**Example**:
```cpp
DebugLog::Enable();  // Turn on debug mode
```

---

#### EnableFileLogging
```cpp
static void EnableFileLogging(const std::string& filename = "debug_log.txt");
static void DisableFileLogging();
```
Controls file logging.

**Example**:
```cpp
DebugLog::EnableFileLogging("my_debug.txt");
```

---

#### Logging Methods
```cpp
static void Info(const std::string& message);
static void Success(const std::string& message);
static void Warning(const std::string& message);
static void Error(const std::string& message);
static void Step(const std::string& message);
static void Address(const std::string& label, uintptr_t address);
static void PointerRead(uintptr_t address, uintptr_t value);
static void ChainStep(int step, int total, uintptr_t address, uintptr_t value);
```

**Example**:
```cpp
DBG_INFO("Starting pointer chain resolution");
DBG_ADDR("Module base", 0x7FF6A2000000);
DBG_STEP("Reading first pointer");
DBG_SUCCESS("Chain resolved successfully");
```

---

## Data Structures

### ModuleInfo
```cpp
struct ModuleInfo {
    std::wstring name;          // Module name
    uintptr_t baseAddress;      // Base address (ImageBase)
    uintptr_t size;             // Module size (SizeOfImage)
};
```

---

### OffsetEntry
```cpp
struct OffsetEntry {
    std::wstring moduleName;    // Module name
    uintptr_t offset;           // Offset from module base
    std::wstring description;   // Description
    
    // Runtime data (not saved):
    uintptr_t resolvedAddress;  // Absolute address
    bool isResolved;            // Resolution flag
};
```

---

### PointerChainEntry
```cpp
struct PointerChainEntry {
    std::wstring moduleName;        // Module name
    uintptr_t baseOffset;           // Base offset from module
    std::vector<uintptr_t> offsets; // Pointer chain offsets
    std::string valueType;          // "int", "float", "double", etc.
    std::string description;        // Chain description
    
    // Runtime data:
    uintptr_t resolvedAddress;
    std::string resolvedValue;
    bool isResolved;
};
```

---

## Debug Macros

Convenience macros for debugging (defined in DebugLog.h):

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

### 1. Error Handling
```cpp
if (!pm.AttachToProcess(L"example.exe")) {
    std::wcerr << L"Failed to attach to process" << std::endl;
    return 1;
}
```

### 2. RAII Pattern
```cpp
{
    ProcessManager pm;
    pm.AttachToProcess(L"example.exe");
    // ... work with process ...
} // Destructor automatically calls Detach()
```

### 3. Handle Updates
```cpp
// After attaching to process, update MemoryReader handle
if (pm.AttachToProcess(processName)) {
    memoryReader.SetProcessHandle(pm.GetHandle());
}
```

### 4. Unicode Strings
```cpp
// Always use wstring for process/module names
std::wstring processName = L"example.exe";
pm.AttachToProcess(processName);
```

### 5. Debug Mode
```cpp
// Enable debug for troubleshooting
DebugLog::Enable();
DebugLog::EnableFileLogging();

// ... perform operations ...

// Disable when done
DebugLog::Disable();
DebugLog::DisableFileLogging();
```

---

## Thread Safety

**⚠️ Important**: Current implementation is **NOT thread-safe**!

For multithreading, add mutex protection:

```cpp
#include <mutex>

class ProcessManager {
private:
    std::mutex m_mutex;
    
public:
    bool AttachToProcess(const std::wstring& processName) {
        std::lock_guard<std::mutex> lock(m_mutex);
        // ... original code ...
    }
};
```

---

## Conclusion

This API provides foundation for:
- Process memory reading
- ASLR-safe offset storage
- Multi-level pointer chain resolution
- Debug logging with colored output and file support

For questions and suggestions — see source code and comments in `.h` files.

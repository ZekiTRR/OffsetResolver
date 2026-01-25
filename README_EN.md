# Process Module & Offset Management Tool

Console application for Windows (x64) for managing offsets, pointer chains, and process modules in reverse engineering context.


## 🎯 Purpose

- **ASLR Protection**: Store offsets in "module + offset" format instead of absolute addresses
- **Pointer Chain Resolution**: Resolve multi-level pointer chains with automatic memory reading
- **Automatic Recalculation**: Automatically recalculate actual addresses on each launch
- **Module Export**: Save process module list with base addresses
- **Debug Mode**: Built-in debugging system with colored console output and file logging
- **Portability**: Configuration works after process/system restart

---

## 🏗️ Architecture

Application divided into 8 logical modules:

| Module | Purpose |
|--------|---------|
| **ProcessManager** | Process search, PID retrieval, handle management |
| **ModuleRegistry** | Module enumeration, ImageBase/Size storage |
| **AddressResolver** | Calculate absolute addresses from module+offset |
| **OffsetStorage** | Load/save offsets in INI format |
| **MemoryReader** | Safe memory reading with validation |
| **PointerChainResolver** | Multi-level pointer chain resolution |
| **PointerChainStorage** | Save/load pointer chains |
| **DebugLog** | Debug output with colors and file logging |
| **ConsoleUI** | Interactive menu system |

---

## 📝 Configuration Formats

### Offset Configuration (`offsets.cfg`):

```ini
# Format: ModuleName+0xOffset=Description
app.dll+0xDEA964=Pointer1
app.dll+0x4DCC098=Pointer2
module2.dll+0x58EFC4=Pointer3
```

### Pointer Chain Configuration (`chains.txt`):

```
# Format: moduleName|baseOffset|offsets|valueType|description
app.dll|0x17E0A8|0xEC|int|Health
app.dll|0x17E0A8|0xF0|int|MaxHealth
app.dll|0x17E0A8|0x18,0x70,0x2D0|float|PositionX
```

---

## 🚀 Usage

### Compilation

All .cpp files together:

```bash
cl /EHsc /std:c++17 /O2 /DUNICODE /D_UNICODE main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp
```

Or with CMake / Visual Studio.

### Running

1. **Main Menu** — choose mode:
   - **Offset Manager** — offset management with ASLR protection
   - **Module Dumper** — quick export of module list (from `base_address.cpp`)

---

## 🎓 Usage Scenarios

### Scenario 1: Offset Management

```
1. Choose "Offset Manager"
2. Attach to process → example.exe
3. Add new offset:
   - Module: app.dll
   - Offset: 0xDEA964
   - Description: MyPointer
4. Save offsets → offsets.cfg
5. On next run: load offsets → resolve → see new addresses
```

### Scenario 2: Pointer Chain Resolution

```
1. Choose "Pointer Chain Manager"
2. Attach to process → example.exe
3. Add pointer chain:
   - Module: app.dll
   - Base offset: 0x17E0A8
   - Offsets: 0xEC (or 0x18,0x70,0x2D0 for multi-level)
   - Type: int
4. Resolve chains → view values
```

### Scenario 3: Debug Mode

```
1. Type "debug" in main menu → enable detailed logging
2. Type "debugfile" → also save to debug_log.txt
3. Perform operations → see step-by-step logs
4. Check debug_log.txt for history
```

---

## 🔧 Debug System

Built-in debugging provides:

- **Colored console output**: Info (cyan), Success (green), Warning (yellow), Error (red)
- **Step-by-step logging**: Track every operation
- **Address tracing**: See pointer chain resolution step by step
- **File logging**: Save debug output to `debug_log.txt`
- **Handle validation**: Detect invalid handles early

---

## ✅ Features

| Feature | Status |
|---------|--------|
| Offset storage (module+offset) | ✅ |
| Multi-level pointer chains | ✅ |
| Memory reading (int/float/double) | ✅ |
| ASLR support | ✅ |
| Debug mode | ✅ |
| File logging | ✅ |
| No external dependencies | ✅ |

---

## 📊 Data Structures

### OffsetEntry
```cpp
struct OffsetEntry {
    std::wstring moduleName;     // app.dll
    uintptr_t offset;            // 0xDEA964
    std::wstring description;    // Pointer1
    uintptr_t resolvedAddress;   // Runtime: calculated address
    bool isResolved;             // Runtime: resolution status
};
```

### PointerChain
```cpp
struct PointerChain {
    std::wstring moduleName;        // app.dll
    uintptr_t baseOffset;           // 0x17E0A8
    std::vector<uintptr_t> offsets; // [0x18, 0x70, 0x2D0]
    ValueType valueType;            // INT / FLOAT / DOUBLE
    std::wstring description;       // Health
    uintptr_t resolvedAddress;      // Runtime: final address
    MemoryValue currentValue;       // Runtime: read value
    bool isResolved;
    std::wstring lastError;
};
```

---

## 📁 Project Structure

```
Process-Module-Dumper/
├── Source Code (11 files)
│   ├── main.cpp
│   ├── ProcessManager.h/.cpp
│   ├── ModuleRegistry.h/.cpp
│   ├── AddressResolver.h/.cpp
│   ├── OffsetStorage.h/.cpp
│   ├── MemoryReader.h/.cpp
│   ├── PointerChainResolver.h/.cpp
│   ├── PointerChainStorage.h/.cpp
│   ├── ConsoleUI.h/.cpp
│   └── DebugLog.h/.cpp
│
├── Documentation
│   ├── README_EN.md (English)
│   ├── QUICKSTART.md
│   ├── ARCHITECTURE.md
│   ├── API.md
│   ├── BUILD.md
│
└── Configuration
    ├── build.ps1
    ├── CMakeLists.txt
    └── offsets_example.cfg
```

---

## ⚠️ Requirements

- **OS**: Windows 10/11 x64
- **Compiler**: MSVC (Visual Studio 2019+)
- **Rights**: Administrator for some processes

---

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [QUICKSTART.md](QUICKSTART.md) | Get started in 60 seconds |
| [ARCHITECTURE.md](ARCHITECTURE.md) | System architecture |
| [API.md](API.md) | API reference |
| [BUILD.md](BUILD.md) | Build instructions |

---

## 📜 License

Educational and research tool for reverse engineering purposes.

---

**Process Module & Offset Management Tool v2.0**

🔍 **Happy Reversing!** 🚀

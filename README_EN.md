# Process Module & Offset Management Tool

Console application for Windows (x64) for managing offsets and process modules in reverse engineering context.

**[Russian Version / Русская версия](RUS/README.md)**

## 🎯 Purpose

- **ASLR Protection**: Store offsets in "module + offset" format instead of absolute addresses
- **Automatic Recalculation**: Automatically recalculate actual addresses on each launch
- **Module Export**: Save process module list with base addresses
- **Portability**: Configuration works after process/system restart

---

## 🏗️ Architecture

Application divided into 5 logical modules:

### 1. **ProcessManager**
- Process search by name
- PID retrieval
- Process handle opening
- Connection lifecycle management

### 2. **ModuleRegistry**
- Get list of loaded modules
- Store ImageBase and SizeOfImage for each module
- Fast module search by name (case-insensitive)

### 3. **AddressResolver**
- Absolute address recalculation: `resolved_address = module_base + offset`
- Automatic offset resolution from configuration
- Handle cases when module not found

### 4. **OffsetStorage**
- Load offsets from text file
- Save offsets (absolute addresses are NOT saved)
- Format: `module+offset=description`

### 5. **ConsoleUI**
- Navigation menu
- User interaction
- Table output and status

---

## 📝 Configuration Format

### Example file `offsets.cfg`:

```ini
# Offset Configuration File
# Format: ModuleName+0xOffset=Description
# Example: client.dll+0xDEA964=LocalPlayer

client.dll+0xDEA964=LocalPlayer
client.dll+0x4DCC098=EntityList
engine.dll+0x58EFC4=ViewAngles
engine.dll+0x590DD0=ClientState
server.dll+0x2F6A20=GlobalVars
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

### Scenario 1: First run (creating configuration)

```
1. Choose "Offset Manager"
2. Attach to process → enter process name (example.exe)
3. Program loads all modules
4. Add new offset:
   - Module: client.dll
   - Offset: 0xDEA964
   - Description: LocalPlayer
5. Add remaining offsets
6. Save offsets to file → offsets.cfg
```

### Scenario 2: Repeated run (loading configuration)

```
1. Offset Manager
2. Attach to process → example.exe
3. Load offsets from file → offsets.cfg
4. Resolve all offsets
5. View offsets — see actual addresses accounting for new ASLR
```

### Scenario 3: Export modules

```
1. Module Dumper
2. Enter process name
3. Module list displays on screen
4. Optionally save to file
```

---

## ✅ Key Features

### What it does:
- ✅ Stores offsets in readable format
- ✅ Automatically recalculates addresses on launch
- ✅ Works with ASLR without issues
- ✅ No external dependencies required
- ✅ Ready for extension (pattern scanner, memory reader)

### What it doesn't do:
- ❌ Doesn't save absolute addresses
- ❌ Doesn't read/write process memory (yet)
- ❌ Doesn't search patterns (can be added)

---

## 📊 Data Structures

### OffsetEntry
```cpp
struct OffsetEntry {
    std::wstring moduleName;     // client.dll
    uintptr_t offset;            // 0xDEA964
    std::wstring description;    // LocalPlayer
    
    // Runtime (not saved to file):
    uintptr_t resolvedAddress;   // 0x7FF6A2DEA964
    bool isResolved;             // true/false
};
```

### ModuleInfo
```cpp
struct ModuleInfo {
    std::wstring name;           // client.dll
    uintptr_t baseAddress;       // 0x7FF6A2000000 (ImageBase)
    uintptr_t size;              // 0x1A3C000 (SizeOfImage)
};
```

---

## 🔧 Extension Points

### Ready for extension:

1. **Pattern Scanner** — add in `ModuleRegistry::FindPattern()`
2. **Memory Reader** — add in `ProcessManager::ReadMemory()`
3. **Signature Updater** — auto-update offsets using signatures
4. **JSON format** — instead of INI for complex structures

See [API.md](API.md) for details.

---

## ⚠️ Requirements

- **OS**: Windows 10/11 x64
- **Rights**: Administrator (for some processes)
- **Compiler**: MSVC, MinGW, Clang (with C++11 support)
- **Dependencies**: WinAPI + standard library

---

## 📂 Project Structure

```
Process-Module-Dumper/
├── Source Files
│   ├── main.cpp
│   ├── ProcessManager.h/.cpp
│   ├── ModuleRegistry.h/.cpp
│   ├── AddressResolver.h/.cpp
│   ├── OffsetStorage.h/.cpp
│   └── ConsoleUI.h/.cpp
│
├── Documentation
│   ├── README_EN.md (this file)
│   ├── QUICKSTART.md
│   ├── ARCHITECTURE.md
│   ├── EXAMPLES.md
│   ├── API.md
│   ├── BUILD.md
│   ├── SUMMARY.md
│   ├── VISUAL_STRUCTURE.md
│   ├── CHANGELOG.md
│   ├── INDEX.md
│   └── RUS/ (Russian versions)
│
├── Configuration
│   ├── CMakeLists.txt
│   ├── build.ps1
│   ├── .gitignore
│   └── offsets_example.cfg
│
└── Legacy
    ├── base_address.cpp
    └── eng_base_address.cpp
```

---

## 🎓 Output Examples

### Module List:
```
====================================================
Module Name                         | Base Address     | Size
---------------------------------------------------------------------------
example.exe                         | 0x7FF7A1D00000   | 0x37E000
client.dll                          | 0x7FF6A2000000   | 0x1A3C000
engine.dll                          | 0x7FF6A5000000   | 0xB3D000
```

### Offsets Table:
```
====================================================
Module               | Offset       | Resolved Addr    | Description
-------------------------------------------------------------------------------------
client.dll           | 0xDEA964     | 0x7FF6A2DEA964   | LocalPlayer
engine.dll           | 0x58EFC4     | 0x7FF6A558EFC4   | ViewAngles
```

---

## 📚 Documentation

| File | Description |
|------|-------------|
| [QUICKSTART.md](QUICKSTART.md) | Quick start for 60 seconds |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Detailed architecture |
| [EXAMPLES.md](EXAMPLES.md) | Usage examples + FAQ |
| [API.md](API.md) | API reference |
| [BUILD.md](BUILD.md) | Build instructions |
| [SUMMARY.md](SUMMARY.md) | Project summary |
| [VISUAL_STRUCTURE.md](VISUAL_STRUCTURE.md) | Visual diagrams |
| [CHANGELOG.md](CHANGELOG.md) | Version history |
| [INDEX.md](INDEX.md) | Documentation index |

**Russian versions**: [RUS/](RUS/)

---

## 📜 License

Created for educational purposes in reverse engineering.  
Use responsibly and in accordance with the law.

---

## 👤 Author

Developed as a tool for analyzing and debugging application processes.

---

**Process Module & Offset Management Tool v1.0.0**

🔍 **Happy Reversing!** 🚀

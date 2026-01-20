# Process Module & Offset Management Tool

**[English](README_EN.md) | [Русский](RUS/README.md)**

Language Selection / Выбор языка:

| Version | Link | Description |
|---------|------|-------------|
| **English** | [README_EN.md](README_EN.md) | Full documentation in English |
| **Русский** | [RUS/README.md](RUS/README.md) | Полная документация на русском |

---

## 📖 Quick Documentation Links

### English
- [Full Overview](README_EN.md)
- [Quick Start - 60 seconds](QUICKSTART.md)
- [Architecture Details](ARCHITECTURE.md)
- [Usage Examples](EXAMPLES.md)
- [API Reference](API.md)
- [Build Guide](BUILD.md)

### Русский
- [Полный обзор](RUS/README.md)
- [Быстрый старт - 60 сек](RUS/QUICKSTART.md)
- [Архитектура](RUS/ARCHITECTURE.md)
- [Примеры использования](RUS/EXAMPLES.md)
- [Справочник API](RUS/API.md)
- [Руководство сборки](RUS/BUILD.md)

---

## 🎯 What is this?

Console application for Windows x64 that helps manage memory offsets safely across ASLR (Address Space Layout Randomization) boundaries. 

**Key Features:**
- Store offsets as `module + offset` instead of absolute addresses
- Automatic address recalculation on process restart
- Export process module lists with base addresses
- Cross-platform configuration that survives system reboots

---

## 💻 Technical Stack

| Component | Details |
|-----------|---------|
| Language | C++17 |
| Platform | Windows 10/11 x64 |
| API | WinAPI (TlHelp32) |
| Build | CMake + PowerShell |
| Config | INI format (text-based) |
| Architecture | 5 modular subsystems |

---

## 🏗️ Architecture Overview

```
[ConsoleUI] - Interactive Menu
    |
    +--[ProcessManager] - Process Discovery
    +--[ModuleRegistry] - Module Enumeration  
    +--[AddressResolver] - Address Calculation
    +--[OffsetStorage] - Config File I/O
    |
[WinAPI Layer] - Windows Process/Module Access
```

**5 Core Modules:**
1. **ProcessManager** - Find & attach to processes
2. **ModuleRegistry** - List loaded modules with ImageBase
3. **AddressResolver** - Calculate address = moduleBase + offset
4. **OffsetStorage** - Load/save INI configuration
5. **ConsoleUI** - Interactive user menu

---

## 📝 Configuration Format

```ini
# Format: ModuleName+0xHexOffset=Description

client.dll+0xDEA964=Pointer1
client.dll+0x4DCC098=Pointer2
engine.dll+0x58EFC4=Pointer3
```


---

## ⚡ Quick Usage

```
1. Compile: .\build.ps1
2. Run: ProcessModuleManager.exe
3. Choose: Offset Manager or Module Dumper
4. Attach: Enter process name
5. Manage: Add/Load/Resolve offsets
```

---

## 📊 Project Status

✅ Core Implementation Complete (11 source files, ~2000 LOC)  
✅ Compilation Successful (Exit Code 0)  
✅ Documentation Complete (English + Russian)  
✅ ASLR Protection Implemented  
✅ Build System Ready  

---

## 📂 File Structure

```
Process-Module-Dumper/
├── [Source Code]
│   ├── main.cpp
│   ├── ProcessManager.h/.cpp
│   ├── ModuleRegistry.h/.cpp
│   ├── AddressResolver.h/.cpp
│   ├── OffsetStorage.h/.cpp
│   └── ConsoleUI.h/.cpp
│
├── [Documentation - English]
│   ├── README_EN.md
│   ├── QUICKSTART.md
│   ├── ARCHITECTURE.md
│   ├── EXAMPLES.md
│   ├── API.md
│   ├── BUILD.md
│   ├── VISUAL_STRUCTURE.md
│   └── INDEX.md
│
├── [Documentation - Russian]
│   └── RUS/
│       ├── README.md
│       ├── QUICKSTART.md
│       ├── ARCHITECTURE.md
│       ├── EXAMPLES.md
│       ├── API.md
│       ├── BUILD.md
│       ├── VISUAL_STRUCTURE.md
│       └── INDEX.md
│
├── [Build]
│   ├── CMakeLists.txt
│   ├── build.ps1
│   └── .gitignore
│
└── [Other]
    ├── offsets_example.cfg
    ├── base_address.cpp (legacy)
    └── eng_base_address.cpp (legacy)
```

---

## ⚠️ System Requirements

- **OS**: Windows 10/11 (x64)
- **Compiler**: MSVC, MinGW, or Clang (C++11+)
- **Permissions**: Administrator rights (for process attachment)
- **Dependencies**: WinAPI + C++ Standard Library only

---

## 🔍 Use Cases

1. **Process Analysis** - Track pointers across application updates
2. **Security Research** - Map process memory layout
3. **Vulnerability Analysis** - Identify data structures
4. **Reverse Engineering** - Educational tool
5. **Debugging** - Foundation for custom debuggers

---

## 📜 License

Created for educational purposes in the field of reverse engineering.  
Use responsibly and in accordance with the law.

---

## 👤 Author

Developed as a tool for analyzing and debugging application processes.

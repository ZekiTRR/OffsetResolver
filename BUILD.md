# Build Instructions

**[English](BUILD.md) | [Русский](RUS/BUILD.md)**

## Method 1: PowerShell Script (Recommended)

```powershell
.\build.ps1
```

The script automatically:
- Finds Visual Studio installation (including custom paths)
- Compiles all source files
- Creates `ProcessModuleManager.exe`

---

## Method 2: Visual Studio Command Line

### Developer Command Prompt:

```cmd
cl /EHsc /std:c++17 /O2 /DUNICODE /D_UNICODE main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp PointerChainStorage.cpp PointerChainResolver.cpp MemoryReader.cpp DebugLog.cpp /Fe:ProcessModuleManager.exe
```

### Visual Studio IDE:

1. Create new "Console Application" project
2. Add all `.cpp` and `.h` files to project
3. In project settings:
   - Character Set: Unicode
   - C++ Language Standard: C++17 or higher
4. Build project (F7)

---

## Method 3: CMake

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Executable will be in `build/Release/ProcessModuleManager.exe`

---

## Method 4: MinGW (Windows)

```bash
g++ -std=c++17 -O2 -DUNICODE -D_UNICODE -o ProcessModuleManager.exe main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp PointerChainStorage.cpp PointerChainResolver.cpp MemoryReader.cpp DebugLog.cpp
```

---

## Method 5: Clang (Windows)

```bash
clang++ -std=c++17 -O2 -DUNICODE -D_UNICODE -o ProcessModuleManager.exe main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp PointerChainStorage.cpp PointerChainResolver.cpp MemoryReader.cpp DebugLog.cpp
```

---

## Source Files

| File | Description |
|------|-------------|
| main.cpp | Entry point |
| ProcessManager.cpp | Process attachment |
| ModuleRegistry.cpp | Module enumeration |
| AddressResolver.cpp | Address resolution |
| OffsetStorage.cpp | Offset file storage |
| ConsoleUI.cpp | User interface |
| PointerChainStorage.cpp | Pointer chain storage |
| PointerChainResolver.cpp | Multi-level pointer resolution |
| MemoryReader.cpp | Safe memory reading |
| DebugLog.cpp | Debug logging system |

---

## Requirements

- **Windows SDK**: For WinAPI access (tlhelp32.h, windows.h)
- **Compiler**: MSVC 2017+, MinGW-w64, Clang 10+
- **C++ Standard**: C++17 or higher
- **Platform**: Windows x64

---

## Verify Build

After successful build:

```cmd
ProcessModuleManager.exe
```

You should see the main application menu.

---

## Debug Build

### MSVC:
```cmd
cl /EHsc /std:c++17 /Zi /DUNICODE /D_UNICODE main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp PointerChainStorage.cpp PointerChainResolver.cpp MemoryReader.cpp DebugLog.cpp /Fe:ProcessModuleManager.exe
```

### GCC/Clang:
```bash
g++ -std=c++17 -g -DUNICODE -D_UNICODE -o ProcessModuleManager.exe *.cpp
```

---

## Administrator Rights

If the program cannot open some processes, run as administrator:

1. Right-click on `ProcessModuleManager.exe`
2. "Run as administrator"

Or through elevated command prompt:
```cmd
.\ProcessModuleManager.exe
```

---

## Troubleshooting

### "Cannot find Visual Studio"

Check custom paths in `build.ps1`:
```powershell
$customPaths = @(
    "E:\Microsft Visual Studio\Product",
    "D:\Visual Studio",
    # Add your path here
)
```

### "Undefined reference to..."

Ensure all `.cpp` files are included in compilation.

### "Module not found" at runtime

Run as administrator for protected processes.

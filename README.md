# [Русская версия](README_RU.md)
 
# Process Module & Memory Analysis Tool

C++ tool for memory analysis, module tracking, and pointer chain resolution in running processes.

## Features

- **Process Management**: Find and attach to processes by name
- **Module Registry**: List loaded modules and get their base addresses
- **Memory Reader**: Type-safe memory operations (int32, float, double)
- **Pointer Chains**: Resolve multi-level pointers with validation
- **Structures Export**: Save chains in a format suitable for analysis and debugging
- **Configuration Storage**: Save/load offset configs in module+offset format (ASLR-safe)
- **Console Interface**: Interactive menu for all operations

## Building

Prerequisites: Windows 10/11, CMake 3.20+, C++17 compiler

```powershell
.\build.ps1
```

Or manually:
```bash
mkdir build && cd build
cmake .. && cmake --build . --config Release
```

## Usage

1. Launch application → main menu
2. Attach to process → enter process name
3. View loaded modules → check base addresses
4. Create/load offset config file
5. Resolve pointer chains and read values

### Configuration Format

File \offsets.cfg\:
```ini
# Format: moduleName+0xOffset=Description
engine.dll+0x649910=BasePointer
engine.dll+0x4DCC098=DataPointer
utils.dll+0x58EFC4=HelperPointer
```

### Example: Multi-Level Pointer

Resolve pointer chain: \engine.dll+0x649910\ → offset 0x18 → offset 0x70 → read as INT

```ini
engine.dll+0x649910=ChainStart
# Offsets: 0x18 → 0x70 → final read
```

### Value Types
- **INT**: int32_t
- **FLOAT**: Single precision
- **DOUBLE**: Double precision

## Key Details

- **Module-Relative Addresses**: Stored as \module+offset\ for ASLR compatibility
- **Type-Safe**: Explicit data types prevent read errors
- **Validation**: All addresses and chains are validated before reading
- **ASLR-Safe**: Addresses survive process restart

## Troubleshooting

- **Process Not Found**: Check process is running, verify name spelling, may need admin rights
- **Module Not Loaded**: Refresh module list, check module name
- **Invalid Address**: Check pointer offsets, address may be in unmapped memory
- **Read Failures**: Check access rights, memory page protection, module unloading

## License

Educational and research purposes for reverse engineering and security analysis.

---

**Note**: Requires appropriate permissions. Use only for authorized security research and debugging.

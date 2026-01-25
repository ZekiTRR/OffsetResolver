# Usage Examples


## 📋 Contents

1. [Quick Start](#quick-start)
2. [Working with Offsets](#working-with-offsets)
3. [Pointer Chains](#pointer-chains)
4. [Debug Mode](#debug-mode)
5. [Module Export](#module-export)
6. [Tips and Tricks](#tips-and-tricks)

---

## Quick Start

### Example 1: First Run

```
> ProcessModuleManager.exe

====================================================
    Process Module & Offset Management Tool        
====================================================

Choose mode:
  1. Offset Manager (ASLR-safe offset storage)
  2. Pointer Chain Manager (multi-level pointers)
  3. Module Dumper (Export module list to file)
  0. Exit

  Commands: 'debug' - toggle debug | 'debugfile' - log to file

Select option [0-3]: 1

====================================================
              Offset Manager Mode                  
====================================================

[ ] Process: Not attached
[ ] Modules: Not loaded
[✓] Offsets: 0 in storage

Options:
  1. Attach to process
  2. Load offsets from file
  3. Add new offset
  4. Resolve all offsets
  5. View offsets and resolved addresses
  6. Save offsets to file
  7. View module list
  0. Back to main menu

Select option [0-7]: 1

Enter process name (e.g., example.exe): notepad.exe

[+] Successfully attached to process 'notepad.exe' (PID: 12345)
[+] Loaded 24 modules.
```

---

## Working with Offsets

### Example 2: Creating Offset Configuration

**Step 1: Attach to process**
```
Select option [0-7]: 1
Enter process name: example.exe
[+] Successfully attached to process 'example.exe' (PID: 23456)
[+] Loaded 156 modules.
```

**Step 2: View modules**
```
Select option [0-7]: 7

=== Module List ===
Module Name                         | Base Address     | Size
---------------------------------------------------------------------------
example.exe                         | 0x7FF7A1D00000   | 0x37E000
app.dll                             | 0x7FF6A2000000   | 0x1A3C000
module2.dll                         | 0x7FF6A5000000   | 0xB3D000
module3.dll                         | 0x7FF6A8000000   | 0x2F7000
...
```

**Step 3: Add offsets**
```
Select option [0-7]: 3

=== Add New Offset ===

Module name (e.g., app.dll): app.dll
[+] Module found. Base: 0x7FF6A2000000
Offset (hex, e.g., 0xDEA964): 0xDEA964
Description (optional): DataPointer

[+] Offset added successfully!
```

**Step 4: Save configuration**
```
Select option [0-7]: 6
Enter filename to save (e.g., offsets.cfg): my_offsets.cfg
[+] Saved 4 offsets to my_offsets.cfg
```

**Result (my_offsets.cfg)**:
```ini
# Offset Configuration File
# Format: ModuleName+0xOffset=Description

app.dll+0xDEA964=DataPointer
app.dll+0x4DCC098=EntityList
module2.dll+0x58EFC4=ViewAngles
module3.dll+0x590DD0=ClientState
```

---

### Example 3: Loading and Resolving Offsets

**After process restart (ASLR changed addresses)**

```
Select option [0-7]: 1
Enter process name: example.exe
[+] Successfully attached to process 'example.exe' (PID: 34567)  ← New PID!
[+] Loaded 156 modules.

Select option [0-7]: 2
Enter config filename: my_offsets.cfg
[+] Loaded 4 offsets from my_offsets.cfg

Select option [0-7]: 4

=== Resolving Offsets ===

[+] Successfully resolved 4/4 offsets.

Select option [0-7]: 5

=== Offset List ===
Module               | Offset       | Resolved Addr    | Description
-------------------------------------------------------------------------------------
app.dll              | 0xDEA964     | 0x7FF7A3DEA964   | DataPointer
app.dll              | 0x4DCC098    | 0x7FF7A7DCC098   | EntityList
module2.dll          | 0x58EFC4     | 0x7FF7A858EFC4   | ViewAngles
module3.dll          | 0x590DD0     | 0x7FF7A8590DD0   | ClientState
```

**Note**: 
- Offsets (`0xDEA964` etc.) — unchanged
- Resolved addresses — completely new (ASLR)
- Everything works automatically!

---

## Pointer Chains

### Example 4: Multi-Level Pointer Resolution

**Step 1: Enable debug mode**
```
Select option [0-3]: debug
[+] Debug mode ENABLED
```

**Step 2: Select Pointer Chain Manager**
```
Select option [0-3]: 2

====================================================
           Pointer Chain Manager Mode              
====================================================
```

**Step 3: Attach and add chain**
```
Module name: app.dll
Base offset (hex): 0x17E0A8
Offsets (comma-separated, e.g., 0x10,0x20,0x30): 0x18,0x70,0x2D0
Value type (int/float/double/int64/pointer): float
Description: PlayerPosition

[+] Pointer chain added!
```

**Step 4: Resolve chains**
```
=== Resolving Pointer Chains ===

[CHAIN] Resolving 'PlayerPosition': app.dll+0x17E0A8 -> [0x18,0x70,0x2D0]
[PTR]   Module base: 0x7FF6A2000000
[PTR]   Base address: 0x7FF6A217E0A8
[STEP]  Step 1/3: Read at 0x7FF6A217E0A8 = 0x22A14567890
[STEP]  Step 2/3: 0x22A14567890 + 0x70 = 0x22A14567900
[STEP]  Step 3/3: Read at 0x22A14567900 = 0x22A14568000
[MEM]   Final address: 0x22A145682D0
[MEM]   Value (float): 123.456

[+] Chain resolved successfully!
```

**Step 5: Save chains**
```
Enter filename: my_chains.txt
[+] Saved 1 chains to my_chains.txt
```

**Result (my_chains.txt)**:
```
# Pointer Chain Configuration
# Format: moduleName|baseOffset|offsets|valueType|description

app.dll|0x17E0A8|0x18,0x70,0x2D0|float|PlayerPosition
```

---

## Debug Mode

### Example 5: Debugging Pointer Chain Issues

**Enable debug mode from main menu:**
```
Select option [0-3]: debug
[+] Debug mode ENABLED

Select option [0-3]: debugfile
[+] File logging ENABLED (debug_log.txt)
```

**Debug output shows:**
- Module base addresses
- Each step of pointer resolution
- Memory values at each level
- Errors with detailed context

**Sample debug output:**
```
[INFO]  Starting chain resolution for 'Health'
[PTR]   Module 'app.dll' base: 0x7FF6A2000000
[ADDR]  Base address: 0x7FF6A217E0A8
[STEP]  Reading first pointer at base address
[PTR]   Read: 0x7FF6A217E0A8 -> 0x22A14567890
[STEP]  Step 1/2: Adding offset 0x18
[ADDR]  Current: 0x22A145678A8
[PTR]   Read: 0x22A145678A8 -> 0x22A14568000
[STEP]  Step 2/2: Adding offset 0xEC
[ADDR]  Final address: 0x22A145680EC
[MEM]   Reading final value as 'int'
[SUCCESS] Value: 100
```

**File log (debug_log.txt):**
```
[2024-01-15 14:30:45] [INFO] Starting chain resolution for 'Health'
[2024-01-15 14:30:45] [PTR] Module 'app.dll' base: 0x7FF6A2000000
...
```

---

## Module Export

### Example 6: Dump Process Modules

```
Select option [0-3]: 3

====================================================
              Module Dumper Mode                   
====================================================

Enter process name (e.g., example.exe): example.exe
[+] Successfully attached to process 'example.exe' (PID: 45678)
[+] Loaded 89 modules.

=== Module List ===
Module Name                         | Base Address     | Size
---------------------------------------------------------------------------
example.exe                         | 0x7FF6A1000000   | 0x2A4000
app.dll                             | 0x7FF6A3000000   | 0x1B5C000
module2.dll                         | 0x7FF6A5000000   | 0xC4D000
...

Save to file? (y/n): y
[+] Module list saved to example_modules_dump.txt
```

---

## Tips and Tricks

### Tip 1: Working with Multiple Processes

Create separate configurations:
```
app1_offsets.cfg
app1_chains.txt

app2_offsets.cfg
app2_chains.txt
```

---

### Tip 2: Comments in Configuration

```ini
# ========== Player Related ==========
app.dll+0xDEA964=DataPointer
app.dll+0x4DCC098=EntityList

# ========== View/Camera ==========
module2.dll+0x58EFC4=ViewAngles
module2.dll+0x590DD0=ClientState
```

---

### Tip 3: Checking Offset Validity

After application update:
```
1. Attach to process
2. Load old offsets
3. Resolve all
4. View offsets

If all resolved successfully — offsets are still valid!
If some failed — module not found, check name
```

---

### Tip 4: Configuration Backup

```powershell
# PowerShell backup script
$date = Get-Date -Format "yyyy-MM-dd"
Copy-Item "offsets.cfg" "backups/offsets_$date.cfg"
Copy-Item "chains.txt" "backups/chains_$date.txt"
```

---

### Tip 5: Using Debug for Troubleshooting

When pointer chain fails:
1. Enable `debug` mode
2. Enable `debugfile` for persistent log
3. Try resolving chain
4. Check `debug_log.txt` for exact failure point

Common issues:
- Invalid base address → wrong module name
- Step N fails → pointer at previous level is invalid
- Final read fails → incorrect value type

---

## FAQ

### Q: Why can't the app find the process?

**A**: Check:
1. Process name is correct (with `.exe` extension)
2. Process is actually running (Task Manager)
3. Case doesn't matter: `EXAMPLE.exe` = `example.exe`

---

### Q: "Failed to get module list. Try running as administrator"

**A**: Some processes require administrator rights.

**Solution**:
1. Right-click on `ProcessModuleManager.exe`
2. "Run as administrator"

---

### Q: Pointer chain fails at step 1

**A**: Common causes:
1. Base offset is wrong
2. Module base changed (try re-attaching)
3. Pointer is NULL at that address

Use debug mode to see exact values.

---

### Q: Can I use for 32-bit processes?

**A**: Theoretically yes, but app is compiled as x64.

**Solution**: Rebuild as x86 application.

---

### Q: Where are configurations stored?

**A**: In the same folder as `ProcessModuleManager.exe`

You can specify absolute path:
```
Enter filename: C:\MyConfigs\offsets.cfg
```

---

## Conclusion

This application is a foundation for reverse engineering tools.

**Next steps**:
1. Study code in `ARCHITECTURE.md`
2. Extend functionality (pattern scanner, memory writer)
3. Integrate into your projects

**Happy Reversing! 🔍**

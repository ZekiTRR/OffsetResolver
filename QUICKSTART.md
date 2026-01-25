# Quick Start Guide - 60 Seconds


Get up and running in 60 seconds!

---

## 🚀 Step 1: Build (15 seconds)

```powershell
cd Process-Module-Dumper
.\build.ps1
```

Or with manual compilation:
```bash
cl /EHsc /std:c++17 /O2 /DUNICODE /D_UNICODE *.cpp /Fe:ProcessModuleManager.exe
```

---

## 📝 Step 2: Run (5 seconds)

```
ProcessModuleManager.exe
```

---

## 🎯 Step 3: Choose Mode (10 seconds)

```
====================================================
    Process Module & Offset Management Tool        
====================================================

Choose mode:
  1. Offset Manager (ASLR-safe offset storage)
  2. Pointer Chain Manager (multi-level pointers)
  3. Module Dumper (Export module list)
  0. Exit

  Commands: 'debug' - toggle debug | 'debugfile' - toggle file log
```

---

## ⚙️ Option A: Offset Manager (15 seconds)

```
1. Attach to process: notepad.exe
2. Add offset:
   - Module: ntdll.dll
   - Offset: 0x10000
   - Description: MyPointer
3. Resolve offsets → see actual addresses
4. Save to file
```

---

## ⚙️ Option B: Pointer Chain Manager (15 seconds)

```
1. Attach to process: example.exe
2. Add chain:
   - Module: app.dll
   - Base offset: 0x17E0A8
   - Offsets: 0xEC
   - Type: int
   - Description: Health
3. Resolve chains → see values
```

---

## 🔧 Debug Mode

Type in main menu:
- `debug` - Enable detailed logging
- `debugfile` - Enable file logging (debug_log.txt)

---

## 📁 Configuration Files

### offsets.cfg
```ini
# ModuleName+0xOffset=Description
ntdll.dll+0x10000=MyPointer
kernel32.dll+0x20000=AnotherPointer
```

### chains.txt
```
# moduleName|baseOffset|offsets|valueType|description
app.dll|0x17E0A8|0xEC|int|Health
app.dll|0x17E0A8|0x18,0x70,0x2D0|float|Position
```

---

## ✅ Done!

You now have:
- ✅ Working offset storage
- ✅ ASLR protection
- ✅ Pointer chain resolution
- ✅ Debug logging

---

## 📚 Next Steps

- [Full Documentation](README_EN.md)
- [Architecture Details](ARCHITECTURE.md)
- [API Reference](API.md)

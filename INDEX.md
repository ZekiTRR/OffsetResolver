# Documentation Index 🗺️


> **Main documentation navigation**

---

## 🚀 Quick Access

### Getting Started
- **[⚡ QUICKSTART.md](QUICKSTART.md)** — Start here! Get running in 60 seconds
- **[📘 README_EN.md](README_EN.md)** — Project overview and capabilities
- **[📙 EXAMPLES.md](EXAMPLES.md)** — Practical examples and FAQ

### For Developers
- **[📗 ARCHITECTURE.md](ARCHITECTURE.md)** — Detailed system architecture
- **[📕 API.md](API.md)** — API reference for extending functionality
- **[🔧 BUILD.md](BUILD.md)** — Build instructions

### General Information
- **[📖 SUMMARY.md](SUMMARY.md)** — Project summary
- **[📊 VISUAL_STRUCTURE.md](VISUAL_STRUCTURE.md)** — Visual structure and diagrams
- **[📔 CHANGELOG.md](CHANGELOG.md)** — Version history and plans

---

## 📚 Detailed Descriptions

### ⚡ QUICKSTART.md
**For**: Everyone who wants to start immediately  
**Contains**:
- Build and run in 60 seconds
- Quick functionality test
- Basic commands and debug mode

**Reading time**: 3-5 minutes

---

### 📘 README_EN.md
**For**: First introduction to the project  
**Contains**:
- Application purpose
- Key capabilities
- Architecture overview
- Configuration format
- Usage scenarios
- Requirements and build

**Reading time**: 10-15 minutes

---

### 📙 EXAMPLES.md
**For**: Users wanting practical examples  
**Contains**:
- Step-by-step workflow examples
- Creating and using configurations
- Pointer chain examples
- Debug mode usage
- Tips and tricks
- FAQ

**Reading time**: 20-30 minutes

---

### 📗 ARCHITECTURE.md
**For**: Developers studying internal structure  
**Contains**:
- Detailed architecture of each module
- Interaction diagrams
- Data structures
- Working algorithms
- ASLR protection mechanism
- Debug system overview
- Extension points

**Reading time**: 30-45 minutes

---

### 📕 API.md
**For**: Developers extending functionality  
**Contains**:
- API reference for all classes
- Method descriptions and parameters
- Debug macros and logging
- API usage examples
- Best practices
- Thread safety

**Reading time**: 25-35 minutes

---

### 🔧 BUILD.md
**For**: Those building the project  
**Contains**:
- Visual Studio build
- CMake build
- MinGW build
- Clang build
- Debug build options
- Troubleshooting

**Reading time**: 10-15 minutes

---

## 🗺️ Reading Order

### New Users
1. **QUICKSTART.md** → Get running fast
2. **README_EN.md** → Understand capabilities
3. **EXAMPLES.md** → Learn by examples

### Developers
1. **ARCHITECTURE.md** → Understand structure
2. **API.md** → Learn APIs
3. **BUILD.md** → Set up development

### Troubleshooting
1. **EXAMPLES.md → FAQ section**
2. **Debug mode** → `debug` command
3. **Log files** → `debugfile` command

---

## 📁 File Structure

```
Process-Module-Dumper/
├── 📘 README_EN.md          # English readme
├── ⚡ QUICKSTART.md         # Quick start guide
├── 📗 ARCHITECTURE.md       # Architecture docs
├── 📕 API.md                # API reference
├── 📙 EXAMPLES.md           # Usage examples
├── 🔧 BUILD.md              # Build instructions
├── 📖 SUMMARY.md            # Project summary
├── 📊 VISUAL_STRUCTURE.md   # Visual diagrams
├── 📔 CHANGELOG.md          # Version history
├── 🗺️ INDEX.md              # This file
│
│
├── 💻 Source Files
│   ├── main.cpp
│   ├── ProcessManager.cpp/h
│   ├── ModuleRegistry.cpp/h
│   ├── AddressResolver.cpp/h
│   ├── OffsetStorage.cpp/h
│   ├── ConsoleUI.cpp/h
│   ├── MemoryReader.cpp/h
│   ├── PointerChainResolver.cpp/h
│   ├── PointerChainStorage.cpp/h
│   └── DebugLog.cpp/h
│
└── 🔧 Build Files
    ├── CMakeLists.txt
    ├── build.ps1
    └── offsets_example.cfg
```

---

## 🔍 Quick Find

| Topic | Document | Section |
|-------|----------|---------|
| Build errors | BUILD.md | Troubleshooting |
| Offset format | README_EN.md | Configuration |
| Pointer chains | EXAMPLES.md | Pointer Chains |
| Debug mode | EXAMPLES.md | Debug Mode |
| API methods | API.md | All sections |
| ASLR explanation | ARCHITECTURE.md | ASLR Protection |
| Module structure | ARCHITECTURE.md | Modules |

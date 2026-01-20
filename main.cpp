// ============================================================================
// Process Module & Offset Management Tool
// Tool for managing offsets and pointer chains for process modules (Windows x64)
//
// ARCHITECTURE:
// - ProcessManager    : Process search and attachment
// - ModuleRegistry    : Module information storage (ImageBase)
// - AddressResolver   : Absolute address resolution with ASLR support
// - OffsetStorage     : Load/save offsets (module+offset format)
// - MemoryReader      : Type-safe memory reading with validation
// - PointerChainResolver : Multi-level pointer chain resolution
// - PointerChainStorage  : JSON persistence for pointer chains
// - ConsoleUI         : User interface
//
// PURPOSE:
// 1. Store offsets as "module + offset" for ASLR protection
// 2. Automatically recalculate absolute addresses on each launch
// 3. Resolve multi-level pointer chains with memory value reading
// 4. Export process module list
//
// USAGE:
// - Run the program
// - Choose mode (Offset Manager, Pointer Chain Manager, or Module Dumper)
// - Follow console instructions
//
// OFFSET FILE FORMAT:
// client.dll+0xDEA964=LocalPlayer
// engine.dll+0x58EFC4=ViewAngles
//
// POINTER CHAIN JSON FORMAT:
// {
//   "pointer_chains": [
//     {
//       "module": "app.dll",
//       "baseOffset": "0x1000",
//       "offsets": ["0x18", "0x70", "0x2D0"],
//       "valueType": "int",
//       "description": "Health"
//     }
//   ]
// }
//
// AUTHOR: Created for reverse engineering purposes
// ============================================================================

#include "ProcessManager.h"
#include "ModuleRegistry.h"
#include "AddressResolver.h"
#include "OffsetStorage.h"
#include "MemoryReader.h"
#include "PointerChainResolver.h"
#include "PointerChainStorage.h"
#include "ConsoleUI.h"
#include <iostream>
#include <windows.h>

int main()
{
    // Set console code page for Unicode
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Create subsystems
    ProcessManager processManager;
    ModuleRegistry moduleRegistry;
    AddressResolver addressResolver;
    OffsetStorage offsetStorage;
    MemoryReader memoryReader(processManager.GetHandle());
    PointerChainResolver pointerChainResolver(&moduleRegistry, &memoryReader);
    PointerChainStorage pointerChainStorage;

    // Initialize UI with all dependencies
    ConsoleUI ui(processManager, moduleRegistry, addressResolver, offsetStorage,
                 memoryReader, pointerChainResolver, pointerChainStorage);

    // Launch main menu
    ui.ShowMainMenu();

    std::wcout << L"\nThank you for using Process Module & Offset Management Tool!\n";

    return 0;
}

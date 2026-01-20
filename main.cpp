// ============================================================================
// Process Module & Offset Management Tool
// Tool for managing offsets and process modules (Windows x64)
//
// ARCHITECTURE:
// - ProcessManager    : Process search and attachment
// - ModuleRegistry    : Module information storage (ImageBase)
// - AddressResolver   : Absolute address resolution with ASLR support
// - OffsetStorage     : Load/save offsets (module+offset format)
// - ConsoleUI         : User interface
//
// PURPOSE:
// 1. Store offsets as "module + offset" for ASLR protection
// 2. Automatically recalculate absolute addresses on each launch
// 3. Export process module list
//
// USAGE:
// - Run the program
// - Choose mode (Offset Manager or Module Dumper)
// - Follow console instructions
//
// OFFSET FILE FORMAT:
// client.dll+0xDEA964=LocalPlayer
// engine.dll+0x58EFC4=ViewAngles
//
// AUTHOR: Created for reverse engineering purposes
// ============================================================================

#include "ProcessManager.h"
#include "ModuleRegistry.h"
#include "AddressResolver.h"
#include "OffsetStorage.h"
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

    // Initialize UI with dependencies
    ConsoleUI ui(processManager, moduleRegistry, addressResolver, offsetStorage);

    // Launch main menu
    ui.ShowMainMenu();

    std::wcout << L"\nThank you for using Process Module & Offset Management Tool!\n";

    return 0;
}

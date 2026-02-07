#include "ProcessManager.h"
#include "ModuleRegistry.h"
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
    MemoryReader memoryReader(processManager.GetHandle());
    PointerChainResolver pointerChainResolver(&moduleRegistry, &memoryReader);
    PointerChainStorage pointerChainStorage;

    // Initialize UI with all dependencies
    ConsoleUI ui(processManager, moduleRegistry,
                 memoryReader, pointerChainResolver, pointerChainStorage);

    // Launch main menu
    ui.ShowMainMenu();

    std::wcout << L"\nThank you for using Process Module & Pointer Chain Management Tool!\n";

    return 0;
}

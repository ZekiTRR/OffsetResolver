# Инструкции по сборке

**[English](../BUILD.md) | [Русский](BUILD.md)**

## Метод 1: PowerShell скрипт (рекомендуется)

```powershell
.\build.ps1
```

Скрипт автоматически:
- Находит установку Visual Studio (включая нестандартные пути)
- Компилирует все исходные файлы
- Создаёт `ProcessModuleManager.exe`

---

## Метод 2: Командная строка Visual Studio

### Developer Command Prompt:

```cmd
cl /EHsc /std:c++17 /O2 /DUNICODE /D_UNICODE main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp PointerChainStorage.cpp PointerChainResolver.cpp MemoryReader.cpp DebugLog.cpp /Fe:ProcessModuleManager.exe
```

### Visual Studio IDE:

1. Создайте новый проект "Console Application"
2. Добавьте все `.cpp` и `.h` файлы в проект
3. В настройках проекта установите:
   - Character Set: Unicode
   - C++ Language Standard: C++17 или выше
4. Соберите проект (F7)

---

## Метод 3: CMake

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Исполняемый файл будет в `build/Release/ProcessModuleManager.exe`

---

## Метод 4: MinGW (Windows)

```bash
g++ -std=c++17 -O2 -DUNICODE -D_UNICODE -o ProcessModuleManager.exe main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp PointerChainStorage.cpp PointerChainResolver.cpp MemoryReader.cpp DebugLog.cpp
```

---

## Исходные файлы

| Файл | Описание |
|------|----------|
| main.cpp | Точка входа |
| ProcessManager.cpp | Подключение к процессу |
| ModuleRegistry.cpp | Перечисление модулей |
| AddressResolver.cpp | Разрешение адресов |
| OffsetStorage.cpp | Хранение оффсетов |
| ConsoleUI.cpp | Пользовательский интерфейс |
| PointerChainStorage.cpp | Хранение цепочек указателей |
| PointerChainResolver.cpp | Разрешение многоуровневых указателей |
| MemoryReader.cpp | Безопасное чтение памяти |
| DebugLog.cpp | Система отладки |

---

## Требования

- **Windows SDK**: Для доступа к WinAPI (tlhelp32.h, windows.h)
- **Компилятор**: MSVC 2017+, MinGW-w64, Clang 10+
- **C++ Standard**: C++17 или выше
- **Платформа**: Windows x64

---

## Проверка сборки

После успешной сборки:

```cmd
ProcessModuleManager.exe
```

Вы должны увидеть главное меню приложения.

---

## Debug сборка

### MSVC:
```cmd
cl /EHsc /std:c++17 /Zi /DUNICODE /D_UNICODE main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp PointerChainStorage.cpp PointerChainResolver.cpp MemoryReader.cpp DebugLog.cpp /Fe:ProcessModuleManager.exe
```

### GCC/Clang:
```bash
g++ -std=c++17 -g -DUNICODE -D_UNICODE -o ProcessModuleManager.exe *.cpp
```

---

## Права администратора

Если программа не может открыть некоторые процессы, запустите её от администратора:

1. Правый клик на `ProcessModuleManager.exe`
2. "Запуск от имени администратора"

---

## Устранение неполадок

### "Cannot find Visual Studio"

Проверьте пользовательские пути в `build.ps1`:
```powershell
$customPaths = @(
    "E:\Microsft Visual Studio\Product",
    "D:\Visual Studio",
    # Добавьте свой путь здесь
)
```

### "Undefined reference to..."

Убедитесь, что все `.cpp` файлы включены в компиляцию.

### "Module not found" во время выполнения

Запустите от администратора для защищённых процессов.

# Инструкции по сборке

## Метод 1: Visual Studio (рекомендуется)

### Через командную строку разработчика:

```cmd
cl /EHsc /std:c++17 /DUNICODE /D_UNICODE main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp /Fe:ProcessModuleManager.exe
```

### Через Visual Studio IDE:

1. Создайте новый проект "Console Application"
2. Добавьте все `.cpp` и `.h` файлы в проект
3. В настройках проекта установите:
   - Character Set: Unicode
   - C++ Language Standard: C++17 или выше
4. Соберите проект (F7)

---

## Метод 2: CMake

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Исполняемый файл будет в `build/Release/ProcessModuleManager.exe`

---

## Метод 3: MinGW (Windows)

```bash
g++ -std=c++17 -DUNICODE -D_UNICODE -o ProcessModuleManager.exe main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp
```

---

## Метод 4: Clang (Windows)

```bash
clang++ -std=c++17 -DUNICODE -D_UNICODE -o ProcessModuleManager.exe main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp
```

---

## Быстрая сборка (PowerShell скрипт)

Создайте файл `build.ps1`:

```powershell
# build.ps1
Write-Host "Building Process Module Manager..." -ForegroundColor Green

$sources = @(
    "main.cpp",
    "ProcessManager.cpp",
    "ModuleRegistry.cpp",
    "AddressResolver.cpp",
    "OffsetStorage.cpp",
    "ConsoleUI.cpp"
)

$output = "ProcessModuleManager.exe"

# Поиск компилятора Visual Studio
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

if (Test-Path $vsWhere) {
    $vsPath = & $vsWhere -latest -property installationPath
    $vcvars = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
    
    if (Test-Path $vcvars) {
        Write-Host "Found Visual Studio at: $vsPath" -ForegroundColor Cyan
        
        # Компиляция
        cmd /c "`"$vcvars`" && cl /EHsc /std:c++17 /DUNICODE /D_UNICODE $($sources -join ' ') /Fe:$output"
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "`nBuild successful! Output: $output" -ForegroundColor Green
        } else {
            Write-Host "`nBuild failed!" -ForegroundColor Red
        }
    }
} else {
    Write-Host "Visual Studio not found. Please install Visual Studio with C++ tools." -ForegroundColor Red
}
```

Запуск:
```powershell
.\build.ps1
```

---

## Требования

- **Windows SDK**: Для доступа к WinAPI (tlhelp32.h, windows.h)
- **Компилятор**: MSVC 2017+, MinGW-w64, Clang 10+
- **C++ Standard**: C++17 или выше

---

## Проверка сборки

После успешной сборки запустите:

```cmd
ProcessModuleManager.exe
```

Вы должны увидеть главное меню приложения.

---

## Отладка

Для сборки с отладочной информацией:

### MSVC:
```cmd
cl /EHsc /std:c++17 /Zi /DUNICODE /D_UNICODE main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp /Fe:ProcessModuleManager.exe
```

### GCC/Clang:
```bash
g++ -std=c++17 -g -DUNICODE -D_UNICODE -o ProcessModuleManager.exe main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp
```

---

## Права администратора

Если программа не может открыть некоторые процессы, запустите её от имени администратора:

1. Правый клик на `ProcessModuleManager.exe`
2. "Запуск от имени администратора"

Или через командную строку с правами администратора:
```cmd
.\ProcessModuleManager.exe
```

# Краткое руководство - 60 Секунд

**[English](../QUICKSTART.md) | [Русский](QUICKSTART.md)**

Начните работу за 60 секунд!

---

## Шаг 1: Сборка (30 секунд)

### Используя PowerShell (Рекомендуется)
```powershell
cd e:\Reverse\CSS\Cheat\Process-Module-Dumper
.\build.ps1
```

### Using Command Line
```bash
cd e:\Reverse\CSS\Cheat\Process-Module-Dumper
cl /EHsc /std:c++17 /O2 /DUNICODE /D_UNICODE ^
    main.cpp ProcessManager.cpp ModuleRegistry.cpp ^
    AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp
```

### Using CMake
```bash
cmake -B build
cmake --build build --config Release
```

---

## 📝 Step 2: Run & Configure (20 seconds)

### Execute
```
ProcessModuleManager.exe
```

### Choose Mode
```
Main Menu:
1. Offset Manager     (manage offsets with ASLR protection)
2. Module Dumper      (export module list)
3. Exit
```

### Attach to Process
```
Enter process name: notepad.exe
[Process found with PID: 1234]
```

---

## ⚙️ Step 3: Use Offset Manager (10 seconds)

```
Offset Manager Menu:
1. Add new offset
2. Load from file
3. View offsets
4. Resolve offsets
5. Save to file
6. Back to main menu
```

### Add Your First Offset
```
Module name: kernel32.dll
Offset: 0x10000
Description: MyAddress

[Offset added: kernel32.dll+0x10000]
```

### Resolve Offsets
```
[Resolving offsets...]
kernel32.dll -> BaseAddress: 0x7FF8A4D00000
Resolved: 0x7FF8A4D10000 = 0x7FF8A4D00000 + 0x10000
```

---

## 📁 Configuration File Format

Create `offsets.cfg`:

```ini
# My Offsets Configuration
# Format: ModuleName+0xOffset=Description

kernel32.dll+0x10000=MyFirstAddress
kernel32.dll+0x20000=MySecondAddress
ntdll.dll+0x5000=SystemFunction
```

Load in application → View resolved addresses → Done!

---

## 🎯 Common Tasks

### Task 1: Export Module List
1. Run application
2. Choose "Module Dumper"
3. Enter process name
4. View list of modules with base addresses
5. Save to file (optional)

### Task 2: Create Offset Configuration
1. Choose "Offset Manager"
2. Attach to process
3. Add offsets manually OR load from file
4. Resolve all offsets
5. Save configuration

### Task 3: Reuse Configuration
1. Run application next time
2. Attach to same process
3. Load previous configuration
4. Offsets automatically recalculated for new ASLR
5. View updated addresses

---

## 📊 Output Examples

### Module List
```
Module List from process 'csgo.exe':

Module Name                    | Base Address     | Size
======================================================================
csgo.exe                       | 0x7FF7A1D00000   | 0x37E000
client.dll                     | 0x7FF6A2000000   | 0x1A3C000
engine.dll                     | 0x7FF6A5000000   | 0xB3D000
server.dll                     | 0x7FF6AA000000   | 0x2C5000
```

### Resolved Offsets
```
Resolved Offsets:

Module          | Offset      | Resolved Address | Description
=====================================================================
client.dll      | 0xDEA964    | 0x7FF6A2DEA964   | LocalPlayer
engine.dll      | 0x58EFC4    | 0x7FF6A558EFC4   | ViewAngles
kernel32.dll    | 0x10000     | 0x7FF8A4D10000   | MyAddress
```

---

## ✅ Troubleshooting

### "Process not found"
- ✓ Check process name spelling
- ✓ Run as Administrator
- ✓ Ensure process is actually running

### "Failed to open process"
- ✓ Run application as Administrator
- ✓ 64-bit application can only attach to 64-bit processes
- ✓ Check Windows permissions

### "Module not found when resolving"
- ✓ Module may have been unloaded
- ✓ Check module name spelling (case-sensitive)
- ✓ Process may have restarted

### "Address resolution failed"
- ✓ Configuration file may be corrupted
- ✓ Delete and recreate configuration
- ✓ Check offset format: `ModuleName+0xOffset`

---

## 🔍 Next Steps

- Read full [README_EN.md](README_EN.md) for detailed overview
- Study [ARCHITECTURE.md](ARCHITECTURE.md) for system design
- See [EXAMPLES.md](EXAMPLES.md) for real-world scenarios
- Check [API.md](API.md) for class reference
- View [BUILD.md](BUILD.md) for build system details

---

## 💡 Pro Tips

1. **ASLR Protection**: Always use module+offset format, never absolute addresses
2. **Configuration Backup**: Save offsets.cfg before updating your code
3. **Batch Operations**: Load multiple offsets at once for efficiency
4. **Export First**: Use Module Dumper to understand the process structure
5. **Module Names**: Use exact names from module list (case-sensitive)

---

**You're ready! Start using Process Module Manager.** 🎉

Questions? See [EXAMPLES.md](EXAMPLES.md) for common scenarios.
```
Выберите: 2 (Module Dumper)
Процесс: notepad.exe
Сохранить: y

✓ Готово! Смотрите notepad.exe_modules_dump.txt
```

---

## 📋 Быстрый старт с Offset Manager

### Шаг 1: Запуск и подключение
```
> ProcessModuleManager.exe

Выберите: 1 (Offset Manager)
Выберите: 1 (Attach to process)
Процесс: notepad.exe
```

### Шаг 2: Добавление оффсета
```
Выберите: 3 (Add new offset)

Модуль: ntdll.dll
Оффсет: 0x1000
Описание: TestOffset
```

### Шаг 3: Разрешение адреса
```
Выберите: 4 (Resolve all offsets)
Показать? y

Увидите: ntdll.dll | 0x1000 | 0x7FF... | TestOffset
```

### Шаг 4: Сохранение
```
Выберите: 6 (Save offsets)
Файл: test.cfg

✓ Сохранено!
```

---

## 🎮 Реальный пример (CS:GO)

### Создание конфигурации

```
1. Запустите CS:GO
2. Запустите ProcessModuleManager.exe
3. Выберите Offset Manager
4. Attach to process → csgo.exe
5. Добавьте оффсеты:

   client.dll + 0xDEA964  = LocalPlayer
   client.dll + 0x4DCC098 = EntityList
   engine.dll + 0x58EFC4  = ViewAngles

6. Save → csgo_offsets.cfg
```

### Использование конфигурации

```
1. Перезапустите CS:GO (ASLR изменит адреса!)
2. Запустите ProcessModuleManager.exe
3. Offset Manager → Attach → csgo.exe
4. Load offsets → csgo_offsets.cfg
5. Resolve all offsets
6. View offsets → видите актуальные адреса!
```

---

## 🔧 Решение проблем

### "Process not found"
✅ Проверьте имя: `notepad.exe` (не просто `notepad`)  
✅ Процесс действительно запущен?

### "Failed to get module list"
✅ Запустите от **администратора**  
✅ ПКМ → Запуск от имени администратора

### "Module 'xxx' not found"
✅ Проверьте имя модуля в списке (View modules)  
✅ Регистр не важен: `CLIENT.DLL` = `client.dll`

---

## 📖 Что дальше?

### Хотите узнать больше?
- 📘 [README.md](README.md) — полное описание
- 📙 [EXAMPLES.md](EXAMPLES.md) — больше примеров

### Хотите расширить?
- 📕 [API.md](API.md) — добавляйте свои функции
- 📗 [ARCHITECTURE.md](ARCHITECTURE.md) — как всё устроено

### Хотите помочь проекту?
- 🐛 Нашли баг? → Создайте Issue
- 💡 Есть идея? → Feature Request
- 🔨 Хотите код? → Pull Request

---

## 🎯 Основные команды

### Module Dumper Mode
```
1 → Выбор режима
Имя процесса → Ввод
y/n → Сохранить в файл
```

### Offset Manager Mode
```
1 → Attach to process
2 → Load offsets from file
3 → Add new offset
4 → Resolve all offsets
5 → View offsets
6 → Save offsets
7 → View modules
0 → Back
```

---

## 💾 Формат конфигурации

### Простой пример (test.cfg)
```ini
ntdll.dll+0x1000=Test1
kernel32.dll+0x2000=Test2
```

### Продвинутый пример (game.cfg)
```ini
# Player data
client.dll+0xDEA964=LocalPlayer
client.dll+0x4DCC098=EntityList

# View system  
engine.dll+0x58EFC4=ViewAngles
engine.dll+0x590DD0=ClientState
```

---

## ⚙️ Сборка (разные способы)

### PowerShell (рекомендуется)
```powershell
.\build.ps1
```

### CMake
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Visual Studio (вручную)
```cmd
cl /EHsc /std:c++17 /DUNICODE /D_UNICODE main.cpp ProcessManager.cpp ModuleRegistry.cpp AddressResolver.cpp OffsetStorage.cpp ConsoleUI.cpp
```

---

## 📊 Структура файлов

```
📁 Process-Module-Dumper/
  ├── 📄 main.cpp                    ← Точка входа
  ├── 📄 ProcessManager.h/.cpp       ← Управление процессами
  ├── 📄 ModuleRegistry.h/.cpp       ← Работа с модулями
  ├── 📄 AddressResolver.h/.cpp      ← Разрешение адресов
  ├── 📄 OffsetStorage.h/.cpp        ← Хранение оффсетов
  ├── 📄 ConsoleUI.h/.cpp            ← Интерфейс
  ├── 📄 CMakeLists.txt              ← CMake
  ├── 📄 build.ps1                   ← Скрипт сборки
  └── 📚 Документация/
      ├── README.md
      ├── EXAMPLES.md
      ├── API.md
      └── ...
```

---

## 🎓 Обучающие ссылки

| Что изучить | Где найти |
|-------------|-----------|
| Основы | [README.md](README.md) |
| Примеры | [EXAMPLES.md](EXAMPLES.md) |
| Архитектура | [ARCHITECTURE.md](ARCHITECTURE.md) |
| API | [API.md](API.md) |
| Сборка | [BUILD.md](BUILD.md) |

---

## ⚠️ Важно!

✅ **Используйте легально** — только для тестирования и research  
✅ **Права администратора** — для некоторых процессов  
✅ **Не работает с античитами** — EAC, BattlEye, Vanguard  
✅ **Backup конфигураций** — делайте копии важных .cfg

---

## 🆘 Помощь

**Вопросы?** → Читайте [EXAMPLES.md](EXAMPLES.md) (FAQ раздел)  
**Баги?** → Создайте Issue с описанием  
**Идеи?** → Feature Request в репозитории

---

## 🏆 Первые шаги (чеклист)

- [ ] Собрал приложение
- [ ] Запустил и увидел меню
- [ ] Протестировал Module Dumper на notepad.exe
- [ ] Попробовал Offset Manager
- [ ] Создал свою конфигурацию
- [ ] Прочитал README.md
- [ ] Изучил примеры в EXAMPLES.md
- [ ] Готов к работе! 🎉

---

**Готово! Теперь вы знаете основы. Удачи в реверс-инжиниринге! 🔍🚀**

> Для подробностей смотрите полную документацию в [README.md](README.md)

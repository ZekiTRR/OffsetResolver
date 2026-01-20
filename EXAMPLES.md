# Примеры использования

## 📋 Содержание

1. [Быстрый старт](#быстрый-старт)
2. [Работа с оффсетами](#работа-с-оффсетами)
3. [Экспорт модулей](#экспорт-модулей)
4. [Реальные сценарии](#реальные-сценарии)
5. [Советы и трюки](#советы-и-трюки)

---

## Быстрый старт

### Пример 1: Первый запуск

```
> ProcessModuleManager.exe

╔═══════════════════════════════════════════════════╗
║     Process Module & Offset Management Tool      ║
╚═══════════════════════════════════════════════════╝

Choose mode:
  1. Offset Manager (ASLR-safe offset storage)
  2. Module Dumper (Export module list to file)
  0. Exit

Select option [0-2]: 1

╔═══════════════════════════════════════════════════╗
║              Offset Manager Mode                  ║
╚═══════════════════════════════════════════════════╝

[ ] Process: Not attached
[ ] Modules: Not loaded
[✓] Offsets: 0 in storage

Options:
  1. Attach to process
  2. Load offsets from file
  3. Add new offset
  4. Resolve all offsets (calculate addresses)
  5. View offsets and resolved addresses
  6. Save offsets to file
  7. View module list
  0. Back to main menu

Select option [0-7]: 1

Enter process name (e.g., csgo.exe): notepad.exe

[+] Successfully attached to process 'notepad.exe' (PID: 12345)
[+] Loaded 24 modules.

Press Enter to continue...
```

---

## Работа с оффсетами

### Пример 2: Создание конфигурации оффсетов

**Шаг 1: Подключиться к процессу**
```
Select option [0-7]: 1
Enter process name: csgo.exe
[+] Successfully attached to process 'csgo.exe' (PID: 23456)
[+] Loaded 156 modules.
```

**Шаг 2: Просмотреть модули**
```
Select option [0-7]: 7

=== Module List ===
Module Name                         | Base Address     | Size
---------------------------------------------------------------------------
csgo.exe                            | 0x7FF7A1D00000   | 0x37E000
client.dll                          | 0x7FF6A2000000   | 0x1A3C000
engine.dll                          | 0x7FF6A5000000   | 0xB3D000
server.dll                          | 0x7FF6A8000000   | 0x2F7000
...
```

**Шаг 3: Добавить оффсеты**
```
Select option [0-7]: 3

=== Add New Offset ===

Module name (e.g., client.dll): client.dll
[+] Module found. Base: 0x7FF6A2000000
Offset (hex, e.g., 0xDEA964): 0xDEA964
Description (optional, e.g., LocalPlayer): LocalPlayer

[+] Offset added successfully!
```

Повторяем для других оффсетов:
```
client.dll + 0x4DCC098 = EntityList
engine.dll + 0x58EFC4 = ViewAngles
engine.dll + 0x590DD0 = ClientState
```

**Шаг 4: Сохранить конфигурацию**
```
Select option [0-7]: 6

No offsets to save? No!
[✓] Offsets: 4 in storage

Enter filename to save (e.g., offsets.cfg): csgo_offsets.cfg
[+] Saved 4 offsets to csgo_offsets.cfg

Press Enter to continue...
```

**Результат (csgo_offsets.cfg)**:
```ini
# Offset Configuration File
# Format: ModuleName+0xOffset=Description

client.dll+0xDEA964=LocalPlayer
client.dll+0x4DCC098=EntityList
engine.dll+0x58EFC4=ViewAngles
engine.dll+0x590DD0=ClientState
```

---

### Пример 3: Загрузка и разрешение оффсетов

**После перезапуска процесса (ASLR изменил адреса)**

```
Select option [0-7]: 1
Enter process name: csgo.exe
[+] Successfully attached to process 'csgo.exe' (PID: 34567)  ← Новый PID!
[+] Loaded 156 modules.

Select option [0-7]: 2
Enter config filename: csgo_offsets.cfg
[+] Loaded 4 offsets from csgo_offsets.cfg

Select option [0-7]: 4

=== Resolving Offsets ===

[+] Successfully resolved 4/4 offsets.

Show resolved addresses? (y/n): y

=== Offset List ===
Module               | Offset       | Resolved Addr    | Description
-------------------------------------------------------------------------------------
client.dll           | 0xDEA964     | 0x7FF7A3DEA964   | LocalPlayer
client.dll           | 0x4DCC098    | 0x7FF7A7DCC098   | EntityList
engine.dll           | 0x58EFC4     | 0x7FF7A858EFC4   | ViewAngles
engine.dll           | 0x590DD0     | 0x7FF7A8590DD0   | ClientState

Press Enter to continue...
```

**Обратите внимание**: 
- Оффсеты (`0xDEA964` и т.д.) — не изменились
- Resolved адреса — полностью новые (ASLR)
- Всё работает автоматически!

---

## Экспорт модулей

### Пример 4: Дамп модулей процесса

```
Choose mode:
  1. Offset Manager
  2. Module Dumper
  0. Exit

Select option [0-2]: 2

╔═══════════════════════════════════════════════════╗
║              Module Dumper Mode                   ║
╚═══════════════════════════════════════════════════╝

Enter process name (e.g., hl2.exe): hl2.exe
[+] Successfully attached to process 'hl2.exe' (PID: 45678)
[+] Loaded 89 modules.

=== Module List ===
Module Name                         | Base Address     | Size
---------------------------------------------------------------------------
hl2.exe                             | 0x7FF6A1000000   | 0x2A4000
client.dll                          | 0x7FF6A3000000   | 0x1B5C000
engine.dll                          | 0x7FF6A5000000   | 0xC4D000
...

Save to file? (y/n): y
[+] Module list saved to hl2.exe_modules_dump.txt

Press Enter to continue...
```

**Результат (hl2.exe_modules_dump.txt)**:
```
Process: hl2.exe (PID: 45678)

Module Name                         | Base Address     | Size
---------------------------------------------------------------------------
hl2.exe                             | 0x7FF6A1000000   | 0x2A4000
client.dll                          | 0x7FF6A3000000   | 0x1B5C000
engine.dll                          | 0x7FF6A5000000   | 0xC4D000
...
```

---

## Реальные сценарии

### Сценарий 1: Разработка игрового чита

**Цель**: Найти адрес локального игрока в CS:GO для чтения HP/позиции.

**Действия**:

1. **Найти оффсет с помощью Cheat Engine**:
   - Найдите адрес LocalPlayer: `0x7FF6A2DEA964`
   - База client.dll: `0x7FF6A2000000`
   - Рассчитайте оффсет: `0xDEA964 - 0x000000 = 0xDEA964`

2. **Сохранить в приложение**:
   ```
   Offset Manager → Attach to csgo.exe → Add offset:
   Module: client.dll
   Offset: 0xDEA964
   Description: LocalPlayer
   ```

3. **Сохранить конфигурацию**: `Save → csgo_offsets.cfg`

4. **При каждом запуске чита**:
   ```cpp
   // Загружаем оффсеты
   offsetStorage.LoadFromFile(L"csgo_offsets.cfg");
   
   // Разрешаем адреса
   addressResolver.ResolveAll(offsetStorage);
   
   // Используем в коде
   uintptr_t localPlayerAddr = offsets[0].resolvedAddress;
   // Читаем HP, позицию и т.д.
   ```

---

### Сценарий 2: Автоматизация тестирования игры

**Цель**: Автоматически читать состояние игры для ботов/AI.

**Пример конфигурации (game_state_offsets.cfg)**:
```ini
# Game state offsets
client.dll+0xDEA964=LocalPlayer
client.dll+0x4DCC098=EntityList
client.dll+0x52BBFE0=GlowManager
engine.dll+0x58EFC4=ViewAngles
engine.dll+0x590DD0=ClientState
engine.dll+0x5A14A0=MaxPlayers
```

**Использование**:
```
1. Запускаем приложение
2. Attach to game.exe
3. Load offsets from game_state_offsets.cfg
4. Resolve all offsets
5. Экспортируем resolved адреса в JSON для бота
```

---

### Сценарий 3: Debugging/Reverse Engineering

**Цель**: Исследовать внутреннюю структуру приложения.

**Workflow**:

1. **Экспорт всех модулей**:
   ```
   Module Dumper → app.exe → Save to file
   ```

2. **Анализ в IDA Pro/Ghidra**:
   - Загружаем модуль в дизассемблер
   - Используем базовые адреса из дампа для правильного mapping

3. **Документирование находок**:
   ```
   Offset Manager → Add offset:
   module.dll+0x123456 = ImportantFunction
   module.dll+0x789ABC = GlobalConfig
   ```

4. **Версионирование**:
   - Сохраняем разные конфигурации для разных версий приложения
   - `app_v1.0_offsets.cfg`
   - `app_v1.1_offsets.cfg`

---

## Советы и трюки

### Совет 1: Работа с несколькими процессами

Создавайте отдельные конфигурации:
```
csgo_offsets.cfg
dota2_offsets.cfg
hl2_offsets.cfg
```

При загрузке просто выбираете нужный файл.

---

### Совет 2: Комментарии в конфигурации

```ini
# ========== Player Related ==========
client.dll+0xDEA964=LocalPlayer
client.dll+0x4DCC098=EntityList

# ========== View/Camera ==========
engine.dll+0x58EFC4=ViewAngles
engine.dll+0x590DD0=ClientState

# ========== Rendering ==========
client.dll+0x52BBFE0=GlowManager
```

---

### Совет 3: Проверка актуальности оффсетов

После обновления игры:
```
1. Attach to process
2. Load old offsets
3. Resolve all
4. View offsets

Если все разрешены успешно — оффсеты всё ещё актуальны!
Если некоторые не разрешились — модуль не найден, проверьте имя
```

---

### Совет 4: Backup конфигураций

```powershell
# PowerShell скрипт для backup
$date = Get-Date -Format "yyyy-MM-dd"
Copy-Item "offsets.cfg" "backups/offsets_$date.cfg"
```

---

### Совет 5: Автоматический запуск

Создайте `.bat` файл:
```batch
@echo off
echo Starting Offset Manager for CS:GO
ProcessModuleManager.exe
```

Или PowerShell:
```powershell
# auto_resolve_csgo.ps1
Start-Process "ProcessModuleManager.exe"
# TODO: автоматизировать ввод команд
```

---

### Совет 6: Работа с закрытыми процессами

Если процесс защищён (anti-cheat):

1. **Запуск от администратора** — обязательно
2. **Отключение anti-cheat** — для тестирования
3. **Использование kernel driver** — для продвинутых пользователей

**Текущее приложение работает только с user-mode доступом!**

---

## Часто задаваемые вопросы (FAQ)

### Q: Почему приложение не может найти процесс?

**A**: Проверьте:
1. Правильность имени процесса (с расширением `.exe`)
2. Процесс действительно запущен (Task Manager)
3. Регистр не важен: `CSGO.exe` = `csgo.exe`

---

### Q: "Failed to get module list. Try running as administrator"

**A**: Некоторые процессы требуют прав администратора.

**Решение**:
1. Правый клик на `ProcessModuleManager.exe`
2. "Запуск от имени администратора"

---

### Q: Модуль найден, но оффсет не разрешается

**A**: Проверьте:
1. Имя модуля точно такое же, как в списке модулей
2. Оффсет в hex формате: `0xDEA964` или `DEA964`
3. Модуль загружен в процесс (View module list)

---

### Q: Можно ли использовать для 32-битных процессов?

**A**: Теоретически да, но приложение скомпилировано как x64.

**Решение**: Пересоберите как x86 приложение.

---

### Q: Где хранятся конфигурации?

**A**: В той же папке, где запущен `ProcessModuleManager.exe`

Можно указать абсолютный путь:
```
Enter filename: C:\MyConfigs\offsets.cfg
```

---

### Q: Можно ли экспортировать в JSON?

**A**: В текущей версии — нет, только INI-like формат.

**Будущее**: Планируется поддержка JSON.

---

### Q: Как автоматизировать процесс?

**A**: Приложение интерактивное, но можно:
1. Использовать scripting (AutoHotkey, AutoIt)
2. Расширить код для command-line интерфейса
3. Создать wrapper на Python с использованием `subprocess`

Пример:
```python
import subprocess
proc = subprocess.Popen(['ProcessModuleManager.exe'], 
                        stdin=subprocess.PIPE, 
                        stdout=subprocess.PIPE)
proc.communicate(b"1\n1\ncsgo.exe\n2\noffsets.cfg\n4\n5\n0\n0\n")
```

---

### Q: Работает ли с играми с античитом?

**A**: **НЕТ!**

Приложение использует стандартные WinAPI функции, которые блокируются античитами:
- EasyAntiCheat
- BattlEye
- Vanguard (Valorant)

**Для research/testing используйте версии игр без античита или offline режим.**

---

## Примеры готовых конфигураций

### Counter-Strike: Source
```ini
# cs_source_offsets.cfg
client.dll+0x4A2D24=LocalPlayer
client.dll+0x489E84=EntityList
engine.dll+0x4D3F0C=ViewAngles
```

### Half-Life 2
```ini
# hl2_offsets.cfg
client.dll+0x5B7A94=LocalPlayer
engine.dll+0x4A54DC=ViewAngles
```

### Source Engine Generic
```ini
# source_generic.cfg
# These offsets may work across multiple Source games

client.dll+0x?????=LocalPlayer     # Find with CE
engine.dll+0x?????=ViewAngles      # Pattern: F3 0F 10 48
```

---

## Дополнительные ресурсы

- **Cheat Engine**: Для поиска начальных оффсетов
- **x64dbg**: Для динамического анализа
- **IDA Pro / Ghidra**: Для статического анализа
- **ReClass.NET**: Для реконструкции структур данных

---

## Заключение

Это приложение — фундамент для разработки инструментов реверс-инжиниринга.

**Следующие шаги**:
1. Изучите код в `ARCHITECTURE.md`
2. Расширьте функционал (pattern scanner, memory reader)
3. Интегрируйте в свои проекты

**Happy Reversing! 🔍**

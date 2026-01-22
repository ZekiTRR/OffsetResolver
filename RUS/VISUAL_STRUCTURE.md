# Визуальная структура

**[English](../VISUAL_STRUCTURE.md) | [Русский](VISUAL_STRUCTURE.md)**

```
Process-Module-Dumper/
│
├── 📂 Исходный код
│   ├── 🎯 main.cpp                      # Точка входа
│   │
│   ├── 📦 ProcessManager                # Управление процессами
│   │   ├── ProcessManager.h
│   │   └── ProcessManager.cpp
│   │
│   ├── 📦 ModuleRegistry                # Реестр модулей
│   │   ├── ModuleRegistry.h
│   │   └── ModuleRegistry.cpp
│   │
│   ├── 📦 AddressResolver               # Разрешение адресов
│   │   ├── AddressResolver.h
│   │   └── AddressResolver.cpp
│   │
│   ├── 📦 OffsetStorage                 # Хранение оффсетов
│   │   ├── OffsetStorage.h
│   │   └── OffsetStorage.cpp
│   │
│   ├── 📦 PointerChainResolver          # Разрешение цепочек
│   │   ├── PointerChainResolver.h
│   │   └── PointerChainResolver.cpp
│   │
│   ├── 📦 PointerChainStorage           # Хранение цепочек
│   │   ├── PointerChainStorage.h
│   │   └── PointerChainStorage.cpp
│   │
│   ├── 📦 MemoryReader                  # Чтение памяти
│   │   ├── MemoryReader.h
│   │   └── MemoryReader.cpp
│   │
│   ├── 📦 DebugLog                      # Система отладки
│   │   ├── DebugLog.h
│   │   └── DebugLog.cpp
│   │
│   └── 📦 ConsoleUI                     # Интерфейс
│       ├── ConsoleUI.h
│       └── ConsoleUI.cpp
│
├── 📂 Документация
│   ├── 📘 README_EN.md                  # Английское описание
│   ├── 📘 README.md                     # Русское описание
│   └── ... (другие .md файлы)
│
├── 📂 RUS/                              # Русская документация
│
└── 📂 build/                            # Директория сборки
```

---

## 🔄 Поток выполнения

```
┌─────────────────────────────────────────────────────────────────┐
│                        ЗАПУСК ПРИЛОЖЕНИЯ                         │
│                            main.cpp                              │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                      ConsoleUI::ShowMainMenu()                   │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                   Главное меню                            │  │
│  │  [DEBUG MODE ENABLED] [FILE LOGGING ON]                  │  │
│  │                                                          │  │
│  │  [1] Менеджер смещений                                   │  │
│  │  [2] Менеджер цепочек указателей                         │  │
│  │  [3] Дампер модулей                                      │  │
│  │  [0] Выход                                               │  │
│  │                                                          │  │
│  │  Команды: 'debug' | 'debugfile'                          │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 📊 Структуры данных

```
┌─────────────────────────────────────────────────────────────┐
│                        ModuleInfo                           │
├─────────────────────────────────────────────────────────────┤
│ + name: wstring                "app.dll"                    │
│ + baseAddress: uintptr_t        0x7FF6A2000000              │
│ + size: uintptr_t               0x1A3C000                   │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                    PointerChainEntry                        │
├─────────────────────────────────────────────────────────────┤
│ + moduleName: wstring          "app.dll"                    │
│ + baseOffset: uintptr_t         0x17E0A8                    │
│ + offsets: vector<uintptr_t>   [0x18, 0x70, 0x2D0]          │
│ + valueType: string            "float"                      │
│ + description: string          "Position"                   │
│                                                             │
│ Runtime:                                                    │
│ + resolvedAddress: uintptr_t    0x22A145682D0               │
│ + resolvedValue: string        "123.456"                    │
│ + isResolved: bool              true                        │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔐 Механизм защиты от ASLR

```
                    Запуск 1                    Запуск 2
                    ────────                    ────────

Загрузка модуля  → app.dll                   → app.dll
                   ├─ Base: 0x7FF6A2000000    ├─ Base: 0x7FF7A3000000  ← ИЗМЕНИЛСЯ!
                   └─ Size: 0x1A3C000         └─ Size: 0x1A3C000

Offset (cfg)     → app.dll+0xDEA964          → app.dll+0xDEA964       ← НЕ ИЗМЕНИЛСЯ!

Разрешение       → 0x7FF6A2000000             → 0x7FF7A3000000
                   + 0xDEA964                   + 0xDEA964
                   ─────────────────            ─────────────────
                   = 0x7FF6A2DEA964  ✓          = 0x7FF7A3DEA964  ✓

Результат        → Правильный адрес            → Правильный адрес
```

---

## 🔗 Разрешение цепочки указателей

```
Цепочка: app.dll+0x17E0A8 -> [0x18, 0x70, 0x2D0]

Шаг 1: Получить базу модуля
       app.dll base = 0x7FF6A2000000

Шаг 2: Вычислить базовый адрес
       0x7FF6A2000000 + 0x17E0A8 = 0x7FF6A217E0A8

Шаг 3: Прочитать первый указатель
       Read(0x7FF6A217E0A8) = 0x22A14567890

Шаг 4: Применить смещения
       0x22A14567890 + 0x18 = 0x22A145678A8
       Read(0x22A145678A8) = 0x22A14568000
       ...

Шаг 5: Прочитать финальное значение
       Read<float>(0x22A145684D0) = 123.456
```

---

## 🔍 Debug вывод

```
┌─────────────────────────────────────────────────────────────┐
│ Debug Mode: ENABLED                                         │
│ File Logging: debug_log.txt                                 │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│ [INFO]  Starting chain resolution for 'Health'              │
│ [PTR]   Module 'app.dll' base: 0x7FF6A2000000              │
│ [STEP]  Step 1/3: Read at 0x7FF6A217E0A8 = 0x22A14567890   │
│ [SUCCESS] Value (int): 100                                  │
└─────────────────────────────────────────────────────────────┘
```

---

## 📝 Форматы конфигурации

### Формат оффсетов (offsets.cfg)
```ini
# Комментарий
app.dll+0xDEA964=PlayerBase
module2.dll+0x58EFC4=ViewAngles
```

### Формат цепочек (chains.txt)
```
# moduleName|baseOffset|offsets|valueType|description
app.dll|0x17E0A8|0x18,0x70,0x2D0|float|Position
app.dll|0x17E0A8|0xEC|int|Health
```

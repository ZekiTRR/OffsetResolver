# CMakeLists.txt для Process Module & Offset Management Tool

cmake_minimum_required(VERSION 3.10)
project(ProcessModuleManager)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исходные файлы
set(SOURCES
    main.cpp
    ProcessManager.cpp
    ModuleRegistry.cpp
    AddressResolver.cpp
    OffsetStorage.cpp
    ConsoleUI.cpp
)

# Заголовочные файлы
set(HEADERS
    ProcessManager.h
    ModuleRegistry.h
    AddressResolver.h
    OffsetStorage.h
    ConsoleUI.h
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Настройки для Windows
if(WIN32)
    # Консольное приложение
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE OFF
    )
    
    # Установка Unicode
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        UNICODE 
        _UNICODE
    )
endif()

# Вывод информации
message(STATUS "Building Process Module & Offset Management Tool")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

# CMakeLists.txt для Process Module & Pointer Chain Management Tool

cmake_minimum_required(VERSION 3.10)
project(ProcessModuleManager)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Auto-detect source files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

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

    # Линкование стандартных Windows библиотек
    target_link_libraries(${PROJECT_NAME} PRIVATE
        kernel32.lib
        user32.lib
        advapi32.lib
    )

    # MSVC флаги компилятора для правильного поиска заголовков
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE
            /EHsc
            /permissive-
            /std:c++17
        )
    endif()
endif()

# Вывод информации
message(STATUS "Building Process Module & Pointer Chain Management Tool")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Project: ${PROJECT_NAME}")

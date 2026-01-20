#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

// Структура для хранения информации о модуле
struct ModuleInfo
{
    std::wstring name;
    uintptr_t baseAddress;
    uintptr_t size;
};

// Функция для получения PID процесса по имени
DWORD GetProcessIdByName(const std::wstring &processName)
{
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnapshot, &pe32))
        {
            do
            {
                if (processName == pe32.szExeFile)
                {
                    pid = pe32.th32ProcessID;
                    break;
                }
            } while (Process32NextW(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    return pid;
}

// Функция для получения списка модулей процесса
std::vector<ModuleInfo> GetProcessModules(DWORD pid)
{
    std::vector<ModuleInfo> modules;
    // TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32 позволяет получать модули и для 32-битных, и для 64-битных процессов
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32W me32;
        me32.dwSize = sizeof(MODULEENTRY32W);

        if (Module32FirstW(hSnapshot, &me32))
        {
            do
            {
                ModuleInfo info;
                info.name = me32.szModule;
                info.baseAddress = (uintptr_t)me32.modBaseAddr;
                info.size = me32.modBaseSize;
                modules.push_back(info);
            } while (Module32NextW(hSnapshot, &me32));
        }
        CloseHandle(hSnapshot);
    }
    return modules;
}

int main()
{
    // Установка локали для корректного отображения кириллицы в консоли
    setlocale(LC_ALL, "");

    std::wcout << L"Введите название процесса (например, hl2.exe): ";
    std::wstring processName;
    std::wcin >> processName;

    DWORD pid = GetProcessIdByName(processName);

    if (pid == 0)
    {
        std::wcout << L"[-] Процесс '" << processName << L"' не найден." << std::endl;
        system("pause");
        return 1;
    }

    std::wcout << L"[+] Процесс найден. PID: " << pid << std::endl;

    std::wcout << L"Сохранить список модулей и адресов в файл? (y/n): ";
    wchar_t saveChoice;
    std::wcin >> saveChoice;
    bool saveToFile = (saveChoice == L'y' || saveChoice == L'Y');

    std::vector<ModuleInfo> modules = GetProcessModules(pid);

    if (modules.empty())
    {
        std::wcout << L"[-] Не удалось получить список модулей. Попробуйте запустить программу от имени администратора." << std::endl;
        system("pause");
        return 1;
    }

    std::wofstream file;
    std::string total_name = std::string(processName.begin(), processName.end()) + "_modules_dump.txt";

    if (saveToFile)
    {
        file.open(total_name);
        if (!file.is_open())
        {
            std::wcout << L"[-] Ошибка создания файла " << std::wstring(total_name.begin(), total_name.end()) << std::endl;
            saveToFile = false;
        }
    }

    std::wcout << L"\nСписок модулей:\n";
    std::wcout << std::left << std::setw(35) << L"Имя модуля" << L" | " << L"Базовый адрес" << std::endl;
    std::wcout << std::wstring(60, L'-') << std::endl;

    if (saveToFile)
    {
        file << std::left << std::setw(35) << L"Имя модуля" << L" | " << L"Базовый адрес" << std::endl;
        file << std::wstring(60, L'-') << std::endl;
    }

    for (const auto &mod : modules)
    {
        // Вывод в консоль
        std::wcout << std::left << std::setw(35) << mod.name << L" | 0x"
                   << std::hex << std::uppercase << mod.baseAddress << std::dec << std::endl;

        // Запись в файл
        if (saveToFile)
        {
            file << std::left << std::setw(35) << mod.name << L" | 0x"
                 << std::hex << std::uppercase << mod.baseAddress << std::dec << std::endl;
        }
    }

    if (saveToFile)
    {
        std::wcout << L"\n[+] Данные успешно сохранены в " << std::wstring(total_name.begin(), total_name.end()) << std::endl;
        file.close();
    }

    std::wcout << L"\nНажмите Enter, чтобы выйти...";
    std::wcin.ignore();
    std::wcin.get();

    return 0;
}

#include "DebugLog.h"

// Инициализация статических переменных
bool DebugLog::s_enabled = false;
bool DebugLog::s_fileLogging = false;
std::wofstream DebugLog::s_logFile;
HANDLE DebugLog::s_consoleHandle = nullptr;

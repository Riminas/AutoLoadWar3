#include "ConfigMapsEngine.h"
#include "LoadDataFail.h"
#include "LoadManager.h"
#include "StringConvector.h"
#include "LogError.h"

ConfigMapsEngine::ConfigMapsEngine(const std::wstring& nameMaps)
{
    m_FilePathFull = m_FilePath1 + L"/" + nameMaps + m_FilePath2;
    if (!std::filesystem::exists(m_FilePathFull)) {

        std::filesystem::path fileNam = m_FilePath1;
        std::filesystem::create_directories(fileNam.c_str());

        fileNam += L"/" + nameMaps;
        std::filesystem::create_directories(fileNam.c_str());

        initializeDefaultConfig();
    }
}

void ConfigMapsEngine::initializeDefaultConfig()
{
    LogError().logMessage("новый конфиг карты.");
    G_CONFIG_MAPS.path = L"";
    G_CONFIG_MAPS.mainConfig = { {
         { "-return" },
         { "-save" },
         { "-create" },
         { "-cam 150" }
         } };

    ConfigMapsEngine::saveConfigMain();
}

bool ConfigMapsEngine::loadConfigMain()
{
    const std::wstring fullLine = LoadDataFail().loadDataFailW(m_FilePathFull);
    if (fullLine.empty()) {
        LogError().logErrorW(L"Unable to open file: " + m_FilePathFull);
        return false;
    }

    std::wistringstream wiss(fullLine);
    std::wstring line;
    std::wstring section;
     
    while (std::getline(wiss, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }

        line.erase(std::remove(line.begin(), line.end(), L'\r'), line.end());
        const std::string strLine{ StringConvector().utf16_to_utf8(line) };
        std::istringstream iss(strLine);
        std::string key, value;

        if (line[0] == '[') {
            section = line.substr(1, line.find(']') - 1);
        }
        else if (std::getline(iss, key, '=') ) {
            if (!std::getline(iss, value))
                value = "";
            if (section == L"Path") {
                if (value.empty()) {
                    G_CONFIG_MAPS.path = { L"" };
                }
                else {
                    G_CONFIG_MAPS.path = StringConvector().utf8_to_utf16(value);
                    if (G_CONFIG_MAPS.path.back() == L'\r')
                        G_CONFIG_MAPS.path.pop_back();
                
                }
            }
            else if (section == L"MainConfig") {
                if (key.find("cmd") == 0) {
                    int index = parseInt(key.substr(3));
                    if (index < G_CONFIG_MAPS.mainConfig.size())
                        G_CONFIG_MAPS.mainConfig[index].cmd = value; /*{ value.empty() ? " " : value };*/ /*std::string(value.begin(), value.end());*/;
                }
                else if (key.find("start") == 0) {
                    int index = parseInt(key.substr(5));
                    if (index < G_CONFIG_MAPS.mainConfig.size())
                        G_CONFIG_MAPS.mainConfig[index].start = parseBool(value);
                }
            }
            else if (section == L"UsersConfig") {
                if (key.find("cmd") == 0) {
                    int index = parseInt(key.substr(3));
                    if (index < G_CONFIG_MAPS.usersConfig.size())
                        G_CONFIG_MAPS.usersConfig[index].cmd = value; /*{ value.empty() ? " " : value };*/ /*std::string(value.begin(), value.end());*/
                }
                else if (key.find("start") == 0) {
                    int index = parseInt(key.substr(5));
                    if (index < G_CONFIG_MAPS.usersConfig.size())
                        G_CONFIG_MAPS.usersConfig[index].start = parseBool(value);
                }
                else if (key.find("isVisibleButton") == 0) {
                    int index = parseInt(key.substr(15));
                    if (index < G_CONFIG_MAPS.usersConfig.size())
                        G_CONFIG_MAPS.usersConfig[index].isVisibleButton = parseBool(value);
                }
            }
        }
    }

    return true;
}

bool ConfigMapsEngine::saveConfigMain()
{
    std::ofstream file(m_FilePathFull);
    if (!file.is_open()) {
        LogError().logErrorW(L"Unable to open file: " + m_FilePathFull);
        return false;
    }

    file << "[Path]" << std::endl;
    file << "path=" << std::string(G_CONFIG_MAPS.path.begin(), G_CONFIG_MAPS.path.end()) << std::endl;

    file << "[MainConfig]" << std::endl;
    for (size_t i = 0; i < G_CONFIG_MAPS.mainConfig.size(); ++i) {
        switch (i) {
        case 0: file << "; Return" << std::endl; break;
        case 1: file << "; save" << std::endl; break;
        case 2: file << "; Craft" << std::endl; break;
        case 3: file << "; Camera" << std::endl; break;
        }
        file << "cmd" << i << "=" << std::string(G_CONFIG_MAPS.mainConfig[i].cmd.begin(), G_CONFIG_MAPS.mainConfig[i].cmd.end()) << std::endl;
        file << "start" << i << "=" << (G_CONFIG_MAPS.mainConfig[i].start ? "true" : "false") << std::endl;
        file << std::endl;
    }

    file << "[UsersConfig]" << std::endl;
    for (size_t i = 0; i < G_CONFIG_MAPS.usersConfig.size(); ++i) {
        file << "cmd" << i << "=" << std::string(G_CONFIG_MAPS.usersConfig[i].cmd.begin(), G_CONFIG_MAPS.usersConfig[i].cmd.end()) << std::endl;
        file << "start" << i << "=" << (G_CONFIG_MAPS.usersConfig[i].start ? "true" : "false") << std::endl;
        file << "isVisibleButton" << i << "=" << (G_CONFIG_MAPS.usersConfig[i].isVisibleButton ? "true" : "false") << std::endl;
        file << std::endl;
    }

    file.close();
    return true;
}

void ConfigMapsEngine::openConfigMain()
{
    // Подготовка структуры для запуска процесса
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi = { 0 };

    // Создаем командную строку для запуска блокнота с файлом
    std::wstring command = L"notepad.exe \"" + m_FilePathFull + L"\"";

    // Запускаем процесс
    BOOL success = CreateProcess(
        NULL,                         // Имя приложения (NULL для использования командной строки)
        command.data(),               // Командная строка
        NULL,                         // Атрибуты безопасности для процесса
        NULL,                         // Атрибуты безопасности для потока
        FALSE,                        // Унаследовать дескрипторы
        0,                            // Флаги создания процесса
        NULL,                         // Переменные окружения
        NULL,                         // Текущий каталог
        &si,                          // Информация о запуске
        &pi                           // Информация о процессе
    );

    if (!success) {
        LogError().logError("неудалось открыть блокнот.");
        return;
    }

    // Ожидание завершения процесса
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Закрытие дескрипторов процесса и потока
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    LogError().logMessage("блокнот закрыт.");


    //// Преобразуем std::string в LPCSTR (необходимый тип для ShellExecute)
    //LPCWSTR filePath = m_FilePathFull.c_str();

    //// Используем ShellExecute для открытия файла в ассоциированном приложении
    //HINSTANCE result = ShellExecute(
    //    NULL,           // HWND, указатель на окно (NULL для отсутствия)
    //    L"open",         // Действие (open для открытия)
    //    filePath,       // Путь к файлу
    //    NULL,           // Дополнительные параметры (не используются)
    //    NULL,           // Рабочий каталог (не используется)
    //    SW_SHOWDEFAULT  // Стиль отображения окна
    //);

    //// Проверка результата
    //if ((int)result <= 32) { // Ошибка, если результат меньше или равен 32
    //    std::wcerr << "Error: Failed to open file " << m_FilePathFull << std::endl;
    //}
}

void ConfigMapsEngine::startGameCoutCmd(const HWND& hWndWindow)
{
    LoadManager LoadManager_(hWndWindow);

    for (CmdEntry& cmds : G_CONFIG_MAPS.mainConfig) {
        if (cmds.start) {
            LoadManager_.sendLoadDataCommands({ cmds.cmd }, true);
        }
    }

    for (CmdEntry& cmds : G_CONFIG_MAPS.usersConfig) {
        if (cmds.start) {
            LoadManager_.sendLoadDataCommands({ cmds.cmd }, true);
        }
    }
}

std::wstring ConfigMapsEngine::trim(const std::wstring& str)
{
    std::wstring::const_iterator it = str.begin();
    while (it != str.end() && isspace(*it)) it++;

    std::wstring::const_reverse_iterator rit = str.rbegin();
    while (rit.base() != it && isspace(*rit)) rit++;

    return std::wstring(it, rit.base());
}

bool ConfigMapsEngine::parseBool(const std::string& value)
{
    return value == "true" || value == "true\r" || value == "1";
}

int ConfigMapsEngine::parseInt(const std::string& value)
{
    return std::stoi(value);
}
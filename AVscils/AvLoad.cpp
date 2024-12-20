#include <windows.h>
#include <iostream>
#include "Engine.h"
#include "LogManager.h"
#include "ConfigMainEngine.h"

int main()
{
    if (FindWindow(NULL, L"AutoLoadWar3") == NULL) {

        ConfigMainEngine().loadConfigMain();//должно полнится только 1 раз при старте программы

        LogManager::logger().log(LogManager::LogLevel::Message, L"\n\n------------------------------------------------------------------------");
        Engine().engine1();
    }
    else
        MessageBox(NULL, L"Программа уже запущена.", L"Оповищение", MB_OK | MB_ICONEXCLAMATION);
    
    return 0;
}
//if (G_CONFIG_MAIN.optionsConfig.autoUpdate) {
//    update();
//}
// 
//#include "json.hpp"
//using json = nlohmann::json;
//
//void update() {
//    AutoUpdate AutoUpddate_;
//    std::string releaseInfo = AutoUpddate_.GetLatestReleaseInfo(L"Riminas", L"AutoLoadWar3");
//
//    if (!releaseInfo.empty())
//    {
//        json jsonData = json::parse(releaseInfo);
//
//        std::string latestVersion = jsonData["tag_name"];
//
//        // Предположим, что currentVersion - это строка с текущей версией вашей программы
//        std::string currentVersion = "AutoLoad_1_18_16";
//
//        if (latestVersion != currentVersion)
//        {
//
//            std::string downloadUrl;
//            for (const auto& asset : jsonData["assets"])
//            {
//                std::string name = asset["name"];
//                if (name == "AvLoad.exe") // или другое имя вашего файла(в гит хаб)
//                {
//                    downloadUrl = asset["browser_download_url"];
//                    break;
//                }
//            }
//
//            wchar_t modulePath[MAX_PATH];
//            GetModuleFileName(NULL, modulePath, MAX_PATH);
//            std::wstring savePath = modulePath;
//            savePath.erase(savePath.size() - 11);
//            savePath += L"\\AutoLoad.exe";//куда будет сохранена программа и с каким именем
//
//
//            LogManager::logger().log(LogManager::LogLevel::Message, L"savePath " + savePath);
//
//            if (AutoUpddate_.DownloadFile(downloadUrl, savePath))
//            {
//                // Перейдите к установке обновления
//                ShellExecute(NULL, L"open", L"update.exe", NULL, NULL, SW_SHOWNORMAL);
//            }
//            else
//            {
//                LogManager::logger().log(LogManager::LogLevel::Error, "Ошибка при загрузке файла обновления.");
//                LogManager::logger().log(LogManager::LogLevel::Message, "Доступно обновление до версии " + latestVersion);
//                LogManager::logger().log(LogManager::LogLevel::Message, "адрес " + downloadUrl);
//            }
//
//
//        }
//        //else
//        //{
//        //    LogManager::logger().log(LogManager::LogLevel::Message, "У вас установлена последняя версия.");
//        //}
//    }
//    else
//    {
//        LogManager::logger().log(LogManager::LogLevel::Message, "Не удалось получить информацию о последнем релизе.");
//    }
//}
// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "Engine.h"

ULONG_PTR gdiplusToken;

extern "C" {
    __declspec(dllexport) void StartEngine() {
        Engine::getInstance().RunEngineDll();
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            Gdiplus::GdiplusStartupInput gdiplusStartupInput;
            Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        Gdiplus::GdiplusShutdown(gdiplusToken);
        break;
    }
    return TRUE;
}

//// dllmain.cpp : Определяет точку входа для приложения DLL.
//#include "pch.h"
//
//BOOL APIENTRY DllMain(HMODULE hModule,
//    DWORD  ul_reason_for_call,
//    LPVOID lpReserved
//)
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//}
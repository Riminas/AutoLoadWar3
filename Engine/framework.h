#pragma once

#define WIN32_LEAN_AND_MEAN
#define ENGINE_EXPORTS

// Windows Header Files
#include <windows.h>
#include <commdlg.h>    // Для GetOpenFileName
#pragma comment(lib, "comdlg32.lib")

// GDI+ Headers - правильный порядок важен!
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
//#pragma once
//
//#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
//// Файлы заголовков Windows
//#include <windows.h>


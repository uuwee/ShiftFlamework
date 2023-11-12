#pragma once

#if defined DLL_EXPORT
#define DLL __declspec(dllexport)
#else if defined DLL_IMPORT
#define DLL __declspec(dllimport)
#endif
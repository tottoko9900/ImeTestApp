#pragma once

#ifdef EVENTHOOKER_EXPORTS
#define EVENTHOOKER_API extern "C" __declspec(dllexport)
#else
#define EVENTHOOKER_API extern "C" __declspec(dllimport)
#endif

EVENTHOOKER_API void InstallHook();
EVENTHOOKER_API void UninstallHook();


#include "pch.h"
#include "exports.h"
#include "EventHooker.h"

EventHooker* g_pEventHooker = NULL;

EVENTHOOKER_API void InstallHook()
{
	if(!g_pEventHooker)
	{
		g_pEventHooker = new EventHooker();
		g_pEventHooker->InstallHook();
	}
}

EVENTHOOKER_API void UninstallHook()
{
	if(g_pEventHooker)
	{
		g_pEventHooker->UnstallHook();
		delete g_pEventHooker;
		g_pEventHooker = NULL;
	}
}

#pragma once
#include <string>

class EventHooker {
public:
	EventHooker();
	~EventHooker();
	void InstallHook();
	void UnstallHook();

private:
	static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void SendCompositionString(HWND hTargetWnd, const std::wstring& text);

	HHOOK        m_hHook;
	HINSTANCE    m_hDllInstance;
};


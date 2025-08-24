#pragma once
#include <string>

class EventHooker {
public:
	EventHooker();
	void InstallHook();
	void UnstallHook();


private:
	static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void SendCompositionString(HWND hTargetWnd, const std::wstring& text);

	static HHOOK m_hHook;
	static HINSTANCE m_hInst;
};


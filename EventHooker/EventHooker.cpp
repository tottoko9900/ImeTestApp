#include "pch.h"
#include "framework.h"
#include "EventHooker.h"
#include <iostream>
#include <format>
#include <imm.h>
#include <Windows.h>

#pragma comment(lib, "imm32.lib")

extern "C" IMAGE_DOS_HEADER __ImageBase;

template<class... Args> 
void TraceLog(const std::wstring& log, Args... args) 
{ 
    std::wstring formatLog = std::vformat(log, std::make_wformat_args(args...));
    OutputDebugString(formatLog.c_str()); 
}

EventHooker::EventHooker() : m_hHook(nullptr), m_hDllInstance(nullptr)
{
    m_hDllInstance = (HINSTANCE)&__ImageBase;
}

EventHooker::~EventHooker()
{
    if(m_hHook)
    {
        UnstallHook();
    }
}

void EventHooker::InstallHook()
{
    m_hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, m_hDllInstance, 0);
    if(!m_hHook)
    {
        DWORD errorCode = GetLastError();
        TraceLog(L"EventHooker::InstallHook() Failed. ErrorCode = 0x{:X}", errorCode);
    }
}

void EventHooker::UnstallHook()
{
    if(m_hHook)
    {
        BOOL result = UnhookWindowsHookEx(m_hHook);
        if(!result)
        {
            DWORD errorCode = GetLastError();
            TraceLog(L"EventHooker::UnstallHook() Failed. ErrorCode = 0x{:X}", errorCode);
        }
        m_hHook = nullptr;
    }
}

LRESULT CALLBACK EventHooker::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode == HC_ACTION && wParam == PM_REMOVE)
    {
        MSG* pMsg = reinterpret_cast<MSG*>(lParam);

        if(pMsg->message == WM_IME_COMPOSITION)
        {
            TraceLog(L"EventHooker::GetMsgProc()  Receive WM_IME_COMPOSITION");
            HWND hWnd = pMsg->hwnd;
            HIMC hImc = ImmGetContext(hWnd);
            if(hImc)
            {
                if(pMsg->lParam & GCS_RESULTSTR)
                {
                    LONG size = ImmGetCompositionString(hImc, GCS_RESULTSTR, nullptr, 0);
                    if(size > 0)
                    {
                        std::wstring comp;
                        comp.resize(size / sizeof(wchar_t));
                        ImmGetCompositionString(hImc, GCS_RESULTSTR, comp.data(), size);

                        HWND targetWnd = FindWindow(NULL, L"MainWindow");
                        if(targetWnd)
                        {
                            SendCompositionString(targetWnd, comp);
                        }
                    }
                }
                else if(pMsg->lParam & GCS_COMPSTR)
                {
                    LONG size = ImmGetCompositionString(hImc, GCS_COMPSTR, nullptr, 0);
                    if(size > 0)
                    {
                        std::wstring comp;
                        comp.resize(size / sizeof(wchar_t));
                        ImmGetCompositionString(hImc, GCS_COMPSTR, comp.data(), size);

                        HWND targetWnd = FindWindow(NULL, L"MainWindow");
                        if(targetWnd)
                        {
                            SendCompositionString(targetWnd, comp);
                        }
                    }
                }

                ImmReleaseContext(hWnd, hImc);
            }
            else
            {
                DWORD errorCode = GetLastError();
                TraceLog(L"EventHooker::GetMsgProc() ImmGetContext() Failed. ErrorCode = 0x{:X}", errorCode);
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void EventHooker::SendCompositionString(HWND hTargetWnd, const std::wstring& text)
{
    COPYDATASTRUCT cds;
    cds.dwData = 1;
    cds.cbData = static_cast<DWORD>((text.size() + 1) * sizeof(wchar_t));
    cds.lpData = (PVOID)text.c_str();

    SendMessage(hTargetWnd, WM_COPYDATA, 0, (LPARAM)&cds);
}
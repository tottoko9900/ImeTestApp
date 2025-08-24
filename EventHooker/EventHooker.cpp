#include "pch.h"
#include "framework.h"
#include "EventHooker.h"
#include <imm.h>
#include <Windows.h>

#pragma comment(lib, "imm32.lib")

extern "C" IMAGE_DOS_HEADER __ImageBase;

HHOOK     EventHooker::m_hHook = NULL;
HINSTANCE EventHooker::m_hInst = NULL;

EventHooker::EventHooker()
{
    m_hInst = (HINSTANCE)&__ImageBase;
}

void EventHooker::InstallHook()
{
    OutputDebugString(L"$$$ : EventHooker::InstallHook()");
    //m_hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, m_hInst, 0);
    m_hHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, m_hInst, 0);
    if(!m_hHook)
    {
        OutputDebugString(L"$$$ : SetWindowsHookEx() Failed!!");
    }
}

void EventHooker::UnstallHook()
{
    if(m_hHook)
    {
        BOOL result = UnhookWindowsHookEx(m_hHook);
        if(!result)
        {
            OutputDebugString(L"$$$ : UnhookWindowsHookEx() Failed!!");
        }
        m_hHook = NULL;
    }
}

LRESULT CALLBACK EventHooker::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode == HC_ACTION)
    {
        CWPSTRUCT* p = (CWPSTRUCT*)lParam;

        if(p->message == WM_IME_COMPOSITION)
        {
            OutputDebugString(L"$$$ : CallWndProc() Receive WM_IME_COMPOSITION");
            HWND hWnd = p->hwnd;
            HIMC hImc = ImmGetContext(hWnd);
            if(hImc)
            {
                if(p->lParam & GCS_RESULTSTR)
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
                else if(p->lParam & GCS_COMPSTR)
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
                OutputDebugString(L"$$$ : ImmGetContext() Failed!!");
            }
        }
    }

    return CallNextHookEx(m_hHook, nCode, wParam, lParam);
}

LRESULT CALLBACK EventHooker::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode == HC_ACTION)
    {
        MSG* p = (MSG*)lParam;

        if(p->message == WM_IME_COMPOSITION)
        {
            OutputDebugString(L"$$$ : GetMsgProc() Receive WM_IME_COMPOSITION");
            HWND hWnd = p->hwnd;
            HIMC hImc = ImmGetContext(hWnd);
            if(hImc)
            {
                if(p->lParam & GCS_RESULTSTR)
                {
                    OutputDebugString(L"$$$ : GetMsgProc()   GCS_RESULTSTR");
                }

                ImmReleaseContext(hWnd, hImc);
            }
            else
            {
                OutputDebugString(L"$$$ : ImmGetContext() Failed!!");
            }
        }
    }

    return CallNextHookEx(m_hHook, nCode, wParam, lParam);
}

void EventHooker::SendCompositionString(HWND hTargetWnd, const std::wstring& text)
{
    COPYDATASTRUCT cds;
    cds.dwData = 1;
    cds.cbData = (text.size() + 1) * sizeof(wchar_t);
    cds.lpData = (PVOID)text.c_str();

    SendMessage(hTargetWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}
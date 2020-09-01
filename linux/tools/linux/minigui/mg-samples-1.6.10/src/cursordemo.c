/* 
** $Id: cursordemo.c,v 1.10 2007-08-30 01:20:10 xwyan Exp $
**
** Listing 10.1
**
** cursordemo.c: Sample program for MiniGUI Programming Guide
**      Demo of using cursor
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define IDC_TRAP    100

static HWND hTrapWin, hMainWnd;
static RECT rcMain, rc;

static int TrapwindowProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    static BOOL bTrapped = FALSE;

    switch (message) {
    case MSG_KEYDOWN:
        if (bTrapped) {
            ClipCursor (NULL);
            bTrapped = FALSE;
        }
        break;

    case MSG_MOUSEMOVE:
        if (!bTrapped) {
            GetWindowRect(hWnd, &rc);
            ClientToScreen(hMainWnd, &rc.left, &rc.top);
            ClientToScreen(hMainWnd, &rc.right, &rc.bottom);
            ClipCursor(&rc);
            bTrapped = TRUE;
        }
        break;
    }

    return DefaultControlProc(hWnd, message, wParam, lParam);
}

BOOL RegisterTrapwindow (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = "trap";
    WndClass.dwStyle     = 0;
    WndClass.dwExStyle   = 0;
    WndClass.hCursor     = GetSystemCursor(IDC_HAND_POINT);
    WndClass.iBkColor    = PIXEL_black;
    WndClass.WinProc     = TrapwindowProc;

    return RegisterWindowClass (&WndClass);
}

static int CursordemoWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_CREATE:
        RegisterTrapwindow();
        hTrapWin = CreateWindow("trap", "", WS_VISIBLE | WS_CHILD, IDC_TRAP, 
                10, 10, 100, 100, hWnd, 0);
        break;

    case MSG_LBUTTONDOWN:
        ShowCursor(FALSE);
        break;

    case MSG_RBUTTONDOWN:
        ShowCursor(TRUE);
        break;

    case MSG_SETCURSOR:
        SetCursor (GetSystemCursor (IDC_IBEAM));
        return 0;

    case MSG_CLOSE:
        DestroyAllControls (hWnd);
        DestroyMainWindow (hWnd);
        PostQuitMessage (hWnd);
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int args, const char* arg[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "cursordemo" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Demo of using cursor";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = CursordemoWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 320;
    CreateInfo.by = 240;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif


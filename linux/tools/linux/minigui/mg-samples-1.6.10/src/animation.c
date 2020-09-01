/* 
** $Id: animation.c,v 1.12 2007-08-30 01:20:10 xwyan Exp $
**
** Listing 36.1
**
** animation.c: Sample program for MiniGUI Programming Guide
**      Usage of ANIMATION control and GIF89a loader.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>

static const char* filename = "9.gif";
int ret;

static ANIMATION* anim = NULL;

static int AnimationWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_CREATE:
    {
        anim = CreateAnimationFromGIF89aFile (HDC_SCREEN, filename);
        if (anim == NULL)
            return 1;

        SetWindowAdditionalData (hWnd, (DWORD) anim);
        CreateWindowEx (CTRL_ANIMATION, 
                          "", 
                          WS_VISIBLE | ANS_AUTOLOOP | ANS_SCALED, 
                          WS_EX_TRANSPARENT,
                          100, 
                          50, 10, 120, 60, hWnd, (DWORD)anim);
        SendMessage (GetDlgItem (hWnd, 100), ANM_STARTPLAY, 0, 0);

        CreateWindowEx (CTRL_ANIMATION, 
                          "", 
                          WS_VISIBLE | ANS_AUTOLOOP | ANS_FITTOANI,
                          WS_EX_TRANSPARENT,
                          200, 
                          50, 160, 120, 60, hWnd, (DWORD)anim);
        break;
    }

    case MSG_LBUTTONDOWN:
        SendMessage (GetDlgItem (hWnd, 200), ANM_SETANIMATION, 0, (LPARAM)anim);
        SendMessage (GetDlgItem (hWnd, 200), ANM_STARTPLAY, 0, 0);
	break;

    case MSG_DESTROY:
        DestroyAnimation ((ANIMATION*)GetWindowAdditionalData (hWnd), TRUE);
        DestroyAllControls (hWnd);
        return 0;

    case MSG_CLOSE:
        ret = PostQuitMessage (hWnd);
        DestroyMainWindow (hWnd);
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    if (argc >= 2)
        filename = argv[1];

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "animation" , 0 , 0);
#endif

    if (!InitMiniGUIExt()) {
        return 1;
    }

    CreateInfo.dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
#ifdef _LANG_ZHCN
    CreateInfo.spCaption = "¶¯»­¿Ø¼þ" ;
#else
    CreateInfo.spCaption = "Animation Control" ;
#endif
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (IDC_ARROW);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = AnimationWinProc;
    CreateInfo.lx = 0; 
    CreateInfo.ty = 0;
    CreateInfo.rx = 240;//GetGDCapability(HDC_SCREEN, GDCAP_HPIXEL);
    CreateInfo.by = 300;//GetGDCapability(HDC_SCREEN, GDCAP_VPIXEL);
    CreateInfo.iBkColor = PIXEL_lightgray;
    CreateInfo.dwAddData = 0;
    CreateInfo.dwReserved = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow (&CreateInfo);
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    MiniGUIExtCleanUp ();
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif


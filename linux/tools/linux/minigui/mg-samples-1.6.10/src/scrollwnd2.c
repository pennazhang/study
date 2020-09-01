/*
** $Id: scrollwnd2.c,v 1.6 2007-10-25 08:12:27 weiym Exp $
**
** scrollwnd2.c: Sample program for MiniGUI Programming Guide
**      Usage of SCROLLWND control.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/


#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


#define IDC_SCROLLWND          100
#define ID_ZOOMIN              200
#define ID_ZOOMOUT             300


static int pic_container_proc (HWND hWnd, int message, 
                WPARAM wParam, LPARAM lParam)
{
    return DefaultContainerProc (hWnd, message, wParam, lParam);
}

#define IDC_PROMPTINFO	100
#define IDC_PROGRESS  	110

static CTRLDATA controls [] =
{ 
    {
        "static",
        WS_VISIBLE | SS_SIMPLE,
        10, 10, 380, 16, 
        IDC_PROMPTINFO, 
#ifdef _LANG_ZHCN
        "е§дк...",
#else
        "Initializing...",
#endif
        0
    },
    {
        "progressbar",
        WS_VISIBLE,
        10, 40, 380, 20,
        IDC_PROGRESS,
        NULL,
        0
    },
    {
        "button",
        WS_TABSTOP | WS_VISIBLE | WS_BORDER, 
        170, 70, 60, 25,
        IDOK, 
        "OK",
        0
    }
};

static int
ImageViewerProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {

    case MSG_INITDIALOG:
    {
        HWND hScrollWnd;

        hScrollWnd = GetDlgItem (hDlg, IDC_SCROLLWND);

        SendMessage (hScrollWnd, SVM_SETCONTAINERPROC, 0, 
                        (LPARAM)pic_container_proc);
        SendMessage (hScrollWnd, SVM_SETCONTRANGE, 
                        640, 480);

        SendMessage (hScrollWnd, SVM_ADDCTRLS, 3, (LPARAM)controls);
        break;
    }

    case MSG_CLOSE:
        EndDialog (hDlg, 0);
        return 0;

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static CTRLDATA CtrlViewer[] =
{
    {
        "scrollwnd",
        WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
         10, 10, 300, 200,
        IDC_SCROLLWND,
        "Scroll Window",
        0
    }
};

static DLGTEMPLATE DlgViewer =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 350, 280,
    "Controls in a ScrollWnd",
    0, 0,
    TABLESIZE(CtrlViewer), CtrlViewer,
    0
};

int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "scrollwnd" , 0 , 0);
#endif
    
    DialogBoxIndirectParam (&DlgViewer, HWND_DESKTOP, ImageViewerProc, 0L);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

